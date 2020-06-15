#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#ifndef WF_TEST_SERVER_H
#define WF_TEST_SERVER_H


class Server {
public:
    Server();
    int get_sock() const;
    void init();
    void echo();
private:
    int sock, listener;
    struct sockaddr_in addr{};
    char buf[1024]{};
    int bytes_read;
};


#endif
