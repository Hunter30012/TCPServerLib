#ifndef __TCPNEWCONNECTIONACCEPTOR_H__
#define __TCPNEWCONNECTIONACCEPTOR_H__

#include <pthread.h>

class TcpServerController;

/* New connection are accepted using 'accept()' sys call
*/
class TcpNewConnectionAcceptor
{
private:
    int accept_fd;
    pthread_t *accept_new_conn_thread;
public:
    TcpServerController *tcp_ctrlr;

    TcpNewConnectionAcceptor(TcpServerController *);
    ~TcpNewConnectionAcceptor();

    void StartTcpNewConnectionAcceptorThread();
    void StartTcpNewConnectionAcceptorThreadInternal();
    void StopTcpNewConnectionAcceptorThread();
    void Stop();
};



#endif