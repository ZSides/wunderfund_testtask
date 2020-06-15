#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#ifndef WF_TEST_SERVER_H
#define WF_TEST_SERVER_H


class Server {
public:
    Server();
    int get_sock() const;
    void init();
    void echo();
    void run();
private:
    int sock, listener;
    struct sockaddr_in addr{};
    char buf[1024]{};
    int bytes_read;

    fd_set readfds;
    int max_sd, sd, activity, valread, addrlen, new_socket, max_clients, client_socket[64];
};


#endif
