#include <iostream>
#include <fstream>
#include "served/served.hpp"
#include "crudbase.h"
#include <list>
#include <memory>

#include <served/plugins.hpp>
#include <../rapidjson/include/rapidjson/document.h>

using namespace std;

std::string readConfigFile()
{
    std::ifstream t("config.json");
    std::string str;

    t.seekg(0, std::ios::end);
    if (!t.is_open())
    {
        return "";
    }
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
    return str;
}

void setup(rapidjson::Document& jsonDoc,
           std::list<std::shared_ptr<CRUDBase>>& resolvers,
           std::string& address, std::string& port,
           int& numThreads,
           served::multiplexer& mux)
{
    rapidjson::Value& v = jsonDoc["server"];
    address = v.GetString();

    v = jsonDoc["port"];
    port = v.GetString();

    numThreads = jsonDoc["numThreads"].GetInt();

    rapidjson::Value& resolversValue = jsonDoc["resolvers"];
    assert(resolversValue.IsArray()); // attributes is an array
    for (rapidjson::Value::ConstValueIterator itr = resolversValue.Begin(); itr != resolversValue.End(); ++itr) {
        const rapidjson::Value& resolver = *itr;
        assert(resolver.IsObject()); // each attribute is an object
        /*for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin(); itr2 != attribute.MemberEnd(); ++itr2) {
            std::cout << itr2->name.GetString() << " : " << itr2->value.GetString() << std::endl;
        }*/
        std::string listenerName = resolver["name"].GetString();
        std::string path = resolver["path"].GetString();
        std::string dbHost = resolver["DBhost"].GetString();
        std::string dbUser = resolver["DBuser"].GetString();
        std::string dbPassword = resolver["DBpassword"].GetString();
        std::string dbPort = resolver["DBport"].GetString();
        std::string dbName = resolver["databaseName"].GetString();
        std::string tableName = resolver["tableName"].GetString();
        bool handleGet = resolver["handleGet"].GetBool();
        bool handleGetById = resolver["handleGetById"].GetBool();
        bool handlePost = resolver["handlePost"].GetBool();
        bool handleDelete = resolver["handleDelete"].GetBool();
        bool handleUpdate = resolver["handleUpdate"].GetBool();
        shared_ptr<CRUDBase> obj = std::make_shared<CRUDBase>(listenerName, path, dbHost, dbUser,
                                                       dbPassword, dbPort,
                                                       dbName, tableName,
                                                       handleGet, handleGetById,
                                                       handlePost, handleDelete, handleUpdate);
        obj->setup(mux);
        resolvers.push_back(obj);
    }
}



int main()
{
    cout << "REST SQL Service!" << endl;


    std::string configJson = readConfigFile();

    if (configJson.empty())
    {
        cout << "Please use config.json file to setup the service" << endl;
        return 1;
    }


    cout << "Reading configuration file" << endl;
    rapidjson::Document d;
    d.Parse(configJson.c_str());

    served::multiplexer mux;
    mux.use_after(served::plugin::access_log);

    std::list<std::shared_ptr<CRUDBase>> resolvers;
    std::string address;
    std::string port;
    int numThreads;

    setup(d, resolvers, address, port, numThreads, mux);

    cout << "Starting Service" << endl;

    // Create the server and run with 10 handler threads.
    served::net::server server(address, port, mux);
    server.run(numThreads);

    return 0;
}
