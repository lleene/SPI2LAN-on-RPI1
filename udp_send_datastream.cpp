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
#include <stdlib.h>

#include <string.h> 
#include <stdint.h>

#define TEST_SIZE (0x3FF)
#define T_REPEAT 1024

static uint32_t TEST_ARRAY[TEST_SIZE];

using namespace std;

int main(int argc, char* argv[])
{

    if (argc != 2)
    {
       cout << "usage: udp_send_data_stream server_ip" << endl;
       return -1;
    }
    // Init Test Block
    for(int i=0; i<TEST_SIZE;i++)
    {
        TEST_ARRAY[i]=(uint32_t)i;
    }
    
    // Init Network connection
    UDT::startup();
    int64_t size;
    struct addrinfo hints, *peer;
    string service("9000");
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    UDTSOCKET fhandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    if (0 != getaddrinfo(argv[1], service.c_str() , &hints, &peer))
    {
       cout << "incorrect server/peer address. " << argv[1] << endl;
       return -1;
    }
    // connect to the server, implict bind
    if (UDT::ERROR == UDT::connect(fhandle, peer->ai_addr, peer->ai_addrlen))
    {
        cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
        return -1;
    }
    freeaddrinfo(peer);

    // send data block size    
    size = (int64_t)(TEST_SIZE*sizeof(uint32_t));
    if (UDT::ERROR == UDT::send(fhandle, (char*)&size, sizeof(int64_t), 0)){
        cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
        return 0;}
    // send data T_REPEAT times
    for(int j = 0; j < T_REPEAT ; j++){
	if (UDT::ERROR == UDT::send(fhandle, (char *)&Test_ARRAY, TEST_SIZE*sizeof(uint32_t), size)){
	    cout << "sendfile: " << UDT::getlasterror().getErrorMessage() << endl;
	    return 0;}
    }

    printf("Clean Exit \n");
    UDT::close(fhandle);
    UDT::cleanup();
    return 1;
}
