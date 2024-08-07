#ifndef _TCP_DEMARCAR_FIXED_SIZE_
#define _TCP_DEMARCAR_FIXED_SIZE_

#include <stdint.h>
#include "TcpMsgDemarcar.h"

class TcpClient;

class TcpMsgFixedSizeDemarcar: public TcpMsgDemarcar
{
private:
    uint16_t msg_fixed_size;
    
public:
    bool IsBufferReadyToFlush();
    void ProcessClientMsg(TcpClient *tcp_client);

    TcpMsgFixedSizeDemarcar(uint16_t fixed_size);
    ~TcpMsgFixedSizeDemarcar();
    void Destroy(); 
};



#endif