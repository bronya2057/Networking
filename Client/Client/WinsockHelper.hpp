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

/*
return linked list of adapters on this PC, then could be queried if not null
*/
PIP_ADAPTER_ADDRESSES getPCAdapters()
{
   //asize variable will store the size of our adapters' address buffer
   DWORD asize = 20000;
   PIP_ADAPTER_ADDRESSES adapters;
   do {
      adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);

      if (!adapters) {
         printf("Couldn't allocate %ld bytes for adapters.\n", asize);
         WSACleanup();
      }
      //AF_UNSPEC, tells Windows that we want both IPv4 and IPv6 addresses
      //AF_INET or AF_INET6 to request only IPv4 or only IPv6 addresses.
      //GAA_FLAG_INCLUDE_PREFIX needed
      int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, //we allocate memory for the adapters, and we request the adapters' addresses
         adapters, &asize);
      if (r == ERROR_BUFFER_OVERFLOW) { //return ERROR_BUFFER_OVERFLOW  if not enough asize 
         printf("GetAdaptersAddresses wants %ld bytes.\n", asize);
         free(adapters); //if not enough space. Free adapters and GetAdaptersAddresses will set asize to bigger buffer size automatically
      }
      else if (r == ERROR_SUCCESS) { //No errors
         break;
      }
      else {
         printf("Error from GetAdaptersAddresses: %d\n", r);
         free(adapters);
         WSACleanup();
      }
   } while (!adapters);

   return adapters;
}

/*
print IPV6 IPV4 and name of each adapter to console
*/
void printAdaptersInfo(PIP_ADAPTER_ADDRESSES adapter)
{
   while (adapter) {
      printf("\nAdapter name: %S\n", adapter->FriendlyName);

      PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress; //Linked list
      while (address) {
         printf("\t%s",
            address->Address.lpSockaddr->sa_family == AF_INET ?
            "IPv4" : "IPv6");

         char ap[100];

         getnameinfo(address->Address.lpSockaddr,
            address->Address.iSockaddrLength,
            ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
         printf("\t%s\n", ap);

         address = address->Next;
      }

      adapter = adapter->Next;
   }
}