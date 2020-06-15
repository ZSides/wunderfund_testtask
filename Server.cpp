#include "Server.h"
int Server::get_sock() const { return sock; }
Server::Server() {
    sock = bytes_read = -1;
    listener = socket(AF_INET, SOCK_STREAM, 0);
}
void Server::init() {
    if(listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3428);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int enable = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }
    listen(listener, 1);
}
void Server::echo() {
    while(true) {
        sock = accept(listener, NULL, NULL);
        if(sock < 0) {
            perror("accept");
            exit(3);
        }
        while(true) {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) break;
            send(sock, buf, bytes_read, 0);
        }
        close(sock);
    }
}