/*win_list.c*/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <WinsockHelper.hpp>

int main() {

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

   CLEAR_WINSOCK()
}