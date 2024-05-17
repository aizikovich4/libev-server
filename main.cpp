//improve - need to make the  port nonblocking
//add the handle of SIGSTOP - in current server its no need, but in real life we must to do something (for example: correct close files, free resources, send notifications)

#include <stdio.h>
#include <netinet/in.h>
#include <ev.h>
#include <memory>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>

#include "main.hpp"

constexpr unsigned int PORT_NO = 5000;
constexpr unsigned int BUFFER_SIZE = 1024;

static std::exception_ptr server_exception = nullptr;

void server()
{
    //initialise the server and run
    try
        {
        struct ev_loop *loop = EV_DEFAULT;
        int socked_descriptor = -1;

        struct ev_io server_watcher;

        // Create server socket
        if( (socked_descriptor = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
        {
          throw std::runtime_error("socket error");
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT_NO);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(socked_descriptor, (sockaddr*) &addr, sizeof(addr)) != 0) {
            throw std::runtime_error("bind error");
        }

        if (listen(socked_descriptor, SOMAXCONN) < 0) {
          throw std::runtime_error("listen error");
        }

        ev_io_init(&server_watcher, accept_new_client, socked_descriptor, EV_READ);
        ev_io_start(loop, &server_watcher);
        std::cout << "Server started on port: " << PORT_NO << std::endl;

        ev_run(loop, 0);
    }
    catch (std::runtime_error& e) {
        server_exception = std::current_exception();
    }
}

int main()
{
    //run server in separate thread, because main thread have other important task - it do nothing
    std::thread server_thread(server);
    server_thread.join();

    if (server_exception) {
        try{
            std::rethrow_exception(server_exception);
        }
        catch(const std::exception &ex)
        {
            std::cerr << "Thread exited with exception: " << ex.what() << "\n";
        }
    }

    return 0;
}

void accept_new_client(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sd;
    ev_io *client_watcher = (struct ev_io*) malloc (sizeof(struct ev_io));

    if(EV_ERROR & revents) {
      std::cout << "invalid event" << std::endl;
      return;
    }

    client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_sd < 0) {
        std::cout << "accept error" << std::endl;
        return;
    }

    auto time = std::time(nullptr);
    std::cout <<  "Client connected: " << std::put_time(std::localtime(&time), "%F %T%z") << std::endl;

    ev_io_init(client_watcher, read_data, client_sd, EV_READ);
    ev_io_start(loop, client_watcher);
}


void read_data(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    char buffer[BUFFER_SIZE];
    ssize_t read_size;

    if(EV_ERROR & revents) {
      std::cout << "error event" << std::endl;
      return;
    }

    // Receive message from client socket
    read_size = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

    auto free_watcher = [&loop, &watcher](){
        ev_io_stop(loop,watcher);
        free(watcher);
    };

    if(read_size < 0) {
      std::cout << "read error, may be client already stopped" << std::endl;
      free_watcher();
      return;
    }

    if(read_size == 0) {
      free_watcher();
      std::cout << "client stopped" << std::endl;
      return;
    }

    //using loop, because we can receive the binary data
    // also need to understand the format of incoming data  - binary or char, but in this case we use only chars for simplify
    std::cout << "Received:";
    for (int i=0; i < read_size; ++i) {
        std::cout << buffer[i];
    }
    std::cout << std::endl;

    //to client
    send(watcher->fd, buffer, read_size, 0);
}
