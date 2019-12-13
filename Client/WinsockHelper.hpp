#pragma once

#ifndef _WIN32_WINNT //  _WIN32_WINNT macro must be defined first so that the proper version of the Windows headers are included
#define _WIN32_WINNT 0x0600
#endif

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h> //MemoryAllocation
#include <conio.h>

#pragma comment(lib, "ws2_32.lib") //link the executable with the Winsock library, ws2_32.lib
#pragma comment(lib, "iphlpapi.lib")

static constexpr int ERROR_CODE = 1;
static constexpr int SUCCESS_CODE = 0;

//WSADATA structure will be filled in by WSAStartup() with details about the Windows Sockets implementation 
//Version 2.2
#define INIT_WINSOCK() WSADATA d;         \
   if (WSAStartup(MAKEWORD(2, 2), &d)) {  \
      printf("Failed to initialize.\n");  \
      return -1;                          \
   }

#define CLEAR_WINSOCK() WSACleanup();\
                        printf("Ok.\n");\
                        system("pause");\
                        return 0;\

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#endif

#if defined(_WIN32)
#define CLOSESOCKET(s) closesocket(s)
#endif

#if defined(_WIN32)
#define GETSOCKETERRNO() (WSAGetLastError())
#endif

/*
*  return linked list of adapters on this PC, then could be queried if not null
*/
PIP_ADAPTER_ADDRESSES getPCAdapters();

/*
*  print IPV6 IPV4 and name of each adapter to console
*/
void printAdaptersInfo(PIP_ADAPTER_ADDRESSES adapter);

/*
*  fill the passed peer_address with given hints (hostname, port)
*  if you pass pointer, then the old pointer will not point anywhere and the copied pointer will do
*/
int fillAddressInfo(const char* hostname, const char* port, addrinfo*& peer_address);

/*
*  fill the passed peer_address with given hints (hostname, port)
*  if you pass pointer, then the old pointer will not point anywhere and the copied pointer will do
*/
int fillLocalAddressInfo(const char* port, addrinfo*& peer_address);

/*
*  fill the passed socket with recently generated peer_address
*/
int createSocket(addrinfo* peer_address, SOCKET& socket_peer);

/*
*  connect socket to the peer_address data remote 
*/
int connectRemoteSocket(addrinfo* peer_address, SOCKET& socket_peer);

/*
*  connect socket to the local_address data locally
*/
int connectLocalSocket(addrinfo* bind_address, SOCKET& socket_listen);

/*
*  listen given socket with number of connections
*/
int listenSocket(SOCKET& socket_listen, int numberOfConnections);

/*
*  Accept client connection from given listening socket. Return client socket.
*/
SOCKET acceptClientSocket(SOCKET& socket_listen);

class CFdSet
{
public:
   CFdSet();
   void addSocket(SOCKET& socket);
   SOCKET getMaxSocket() const;
   void zero();
   fd_set getSet() const;
   void clear(SOCKET& socketNumber);

private:
   fd_set mMaster;
   SOCKET mMaxSocket;
};