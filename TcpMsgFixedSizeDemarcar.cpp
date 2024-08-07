#include <assert.h>
#include "TcpMsgDemarcar.h"
#include "TcpMsgFixedSizeDemarcar.h"
#include "TcpClient.h"
#include "TcpServerController.h"
#include "ByteCircularBuffer.h"

bool TcpMsgFixedSizeDemarcar::IsBufferReadyToFlush()
{
    if((this->TcpMsgDemarcar::bcb->current_size / this->msg_fixed_size) > 0) {
        return true; 
    } 
    return false;
}

void TcpMsgFixedSizeDemarcar::ProcessClientMsg(TcpClient *tcp_client)
{
    uint16_t bytes_read;
    if(!this->IsBufferReadyToFlush()) return;

    while (bytes_read = BCBRead(this->TcpMsgDemarcar::bcb, 
                                this->TcpMsgDemarcar::buffer, 
                                this->msg_fixed_size,
                                true))
    {
        tcp_client->tcp_ctrlr->client_msg_recvd(tcp_client->tcp_ctrlr, 
                                                tcp_client, 
                                                this->TcpMsgDemarcar::buffer,
                                                bytes_read );
    }

}

TcpMsgFixedSizeDemarcar::TcpMsgFixedSizeDemarcar(uint16_t fixed_size)
    : TcpMsgDemarcar(DEFAULT_CBC_SIZE)
{
    this->msg_fixed_size = fixed_size;
}

TcpMsgFixedSizeDemarcar::~TcpMsgFixedSizeDemarcar()
{
}

void TcpMsgFixedSizeDemarcar::Destroy()
{
    this->TcpMsgDemarcar::Destroy();
}
