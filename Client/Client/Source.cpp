/*win_list.c*/

#include <WinsockHelper.hpp>

#include <string>

constexpr char* hostname = "127.0.0.1";//"example.com";
constexpr char* port = "8080";

int main(int argc, char *argv[]) {

   INIT_WINSOCK()
   if (PIP_ADAPTER_ADDRESSES adapters = getPCAdapters())
   {
      printAdaptersInfo(adapters);
      free(adapters);
   }
   else
   {
      free(adapters);
      return -1;
   }

   addrinfo *peer_address;
   if (fillAddressInfo(hostname, port, peer_address) != SUCCESS_CODE) return ERROR_CODE;
   
   SOCKET socket_peer;
   if (createSocket(peer_address, socket_peer) != SUCCESS_CODE) return ERROR_CODE;

   while (connectRemoteSocket(peer_address, socket_peer) != SUCCESS_CODE)
   {

   }

   printf("Connecting...\n");
   //if (connectRemoteSocket(peer_address, socket_peer) != SUCCESS_CODE) return ERROR_CODE;

   freeaddrinfo(peer_address); //don't need the peer_address anymore

   while (1)
   {
      fd_set reads;
      FD_ZERO(&reads);
      FD_SET(socket_peer, &reads);

      struct timeval timeout;
      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;

      if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
         fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
         return 1;
      }

      if (FD_ISSET(socket_peer, &reads)) {
         char read[4096];
         int bytes_received = recv(socket_peer, read, 4096, 0); // <1 then connection closed
         if (bytes_received < 1) {
            printf("Connection closed by peer.\n");
            break;
         }
         printf("Received (%d bytes): %.*s",
            bytes_received, bytes_received, read); //print string of specific length
      }

      if (_kbhit()) { //console input waits?
         char read[4096];
         if (!fgets(read, 4096, stdin)) break;
         printf("Sending: %s", read);
         int bytes_sent = send(socket_peer, read, strlen(read), 0); //ignore errors on send, handle on recv
         printf("Sent %d bytes.\n", bytes_sent);
      }

   }

   CLOSESOCKET(socket_peer);
   CLEAR_WINSOCK()
}