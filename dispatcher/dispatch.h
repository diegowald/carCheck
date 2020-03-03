#ifndef DISPATCH_H
#define DISPATCH_H

#include <string>
#include "../served/src/served/served.hpp"
#include <asio/streambuf.hpp>

class Dispatch
{
public:
    Dispatch(const std::string &name,
             const std::string &path,
             const std::string &resolverURL,
             const std::string &resolverPort,
             const std::string &resolverPath);

    virtual ~Dispatch();

    void setup(served::multiplexer &multiplexer);

protected:
    virtual void get(served::response & res, const served::request & req);
    virtual void getById(served::response &res, const served::request &req);
    virtual void post(served::response &res, const served::request &req);
    virtual void del(served::response &res, const served::request &req);
    virtual void update(served::response &res, const served::request &req);

private:
    void parseResponse(served::response &res, asio::streambuf &httpBuffer);
private:
    std::string _name;
    std::string _path;
    std::string _resolverURL;
    std::string _resolverPort;
    std::string _resolverPath;
};

#endif // DISPATCH_H
