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

   CFdSet fdSet;
   fdSet.zero();
   fdSet.addSocket(socket_listen);

   while (1) {
      fd_set reads;
      reads = fdSet.getSet();

      if (select(fdSet.getMaxSocket() + 1, &reads, 0, 0, 0) < 0) {
         fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
         return 1;
      }

      SOCKET i;
      for (i = 1; i <= fdSet.getMaxSocket(); ++i) {
         if (FD_ISSET(i, &reads)) {

            if (i == socket_listen) {
               SOCKET socket_client = acceptClientSocket(socket_listen);

               if (!ISVALIDSOCKET(socket_client)){return 1;}

               fdSet.addSocket(socket_client);
            }
            else {
               char read[1024];
               int bytes_received = recv(i, read, 1024, 0);
               if (bytes_received < 1) {
                  fdSet.clear(i);
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