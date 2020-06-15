#include "Server.h"
#include <thread>
#include <string>
#include <iostream>

int main() {
    Server server;
    server.init();
    std::thread thread([&server]() {
        server.run();
    });
    thread.detach();
    std::string action;
    while(true) {
        std::cin >> action;
        if (action == "quit") {
            close(server.get_listener());
            return 0;
        }
    }
}
