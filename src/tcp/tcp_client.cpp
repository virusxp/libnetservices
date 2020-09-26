#include "tcp/tcp_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

TCPConnection::TCPConnection(std::string address, uint16_t port) : address(address),port(port)
{
    if(setInternals() != 0)
    {
        // TODO: throw exception
    }
}

TCPConnection::TCPConnection(const char* address, uint16_t port) : address(std::string(address)),port(port)
{
    if(setInternals() != 0)
    {
        // TODO: throw exception
    }
}

TCPConnection::~TCPConnection()
{
    freeaddrinfo(aInfo);
}

int TCPConnection::setInternals()
{
    int err = 0;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    err = getaddrinfo(address.c_str(),NULL,&hints,&aInfo);
    
    return err;
}

std::string TCPConnection::getAddress()
{
    return this->address;
}

void TCPConnection::setAddress(const char* address)
{
    this->address = std::string(address);
    if(setInternals() != 0)
    {
        // TODO: throw exception
    }
}

void TCPConnection::setAddress(std::string address)
{
    this->address = address;
    if(setInternals() != 0)
    {
        // TODO: throw exception
    }
}

uint16_t TCPConnection::getPort()
{
    return this->port;
}

void TCPConnection::setPort(uint16_t port)
{
    this->port = port;
}

struct addrinfo* TCPConnection::getAddrInfo()
{
    return this->aInfo;
}



void TCPAsyncClient_RcvThread(std::atomic_bool* killer, int* sock, SimpleConcurrentQueue<char>* readQ)
{
    char data[RECEIVE_AT_ONCE];
    fd_set sockSet;
    FD_ZERO(&sockSet);
    FD_SET(*sock,&sockSet);

    int err = 0;
    struct timeval timeout;
    while(!*killer)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        err = select(1,&sockSet,NULL,NULL,&timeout);
        if(err > 0)
        {
            // data available
            ssize_t dataRcvd;
            
            while((dataRcvd = recv(*sock,data,RECEIVE_AT_ONCE,MSG_DONTWAIT)) > 0)
            {
                readQ->pushn(data,dataRcvd);
            }
        }
        else if(err < 0)
        {
            // error
            break;
        }
        // else repeat loop
    }
}

void TCPAsyncClient_SndThread(std::atomic_bool* killer, int* sock, SimpleConcurrentQueue<char>* writeQ)
{
    char data[SEND_AT_ONCE];
    fd_set sockSet;
    FD_ZERO(&sockSet);
    FD_SET(*sock,&sockSet);

    while(!*killer)
    {
        //TODO: place condition variable here

        size_t toSnd = writeQ->size();
        if(toSnd == 0 || *killer)
        {
            continue;
        }

        // check how much data we can send at once
        toSnd = (toSnd > SEND_AT_ONCE) ? SEND_AT_ONCE : toSnd;
        writeQ->popn(data,toSnd);
        send(*sock,data,toSnd,0);
    }
}

TCPAsyncClient::TCPAsyncClient(std::string address, uint16_t port)
{
    this->conn = new TCPConnection(address,port);
    this->killThread = false;
}

TCPAsyncClient::TCPAsyncClient(const char* address, uint16_t port)
{
    this->conn = new TCPConnection(address,port);
    this->killThread = false;
}

TCPAsyncClient::TCPAsyncClient(TCPConnection* connection)
{
    this->conn = new TCPConnection(connection->address,connection->port);
    this->killThread = false;
}

TCPAsyncClient::~TCPAsyncClient()
{
    delete this->conn;
}

bool TCPAsyncClient::openConnection()
{
    // if socket is opened already, then close it first
    if(this->sock != 0)
    {
        close(this->sock);
    }

    struct addrinfo* p;
    for(p = this->conn->aInfo; p != NULL; p = p->ai_next)
    {
        if((this->sock = socket(p->ai_family,p->ai_socktype,p->ai_protocol) < 0))
        {
            continue;
        }

        if(connect(this->sock,p->ai_addr,p->ai_addrlen) < 0)
        {
            close(this->sock);
            continue;
        }

        break;
    }

    if(p == NULL)
    {
        return false;
    }

    this->killThread = false;
    this->rcvThread = std::thread(TCPAsyncClient_RcvThread,&this->killThread,&this->sock,&this->qr);
    this->rcvThread = std::thread(TCPAsyncClient_RcvThread,&this->killThread,&this->sock,&this->qw);

    return true;
}

void TCPAsyncClient::closeConnection()
{
    this->killThread = true;
    // join threads
}