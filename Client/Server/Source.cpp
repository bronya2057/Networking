/*win_list.c*/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <WinsockHelper.hpp>

#pragma warning(disable:4996)

//constexpr char* hostname = "example.com";
constexpr char* port = "8080";

int main() 
{
   INIT_WINSOCK()
   
   addrinfo *peer_address;
   if (fillLocalAddressInfo(port, peer_address) != SUCCESS_CODE) return ERROR_CODE;

   SOCKET socket_listen;
   if (createSocket(peer_address, socket_listen) != SUCCESS_CODE) return ERROR_CODE;

   printf("Connecting...\n");
   if (connectLocalSocket(peer_address, socket_listen) != SUCCESS_CODE) return ERROR_CODE;
   freeaddrinfo(peer_address  );
   const int numberOfConnections = 10;  
   printf("Listening...\n");
   if (listenSocket(socket_listen, numberOfConnections) != SUCCESS_CODE) return ERROR_CODE;

   fd_set master;
   FD_ZERO(&master);
   FD_SET(socket_listen, &master);
   SOCKET max_socket = socket_listen;

   while (1) {
      fd_set reads;
      reads = master;
      if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
         fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
         return 1;
      }

      SOCKET i;
      for (i = 1; i <= max_socket; ++i) {
         if (FD_ISSET(i, &reads)) {

            if (i == socket_listen) {
               struct sockaddr_storage client_address;
               socklen_t client_len = sizeof(client_address);
               SOCKET socket_client = accept(socket_listen,
                  (struct sockaddr*) &client_address,
                  &client_len);
               if (!ISVALIDSOCKET(socket_client)) {
                  fprintf(stderr, "accept() failed. (%d)\n",
                     GETSOCKETERRNO());
                  return 1;
               }

               FD_SET(socket_client, &master);
               if (socket_client > max_socket)
                  max_socket = socket_client;

               char address_buffer[100];
               getnameinfo((struct sockaddr*)&client_address,
                  client_len,
                  address_buffer, sizeof(address_buffer), 0, 0,
                  NI_NUMERICHOST);
               printf("New connection from %s\n", address_buffer);

            }
            else {
               char read[1024];
               int bytes_received = recv(i, read, 1024, 0);
               if (bytes_received < 1) {
                  FD_CLR(i, &master);
                  CLOSESOCKET(i);
                  continue;
               }

               int j;
               for (j = 0; j < bytes_received; ++j)
                  read[j] = toupper(read[j]);
               send(i, read, bytes_received, 0);
            }

         } //if FD_ISSET
      } //for i to max_socket
   } //while(1)
   CLEAR_WINSOCK()
}