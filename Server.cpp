#include "Server.h"
int Server::get_sock() const { return sock; }
void Server::set_sock(int s) { sock = s; }
Server::Server() {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }
    listen(listener, 1);
    sock = -1;
    bytes_read = 0;
}