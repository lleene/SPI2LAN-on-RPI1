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

#define TEST_LENGTH (1024*1024)

using namespace std;

int main(int argc, char* argv[])
{

if (argc != 3)
{
   cout << "usage: cnt local_filename server_ip" << endl;
   return -1;
}


UDT::startup();
int64_t size, offset;
struct addrinfo hints, *peer;
string service("9000");
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_flags = AI_PASSIVE;
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;

UDTSOCKET fhandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

   if (0 != getaddrinfo(argv[2], service.c_str() , &hints, &peer))
   {
      cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
      return -1;
   }

// connect to the server, implict bind
   if (UDT::ERROR == UDT::connect(fhandle, peer->ai_addr, peer->ai_addrlen))
   {
      cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
      return -1;
   }
   freeaddrinfo(peer);

   char data[TEST_LENGTH];
   const char* pntr = data;
   size = (int64_t)sizeof(data);
   for(int i = 0; i<TEST_LENGTH; i++) data[i]=(char)i*i;

   if (UDT::ERROR == UDT::send(fhandle, (char*)&size, sizeof(int64_t), 0)){
      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;}

   for(int i = 0; i < TEST_LENGTH ; i++){
   offset=0;
   if (UDT::ERROR == UDT::send(fhandle, pntr, sizeof(data), size)){
      cout << "sendfile: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;}
   }


   UDT::close(fhandle);
   UDT::cleanup();

return 1;
}
