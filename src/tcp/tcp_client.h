#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <string>
#include <cstdint>
#include <netdb.h>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>

#include "utils/container.h"

#define RECEIVE_AT_ONCE 64
#define SEND_AT_ONCE    64

class TCPConnection
{
    friend class TCPAsyncClient;

    private:
        std::string address;
        uint16_t port;

        struct addrinfo* aInfo; 

    private:
        TCPConnection() = default;
        int setInternals();

    public:
        TCPConnection(std::string address, uint16_t port);
        TCPConnection(const char* address, uint16_t port);
        ~TCPConnection();

        std::string getAddress();
        void setAddress(const char* address);
        void setAddress(std::string address);

        uint16_t getPort();
        void setPort(uint16_t port);

        struct addrinfo* getAddrInfo();
};

class TCPAsyncClient
{
    private:
        TCPConnection* conn;
        int sock = 0;

        std::atomic_bool killThread;
        std::thread rcvThread;
        std::thread sendThread;
        SimpleConcurrentQueue<char> qw;
        SimpleConcurrentQueue<char> qr;

    private:
        TCPAsyncClient() = default;

    public:
        TCPAsyncClient(std::string address, uint16_t port);
        TCPAsyncClient(const char* address, uint16_t port);
        TCPAsyncClient(TCPConnection* connection);
        ~TCPAsyncClient();

        bool openConnection();
        void closeConnection();

    
};

#endif //TCP_CLIENT_H_