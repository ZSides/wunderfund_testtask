#include "Server.h"
int Server::get_listener() const { return listener; }
Server::Server() {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    max_clients = SERVER_MAX_USERS;
    for (int i = 0; i < max_clients; ++i) {
        client_socket[i] = 0;
        client_timestams[i] = 0;
    }
}
void Server::init() {
    if(listener < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(listener, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}
void Server::run() {
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(listener, &readfds);
        max_sd = listener;

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if(sd > max_sd) {
                max_sd = sd;
            }
        }
        activity = select( max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            printf("select error");
        }
        if (FD_ISSET(listener, &readfds)) {
            if ((new_socket = accept(listener, (struct sockaddr *)&addr, (socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("New connection! socket fd: %d, ip: %s, port: %d\n", new_socket, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            for (int i = 0; i < max_clients; i++) {
                if(client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buf,1024)) == 0) {
                    getpeername(sd, (struct sockaddr*)&addr, (socklen_t*)&addrlen);
                    printf("Host disconnected. ip: %s, port: %d \n", inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port));
                    close(sd);
                    client_socket[i] = 0;
                }
                else {
                    m_ptr = 0;
                    for (int j = 0; j < valread; ++j) {
                        if (buf[j] < 32 || buf[j] == 127) {
                            m_ptr++;
                        } else if (m_ptr > 0) {
                            buf[j - m_ptr] = buf[j];
                        }
                    }
                    valread -= m_ptr - 1;
                    buf[valread - 1] = '\n';
                    buf[valread] = '\0';
                    if (strlen(buf) > 180) {
                        send(client_socket[i], ERROR_MSG_LEN, strlen(ERROR_MSG_LEN), 0);
                    } else if (difftime(time(nullptr), client_timestams[i]) <= 1) {
                        send(client_socket[i], ERROR_MSG_TIME, strlen(ERROR_MSG_TIME), 0);
                    } else {
                        for (int j = 0; j < max_clients; ++j) {
                            if (client_socket[j] == 0) {
                                break;
                            }
                            if (i != j) {
                                send(client_socket[j], buf,strlen(buf),0);
                            }
                        }
                    }
                    client_timestams[i] = time(nullptr);
                }
            }
        }
    }
}