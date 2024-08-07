#include <assert.h>
#include "TcpMsgVariableSizeDemarcar.h"
#include "TcpClient.h"
#include "ByteCircularBuffer.h"
#include "TcpServerController.h"


#define HDR_MSG_SIZE 2

TcpMsgVariableSizeDemarcar::TcpMsgVariableSizeDemarcar()
    :TcpMsgDemarcar(DEFAULT_CBC_SIZE)
{

}

TcpMsgVariableSizeDemarcar::~TcpMsgVariableSizeDemarcar()
{
}


bool TcpMsgVariableSizeDemarcar::IsBufferReadyToFlush()
{
    uint16_t msg_size;
    ByteCircularBuffer_t *bcb = this->TcpMsgDemarcar::bcb;
    if(bcb->current_size < HDR_MSG_SIZE) return false;

    BCBRead(bcb, (unsigned char*)&msg_size, HDR_MSG_SIZE, false);
    /* message is full with number in hdr msg */
    if(msg_size <= bcb->current_size) return true;

    return false;
}

void TcpMsgVariableSizeDemarcar::ProcessClientMsg(TcpClient *tcp_client)
{
    uint16_t msg_size;
    ByteCircularBuffer_t *bcb = this->TcpMsgDemarcar::bcb;

    while (this->IsBufferReadyToFlush()) {

        BCBRead(bcb, (unsigned char *)&msg_size, HDR_MSG_SIZE, false);
        assert(msg_size == BCBRead(bcb, this->TcpMsgDemarcar::buffer, msg_size, true));

        tcp_client->tcp_ctrlr->client_msg_recvd(tcp_client->tcp_ctrlr,
                                                tcp_client,  
                                                this->TcpMsgDemarcar::buffer, 
                                                msg_size);
    }

}

void TcpMsgVariableSizeDemarcar::Destroy()
{
    this->TcpMsgDemarcar::Destroy();
}
