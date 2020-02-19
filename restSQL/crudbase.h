#ifndef CRUDBASE_H
#define CRUDBASE_H

#include "served/served.hpp"
#include <string>

class CRUDBase
{
public:
    CRUDBase(const std::string &path,
             const std::string &host,
             const std::string &user,
             const std::string &password,
             const std::string &port,
             const std::string &database,
             const std::string &table,
             bool handleGet, bool handleGetById, bool handlePost, bool handleDelete, bool handleUpdate);

    virtual ~CRUDBase();

    void setup(served::multiplexer &multiplexer);


protected:
    virtual void get(served::response & res, const served::request & req);
    virtual void getById(served::response &res, const served::request &req);
    virtual void post(served::response &res, const served::request &req);
    virtual void del(served::response &res, const served::request &req);
    virtual void update(served::response &res, const served::request &req);

private:
    std::string _path;
    std::string _host;
    std::string _user;
    std::string _password;
    std::string _port;
    std::string _database;
    std::string _table;
    bool _handleGet;
    bool _handleGetById;
    bool _handlePost;
    bool _handleDelete;
    bool _handleUpdate;
};

#endif // CRUDBASE_H
