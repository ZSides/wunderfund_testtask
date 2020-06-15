#include "Server.h"
int Server::get_sock() const { return sock; }
Server::Server() {
    sock = bytes_read = -1;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    max_clients = 64;
    for (int i = 0; i < max_clients; ++i) {
        client_socket[i] = 0;
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
    addr.sin_port = htons(3428);
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

        //add child sockets to set
        for (int i = 0 ; i < max_clients ; i++) {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(listener, &readfds))
        {
            if ((new_socket = accept(listener, (struct sockaddr *)&addr, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection! socket fd: %d, ip: %s , port: %d\n" , new_socket , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port));

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (int i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buf, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&addr , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                           inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }

                    //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buf[valread] = '\0';
                    send(sd, buf, strlen(buf) , 0 );
                }
            }
        }
    }
}
void Server::echo() {
    while(true) {
        sock = accept(listener, nullptr, nullptr);
        if(sock < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        while(true) {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) break;
            send(sock, buf, bytes_read, 0);
        }
        close(sock);
    }
}