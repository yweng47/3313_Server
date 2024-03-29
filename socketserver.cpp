#include "socketserver.h"
#include <strings.h>
#include <iostream>
#include <errno.h>
#include <algorithm>

namespace Communication{
SocketServer::SocketServer(int port)
{
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
        throw std::string("Unable to open the socket server");
    bzero((char*)&socketDescriptor,sizeof(sockaddr_in));
    socketDescriptor.sin_family = AF_INET;
    socketDescriptor.sin_port = htons(port);
    socketDescriptor.sin_addr.s_addr = INADDR_ANY;
    if (bind(socketFD,(sockaddr*)&socketDescriptor,sizeof(socketDescriptor)) < 0)
        throw std::string("Unable to bind socket to requested port");

    // Set up a maximum number of pending connections to accept
    listen(socketFD,5);
    SetFD(socketFD);
    // At this point, the object is initialized.  So return.
}
SocketServer::~SocketServer(void)
{
    close(GetFD());
	//shutdown(GetFD(),SHUT_RDWR);
}

Socket SocketServer::Accept(void)
{
    FlexWait waiter(2,this,&terminator);
    Blockable * result = waiter.Wait();

    if (result == &terminator)
    {
        terminator.Reset();
        throw TerminationException(2);
    }

    if (result == this)
    {
        int connectionFD = accept(GetFD(),NULL,0);
        if (connectionFD < 0)
        {
            throw std::string("Unexpected error in the server");
        }
        return Socket(connectionFD);
    }
    else
        throw std::string("Unexpected error in the server");
}

void SocketServer::Shutdown(void)
{
    terminator.Trigger();
}
};
