#ifndef _TCP_MSG_VARIABLE_SIZE_DEMARCAR_
#define _TCP_MSG_VARIABLE_SIZE_DEMARCAR_

#include "TcpMsgDemarcar.h"

#define VARIABLE_SIZE_MAX_BUFFER    256

class TcpClient;

class TcpMsgVariableSizeDemarcar: public TcpMsgDemarcar
{
private:

public:

    ~TcpMsgVariableSizeDemarcar();

    bool IsBufferReadyToFlush();
    void ProcessClientMsg(TcpClient *tcp_client);
    void Destroy();
};

#endif