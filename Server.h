#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <time.h>
#include "config.h"

#ifndef WF_TEST_SERVER_H
#define WF_TEST_SERVER_H

class Server {
public:
    Server();
    ~Server();
    void init();
    void run();
    int get_listener() const;
private:
    struct sockaddr_in addr{};
    char buf[1024]{};
    fd_set readfds{};
    int listener, max_sd, sd, activity, valread, addrlen, new_socket, m_ptr, max_clients, client_socket[SERVER_MAX_USERS]{};
    time_t client_timestams[SERVER_MAX_USERS]{};
};


#endif
