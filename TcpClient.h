#ifndef __TCP_CLIENT__
#define __TCP_CLIENT__

#include <stdint.h>

#define MAX_CLIENT_BUFFER_SIZE 1024

class TcpServerController;
class TcpMsgDemarcar;

class TcpClient {
    private:
    
    public:
        uint32_t ip_addr;
        uint16_t port_no;
        uint32_t server_ip_addr;
        uint16_t server_port_no;

        int comm_fd;
        TcpServerController *tcp_ctrlr;
        TcpMsgDemarcar *msgd;
        TcpClient(uint32_t ip_addr, uint16_t port_no);

        void Display();
};

#endif