#include <iostream>
#include "httplib.h"

int main() {
    httplib::Server svr;
    svr.Get("/data", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello world!", "text/plain");
    });

    return 0;
}