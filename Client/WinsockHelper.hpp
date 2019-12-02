#pragma once

#ifndef _WIN32_WINNT //  _WIN32_WINNT macro must be defined first so that the proper version of the Windows headers are included
#define _WIN32_WINNT 0x0600
#endif

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h> //MemoryAllocation

#pragma comment(lib, "ws2_32.lib") //link the executable with the Winsock library, ws2_32.lib
#pragma comment(lib, "iphlpapi.lib")

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
return linked list of adapters on this PC, then could be queried if not null
*/
PIP_ADAPTER_ADDRESSES getPCAdapters();

/*
print IPV6 IPV4 and name of each adapter to console
*/
void printAdaptersInfo(PIP_ADAPTER_ADDRESSES adapter);