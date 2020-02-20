#include "crudbase.h"

#include <iostream>
#include <memory>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset_metadata.h>

CRUDBase::CRUDBase(const std::string &listenerName, const std::string &path, const std::string &host,
                   const std::string &user, const std::string &password,
                   const std::string &port, const std::string &database,
                   const std::string &table, bool handleGet, bool handleGetById, bool handlePost,
                   bool handleDelete, bool handleUpdate) :
    _name(listenerName), _path(path), _host(host), _user(user), _password(password),
    _port(port), _database(database), _table(table),
    _handleGet(handleGet), _handleGetById(handleGetById), _handlePost(handlePost),
    _handleDelete(handleDelete), _handleUpdate(handleUpdate)
{
    std::cout << "Handler created. Name" << _name << std::endl
              << "\tPath: " << _path << std::endl
              << "\tDatabase: " << _database << ", Table: " << _table << std::endl;
}

CRUDBase::~CRUDBase()
{
}

void CRUDBase::setup(served::multiplexer &multiplexer)
{
    if (_handleGet)
    {
        multiplexer.handle(_path + "s").get(
                    [this](served::response & res, const served::request & req) {
            get(res, req);
        });
    }

    std::string pathById = _path + "/{id}";

    if (_handleGetById)
    {
        multiplexer.handle(pathById).get([this](served::response &res, const served::request &req) {
            getById(res, req);
        });
    }

    if (_handlePost)
    {
        multiplexer.handle(_path).post(
                    [this](served::response &res, const served::request &req) {
            post(res, req);
        });
    }

    if (_handleDelete)
    {
        multiplexer.handle(pathById).del(
                    [this](served::response &res, const served::request &req) {
            del(res, req);
        });
    }

    if (_handleUpdate)
    {
        multiplexer.handle(pathById).put(
                    [this](served::response &res, const served::request &req) {
            update(res, req);
        });
    }
}


void CRUDBase::get(served::response & res, const served::request & req)
{
    try {
      sql::Driver *driver;

      /* Create a connection */
      driver = get_driver_instance();

      std::stringstream ss;
      ss << "tcp://" << _host << ":" << _port;

      std::unique_ptr<sql::Connection> con(driver->connect(ss.str(), _user, _password));
      /* Connect to the MySQL test database */
      con->setSchema(_database);

      std::unique_ptr<sql::Statement> stmt(con->createStatement());

      ss.str(std::string());
      ss << "SELECT JSON_ARRAYAGG(doc) as doc FROM " << _table << ";";

      std::unique_ptr<sql::ResultSet> resultSet(stmt->executeQuery(ss.str()));

      sql::ResultSetMetaData *md = resultSet->getMetaData();

      while (resultSet->next()) {
        std::cout << "\t... MySQL replies: ";
        /* Access column data by alias or column name */
        std::cout << resultSet->getString("doc") << std::endl;
        std::cout << "\t... MySQL says it again: ";
        /* Access column data by numeric offset, 1 is the first column */
        std::cout << resultSet->getString(1) << std::endl;
        res << resultSet->getString("doc");
      }
    } catch (sql::SQLException &e) {
      std::cout << "# ERR: SQLException in " << __FILE__;
      std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
      std::cout << "# ERR: " << e.what();
      std::cout << " (MySQL error code: " << e.getErrorCode();
      std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
      res.set_status(400);
    }

    std::cout << std::endl;
}

