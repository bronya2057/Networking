#pragma once

#include <WinsockHelper.hpp>

int example1()
{
   INIT_WINSOCK()

   printf("Configuring local address...\n");
   addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM; //TCP, for UDP SOCK_DGRAM 
   hints.ai_flags = AI_PASSIVE; //listen on any available network interface

   struct addrinfo* bind_address;
   getaddrinfo(0, "8080", &hints, &bind_address); //8080 arbitrary getaddrinfo() is protocol-independent (v4 or v6)
                                                  //only one program can bind to a particular port at a time

   printf("Creating socket...\n");
   SOCKET socket_listen;
   socket_listen = socket(bind_address->ai_family,
      bind_address->ai_socktype, bind_address->ai_protocol); //Gen socket

   if (!ISVALIDSOCKET(socket_listen)) {
      fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }

   printf("Binding socket to local address...\n");
   if (bind(socket_listen,
      bind_address->ai_addr, bind_address->ai_addrlen)) {
      fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }
   freeaddrinfo(bind_address);


   printf("Listening...\n");
   if (listen(socket_listen, 10) < 0) { //queue up to 10 connection
      fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }

   printf("Waiting for connection...\n");
   struct sockaddr_storage client_address; //client info
   socklen_t client_len = sizeof(client_address);
   // block program until a new connection is made
   //Your original socket continues to listen for new connections, 
   //but the new socket returned by accept() can be used to send and 
   //   receive data over the newly established connection. accept() 
   //   also fills in address info of the client that connected.
   SOCKET socket_client = accept(socket_listen,
      (struct sockaddr*) &client_address, &client_len);
   if (!ISVALIDSOCKET(socket_client)) {
      fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }

   printf("Client is connected... ");
   char address_buffer[100];
   getnameinfo((struct sockaddr*)&client_address,
      client_len, address_buffer, sizeof(address_buffer), 0, 0,
      NI_NUMERICHOST);
   printf("%s\n", address_buffer);

   printf("Reading request...\n");
   char request[1024];
   int bytes_received = recv(socket_client, request, 1024, 0); //always check if recv > 0
   printf("Received %d bytes.\n", bytes_received);
   printf("%.*s", bytes_received, request); // print specific number of characters. Recieved data not always 0 terminated

   printf("Sending response...\n");
   const char *response =
      "HTTP/1.1 200 OK\r\n"
      "Connection: close\r\n"
      "Content-Type: text/plain\r\n\r\n"
      "Local time is: ";
   int bytes_sent = send(socket_client, response, strlen(response), 0); //if not the byte count that you expect you should send the rest later
   printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

   time_t timer;
   time(&timer);
   char *time_msg = ctime(&timer);
   bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
   printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

   bytes_sent = send(socket_client, "HelloBoy", strlen("HelloBoy"), 0);

   printf("Closing connection...\n");
   CLOSESOCKET(socket_client); // If we don't close the connection, the browser will just wait for more data until it times out.

   printf("Closing listening socket...\n");
   CLOSESOCKET(socket_listen);

   CLEAR_WINSOCK()
}