#include <stdio.h>
#include <assert.h>

#include "TcpServerController.h"

#include "network_utils.h"
#include "TcpClientServiceManager.h"
#include "TcpClientDBManager.h"
#include "TcpNewConnectionAcceptor.h"
#include <arpa/inet.h>



TcpServerController::TcpServerController(std::string ip_addr, uint16_t port_no, std::string name)
{
    this->ip_addr = network_covert_ip_p_to_n(ip_addr.c_str());
    this->port_no = port_no;
    this->name = name;
    this->state_flags = 0;

    this->tcp_client_db_mgr = new TcpClientDbManager(this);
    this->tcp_client_svc_mgr = new TcpClientServiceManager(this);
    this->tcp_new_conn_acc = new TcpNewConnectionAcceptor(this);
    this->SetBit(TCP_SERVER_INITIALIZED);
}

TcpServerController::~TcpServerController()
{
    
}

void TcpServerController::Start()
{
    /* start Connection accept service thread (CAS)
             Data request service thread (DRS)
        initialize the DBMS
    */

    this->tcp_new_conn_acc->StartTcpNewConnectionAcceptorThread();
    this->tcp_client_svc_mgr->StartTcpClientServiceManagerThread();
    this->tcp_client_db_mgr->StartTcpClientDbInit();

    printf("Tcp Server is Up and Running [%s, %d]\nOk\n",
            network_convert_ip_n_to_p(this->ip_addr, NULL), this->port_no);
    this->SetBit(TCP_SERVER_RUNNING);
}

void TcpServerController::Stop()
{
}

void TcpServerController::ProcessNewClient(TcpClient *tcp_client)
{
    this->tcp_client_db_mgr->AddClientToDB(tcp_client);
    this->tcp_client_svc_mgr->ClientFDStartListen(tcp_client); 
}

void TcpServerController::Display()
{
    printf("Server Name: %s\n", this->name.c_str());
    if(!this->IsSetBit(TCP_SERVER_RUNNING)) {
        printf("Tcp Server Not Running\n");
        return;
    }
    printf("Listening on: [%s, %d]\n", network_convert_ip_n_to_p(this->ip_addr, 0), this->port_no);
    this->tcp_client_db_mgr->DisplayClientDb();
}

void TcpServerController::SetBit(uint32_t bit_value)
{
    state_flags |= bit_value;
}

void TcpServerController::UnSetBit(uint32_t bit_value)
{
    state_flags &= ~bit_value;
}

bool TcpServerController::IsSetBit(uint32_t bit_value)
{
    return (state_flags & bit_value);
}



void TcpServerController::SetServerNotifCallbacks(void (*client_connected)(const TcpServerController *, const TcpClient *), 
                                                  void (*client_disconnected)(const TcpServerController *, const TcpClient *), 
                                                  void (*client_msg_recvd)(const TcpServerController *, const TcpClient *, unsigned char *, uint16_t))
{
    this->client_connected = client_connected;
    this->client_disconnected = client_disconnected;
    this->client_msg_recvd = client_msg_recvd;
}

void TcpServerController::StopConnectionAcceptorSvc()
{
    if(this->IsSetBit(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS)) return;
    this->SetBit(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS);
    this->tcp_new_conn_acc->Stop();
    this->tcp_new_conn_acc = NULL;
}

void TcpServerController::StartConnectionAcceptorSvc()
{
    if(!this->IsSetBit(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS)) return;
    this->UnSetBit(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS);
    this->tcp_new_conn_acc = new TcpNewConnectionAcceptor(this);
    this->tcp_new_conn_acc->StartTcpNewConnectionAcceptorThread(); 
}

void TcpServerController::StopClientSvcMgr()
{
    if(this->IsSetBit(TCP_SERVER_NOT_LISTENING_CLIENTS)) return;
    this->tcp_client_svc_mgr->Stop();
    this->SetBit(TCP_SERVER_NOT_LISTENING_CLIENTS);
    this->tcp_client_svc_mgr = NULL;
}

void TcpServerController::StartClientSvcMgr()
{
}
