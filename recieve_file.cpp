#ifndef WIN32
   #include <arpa/inet.h>
   #include <netdb.h>
#else
   #include <winsock2.h>
   #include <ws2tcpip.h>
#endif
#include <iostream>
#include <udt.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

int main(int argc, char* argv[])
{
   int i = 1;
   const char* fname;
   stringstream ss;

   // Check for usage
   if (argc != 2)
   {
      cout << "usage: serv local_filename" << endl;
      return -1;
   }

   // Initilize UDT process/struc.
   UDT::startup();

   addrinfo hints;
   addrinfo* res;

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;

   // Check if port is busy
   string service("9000");
   if (0 != getaddrinfo(NULL, service.c_str(), &hints, &res))
   {
      cout << "port is busy.\n" << endl;
      return 0;
   }

   // start service & bind process control
   UDTSOCKET serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
   if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
   {
      cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   // Notify setup is complete
   freeaddrinfo(res);
   cout << "server is ready at port: " << service << endl;
   UDT::listen(serv, 10);


   while (true)
   {

   sockaddr_storage clientaddr;
   int addrlen = sizeof(clientaddr);

   UDTSOCKET fhandle;

   if (UDT::INVALID_SOCK == (fhandle = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
   {
      cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   char clienthost[NI_MAXHOST];
   char clientservice[NI_MAXSERV];
   getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
   cout << "new connection: " << clienthost << ":" << clientservice << endl;

   // get size information
  int64_t size;
   if (UDT::ERROR == UDT::recv(fhandle, (char*)&size, sizeof(int64_t), 0))
   {
      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
      return -1;
   }

   ss.str("");
   ss << argv[1] << i ;
   fname = ss.str().c_str();
   //fname = "catch";
   fstream ofs( fname, ios::out | ios::binary | ios::trunc);
   int64_t recvsize;
   int64_t offset = 0;

   cout << fname << endl;

   if (UDT::ERROR == (recvsize = UDT::recvfile(fhandle, ofs, offset, size)))
   {
      cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
      return -1;
   }

   // Close File Handle & Communication port
   UDT::close(fhandle);
   ofs.close();
   i++;
   }

   // Stop listening & Clean up UDT
   UDT::close(serv);
   UDT::cleanup();

   return 0;
}
