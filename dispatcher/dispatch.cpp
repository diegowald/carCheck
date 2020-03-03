#include "dispatch.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include "../served/src/served/request_parser_impl.hpp"

Dispatch::Dispatch(const std::string& name,
                   const std::string& path,
                   const std::string& resolverURL,
                   const std::string& resolverPort,
                   const std::string& resolverPath) :
    _name(name), _path(path), _resolverURL(resolverURL),
    _resolverPort(resolverPort), _resolverPath(resolverPath)
{
}

Dispatch::~Dispatch()
{}

void Dispatch::setup(served::multiplexer &multiplexer)
{

    multiplexer.handle(_path + "s").get(
                [this](served::response & res, const served::request & req) {
        get(res, req);
    });

    std::string pathById = _path + "/{id}";

    multiplexer.handle(pathById).get([this](served::response &res, const served::request &req) {
        getById(res, req);
    });

    multiplexer.handle(_path).post(
                [this](served::response &res, const served::request &req) {
        post(res, req);
    });

    multiplexer.handle(pathById).del(
                [this](served::response &res, const served::request &req) {
        del(res, req);
    });

    multiplexer.handle(pathById).put(
                [this](served::response &res, const served::request &req) {
        update(res, req);
    });
}

void Dispatch::get(served::response & res, const served::request & req)
{
    // Performs an HTTP GET and prints the response
    try
    {
        auto const host = _resolverURL;
        auto const port = _resolverPort;
        auto const target = _resolverPath;
        int version = 11; //argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11;

        // The io_context is required for all I/O
        boost::asio::io_context ioc;

        // These objects perform our I/O
        boost::asio::ip::tcp::resolver resolver{ioc};
        boost::asio::ip::tcp::socket socket{ioc};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(socket, results.begin(), results.end());

        // Set up an HTTP GET request message
        boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::get, target, version};
        req.set(boost::beast::http::field::host, host);
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        boost::beast::http::write(socket, req);

        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;

        // Declare a container to hold the response
        boost::beast::http::response<boost::beast::http::dynamic_body> response;

        // Receive the HTTP response
        boost::beast::http::read(socket, buffer, response);
        
        served::request req2;
        served::request_parser_impl req_parser(req2);
        // Write the message to standard out
        std::cout << response << std::endl;

        std::cout << response.base() << std::endl;
        std::cout << response.base().reason() << std::endl;
        std::cout << response.base().result_int() << std::endl;
        std::cout << response.base().version() << std::endl;

        std::cout << response.body().size() << std::endl;
//        std::cout << boost::asio::buffer_cast<const char*>(response.body().data()) << std::endl;
        //std::cout << response.body().data() << std::endl;
        std::string body;
        for (auto seq : response.body().data()) {
          auto* cbuf = boost::asio::buffer_cast<const char*>(seq);
          body.append(cbuf, boost::asio::buffer_size(seq));
        }
        std::cout << body << std::endl;

        res.set_status(response.base().result_int());

        res.set_body(body);

        // Gracefully close the socket
        boost::system::error_code ec;
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != std::errc::not_connected)
            throw std::system_error{ec};

        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        //return EXIT_FAILURE;
    }
    //return EXIT_SUCCESS;
}

void Dispatch::getById(served::response &res, const served::request &req)
{
    std::cout << req.url().URI() << std::endl;
    std::cout << req.url().path() << std::endl;
    std::cout << req.url().query() << std::endl;
    std::cout << req.url().fragment() << std::endl;
    res << "hola";
}

void Dispatch::post(served::response &res, const served::request &req)
{}

void Dispatch::del(served::response &res, const served::request &req)
{}

void Dispatch::update(served::response &res, const served::request &req)
{}

void Dispatch::parseResponse(served::response &res, asio::streambuf &httpBuffer)
{/*
    std::istream buffer(&httpBuffer);
    std::string line;

    // parsing the headers
    while (getline(buffer, line, '\n')) {
        if (line.empty() || line == "\r") {
            break; // end of headers reached
        }
        if (line.back() == '\r') {
            line.resize(line.size()-1);
        }
        // simply ignoring headers for now
        std::cout << "Ignore header: " << std::quoted(line) << "\n";
    }


    std::string const body(std::istreambuf_iterator<char>{buffer}, {});

    std::cout << "Parsed content: " << std::quoted(body) << "\n";*/
}

/*
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

// Performs an HTTP GET and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        if(argc != 4 && argc != 5)
        {
            std::cerr <<
                "Usage: http-client-sync <host> <port> <target> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
                "Example:\n" <<
                "    http-client-sync www.example.com 80 /\n" <<
                "    http-client-sync www.example.com 80 / 1.0\n";
            return EXIT_FAILURE;
        }
        auto const host = argv[1];
        auto const port = argv[2];
        auto const target = argv[3];
        int version = argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11;

        // The io_context is required for all I/O
        boost::asio::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        tcp::socket socket{ioc};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(socket, results.begin(), results.end());

        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(socket, req);

        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(socket, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;

        // Gracefully close the socket
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != boost::system::errc::not_connected)
            throw boost::system::system_error{ec};

        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
*/


/*#include <iostream>
#include <iomanip>
#include <boost/asio.hpp>

int main() {
    boost::asio::streambuf request;
    {
        std::ostream sample(&request);
        sample <<
            "POST /cgi-bin/process.cgi HTTP/1.1\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
            "Host: www.tutorialspoint.com\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-Length: 49\r\n"
            "Accept-Language: en-us\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: Keep-Alive\r\n"
            "\r\n"
            "licenseID=string&content=string&/paramsXML=string"
            ;
    }

    std::istream buffer(&request);
    std::string line;

    // parsing the headers
    while (getline(buffer, line, '\n')) {
        if (line.empty() || line == "\r") {
            break; // end of headers reached
        }
        if (line.back() == '\r') {
            line.resize(line.size()-1);
        }
        // simply ignoring headers for now
        std::cout << "Ignore header: " << std::quoted(line) << "\n";
    }

    std::string const body(std::istreambuf_iterator<char>{buffer}, {});

    std::cout << "Parsed content: " << std::quoted(body) << "\n";
}*/
