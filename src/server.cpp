#include <iostream>
#include <httplib.h>

#include "data.hpp"

int main() {
    Data data;

    httplib::Server svr;
    svr.Get("/data", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            data.read();
        }
        catch (const std::exception& e) {
            std::cout << e.what() << '\n';
        }

        res.set_content(data.serve(), "application/json");
    });

    std::cout << "Data provider server running\n";
    svr.listen("localhost", 1337);

    return 0;
}