void CRUDBase::getById(served::response &res, const served::request &req)
{
    try {
      sql::Driver *driver;

      /* Create a connection */
      driver = get_driver_instance();

      std::stringstream ss;
      ss << "tcp://" << _host << ":" << _port;

      std::unique_ptr<sql::Connection> con(driver->connect(ss.str(), _user, _password));
      /* Connect to the MySQL test database */
      con->setSchema(_database);

      std::unique_ptr<sql::Statement> stmt(con->createStatement());

      ss.str(std::string());
      ss << "SELECT JSON_ARRAYAGG(doc) as doc FROM " << _table << " WHERE id = " << req.params["id"] << ";";

      std::unique_ptr<sql::ResultSet> resultSet(stmt->executeQuery(ss.str()));

      sql::ResultSetMetaData *md = resultSet->getMetaData();

      while (resultSet->next()) {
        std::cout << "\t... MySQL replies: ";
        /* Access column data by alias or column name */
        std::cout << resultSet->getString("doc") << std::endl;
        std::cout << "\t... MySQL says it again: ";
        /* Access column data by numeric offset, 1 is the first column */
        std::cout << resultSet->getString(1) << std::endl;
        res.set_body(resultSet->getString("doc"));
      }
    } catch (sql::SQLException &e) {
      std::cout << "# ERR: SQLException in " << __FILE__;
      std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
      std::cout << "# ERR: " << e.what();
      std::cout << " (MySQL error code: " << e.getErrorCode();
      std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    std::cout << std::endl;
}

void CRUDBase::post(served::response &res, const served::request &req)
{
    try {
      sql::Driver *driver;

      /* Create a connection */
      driver = get_driver_instance();

      std::stringstream ss;
      ss << "tcp://" << _host << ":" << _port;

      std::unique_ptr<sql::Connection> con(driver->connect(ss.str(), _user, _password));
      /* Connect to the MySQL test database */
      con->setSchema(_database);


      ss.str(std::string());
      ss << "INSERT INTO " << _table << " (doc) VALUES (?)";

      std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(ss.str()));
      stmt->setString(1, req.body());

      bool result = stmt->execute();

      res.set_body((result ? "true" : "false"));

    } catch (sql::SQLException &e) {
      std::cout << "# ERR: SQLException in " << __FILE__;
      std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
      std::cout << "# ERR: " << e.what();
      std::cout << " (MySQL error code: " << e.getErrorCode();
      std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    std::cout << std::endl;
}

void CRUDBase::del(served::response &res, const served::request &req)
{
    try {
      sql::Driver *driver;

      /* Create a connection */
      driver = get_driver_instance();

      std::stringstream ss;
      ss << "tcp://" << _host << ":" << _port;

      std::unique_ptr<sql::Connection> con(driver->connect(ss.str(), _user, _password));
      /* Connect to the MySQL test database */
      con->setSchema(_database);


      ss.str(std::string());
      ss << "DELETE FROM " << _table << " WHERE id = ?";

      std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(ss.str()));
      stmt->setUInt64(1, std::atol(req.params["id"].c_str()));

      bool result = stmt->execute();

      res.set_body((result ? "true" : "false"));

    } catch (sql::SQLException &e) {
      std::cout << "# ERR: SQLException in " << __FILE__;
      std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
      std::cout << "# ERR: " << e.what();
      std::cout << " (MySQL error code: " << e.getErrorCode();
      std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    std::cout << std::endl;
}

void CRUDBase::update(served::response &res, const served::request &req)
{
    try {
      sql::Driver *driver;

      /* Create a connection */
      driver = get_driver_instance();

      std::stringstream ss;
      ss << "tcp://" << _host << ":" << _port;

      std::unique_ptr<sql::Connection> con(driver->connect(ss.str(), _user, _password));
      /* Connect to the MySQL test database */
      con->setSchema(_database);


      ss.str(std::string());
      ss << "UPDATE " << _table << " SET doc = ? WHERE id = ?";

      std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(ss.str()));
      stmt->setString(1, req.body());
      stmt->setInt64(2, std::atol(req.params["id"].c_str()));

      bool result = stmt->execute();

      res.set_body((result ? "true" : "false"));

    } catch (sql::SQLException &e) {
      std::cout << "# ERR: SQLException in " << __FILE__;
      std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
      std::cout << "# ERR: " << e.what();
      std::cout << " (MySQL error code: " << e.getErrorCode();
      std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    std::cout << std::endl;
}
