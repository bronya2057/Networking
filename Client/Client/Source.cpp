/*win_list.c*/

#include "WinsockHelper.hpp"

int main() {

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

   CLEAR_WINSOCK()
}