#include "TcpNewConnectionAcceptor.h"
#include "TcpServerController.h"
#include "TcpClient.h"
#include <stdio.h>
#include <memory.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h> /* for IPPROTO_TCP */
#include "network_utils.h"
#include "TcpMsgFixedSizeDemarcar.h"


TcpNewConnectionAcceptor::TcpNewConnectionAcceptor(TcpServerController* tcp_ctrlr)
{
    /* create a socket fd */
    this->accept_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(this->accept_fd < 0) {
        printf("Error: Couldn't create Accept FD\n");
        exit(0);
    }
    this->accept_new_conn_thread = (pthread_t *) calloc(1, sizeof(pthread_t));
    this->tcp_ctrlr = tcp_ctrlr;
}

TcpNewConnectionAcceptor::~TcpNewConnectionAcceptor()
{

}

static void *tcp_listen_for_new_connection(void *arg) 
{
    TcpNewConnectionAcceptor *tcp_new_conn_acc = (TcpNewConnectionAcceptor*) arg;
    tcp_new_conn_acc->StartTcpNewConnectionAcceptorThreadInternal();

    return NULL;
} 

void TcpNewConnectionAcceptor::StartTcpNewConnectionAcceptorThread()
{
    if(pthread_create(this->accept_new_conn_thread, NULL, tcp_listen_for_new_connection, (void *)this)) {
        printf("%s Thread Creation failed", __FUNCTION__);
        exit(0);
    }
    printf("Service Started: TcpNewConnectionAcceptorThread\n");

}

void TcpNewConnectionAcceptor::StartTcpNewConnectionAcceptorThreadInternal()
{
    int opt = 1;

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->tcp_ctrlr->port_no);
    server_addr.sin_addr.s_addr = htonl(this->tcp_ctrlr->ip_addr);

    /* init socket with options */
    if(setsockopt(this->accept_fd, SOL_SOCKET, SO_REUSEADDR, (char* )&opt, sizeof(opt)) < 0) {
        printf("setsockopt Failed\n");
        exit(0);
    }
    if(setsockopt(this->accept_fd, SOL_SOCKET, SO_REUSEPORT, (char* )&opt, sizeof(opt)) < 0) {
        printf("setsockopt Failed\n");
        exit(0);
    }
    
    /* bind the socket now */
    if(bind(this->accept_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Error: Acceptor socket bind failed\n");
        exit(0);
    }

    if(listen(this->accept_fd, 5) < 0) {
        printf("listen failed\n");
        exit(0);
    }
    
    printf("Create socket for TcpNewConnectionAcceptor successfully\n");

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int comm_sock_fd;

    while (true)
    {
        comm_sock_fd = accept(this->accept_fd, (struct sockaddr*)&client_addr, &addr_len);

        if(comm_sock_fd < 0) {
            printf("Error in Accepting New Connections\n");
            continue;
        }

        TcpClient *tcp_client = new TcpClient(client_addr.sin_addr.s_addr, client_addr.sin_port);
        
        tcp_client->tcp_ctrlr = this->tcp_ctrlr;
        tcp_client->server_ip_addr = this->tcp_ctrlr->ip_addr;
        tcp_client->server_port_no = this->tcp_ctrlr->port_no;
        tcp_client->comm_fd = comm_sock_fd;

        /* Notify for Appln */
        if(this->tcp_ctrlr->client_connected) {
            this->tcp_ctrlr->client_connected(this->tcp_ctrlr, tcp_client);
        }

        /* hard code to test */
        tcp_client->msgd = new TcpMsgFixedSizeDemarcar(27);

        /* Tell the TCP Controller, to further process the Client */
        this->tcp_ctrlr->ProcessNewClient(tcp_client);

        printf("Connection Accepted from Client [%s, %d]\n", 
        network_convert_ip_n_to_p(htonl(client_addr.sin_addr.s_addr),0),
        htons(client_addr.sin_port)); 
    }
    
}

void TcpNewConnectionAcceptor::StopTcpNewConnectionAcceptorThread()
{
    if(!this->accept_new_conn_thread) return;
    pthread_cancel(*this->accept_new_conn_thread);
    /* wait until the thread is cancelled successfully */
    pthread_join(*this->accept_new_conn_thread, NULL);
    free(this->accept_new_conn_thread);
    this->accept_new_conn_thread = NULL;
}

void TcpNewConnectionAcceptor::Stop()
{
    /**
     * 1. Stop the CAS thread
     * 2. Release the resourse (accept_fd)
     * 3. delete instance CAS
    */
    this->StopTcpNewConnectionAcceptorThread();
    close(this->accept_fd);
    this->accept_fd = 0;
    delete this;
}
