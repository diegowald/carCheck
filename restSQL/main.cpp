#include <iostream>
#include "served/served.hpp"
#include "crudbase.h"

using namespace std;

int main()
{
    cout << "Hello World!" << endl;

    served::multiplexer mux;

    CRUDBase crudBase("/hello2", "172.17.0.2", "diego", "12345", "3306", "carcheck", "tabla1", true, true, true, false, false);
    crudBase.setup(mux);

    // GET /hello
    mux.handle("/hello")
        .get([](served::response & res, const served::request & req) {
            res << "Hello world!";
        });

    // Create the server and run with 10 handler threads.
    served::net::server server("127.0.0.1", "8080", mux);
    server.run(10);

    return 0;
}
