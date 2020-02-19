#include <iostream>
#include "served/served.hpp"

using namespace std;


///
/// \brief main
/// \return
///
/// Ideas
/// 1) hacer una clase que sea parametrizable. y que reciba un json en el request, y haga el CRUD a la base de datos
/// 2) hacer clases que vayan procesando, por ejemplo jwt, session, etc
/// 3) hacer clases para reglas de negocios
/// 4) hacer un dispatcher
///
///
int main()
{
    cout << "Hello World!" << endl;

    served::multiplexer mux;

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
