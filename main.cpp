#include "Server.h"
int main() {
    Server server;
    auto run_server = [server]() {
        while(true) {
            sock = accept(listener, NULL, NULL);
            if(sock < 0) {
                perror("accept");
                exit(3);
            }

            while(true) {
                bytes_read = recv(sock, buf, 1024, 0);
                if(bytes_read <= 0) break;
                std::cout << buf << '\n';
                //send(sock, buf, bytes_read, 0);
            }

            close(sock);
        }
    };
    std::thread thread(run_server);
    thread.detach();
    std::string action;
    while(true) {
        std::cin >> action;
        if (action == "quit") {
            close(server.get_sock());
            return 0;
        }
    }
    return 0;
}
