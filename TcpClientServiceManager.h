#ifndef __TCPCLIENTSERVICEMANAGER_H__
#define __TCPCLIENTSERVICEMANAGER_H__

#include <list>
#include <sys/select.h>
#include <pthread.h>

class TcpClient;
class TcpServerController;

class TcpClientServiceManager
{
private:
    int max_fd;
    fd_set active_fd_set;
    fd_set backup_fd_set;
    pthread_t *client_svc_mgr_thread;
    std::list<TcpClient*> tcp_client_db;

    int GetMaxFd();
    void CopyClientFDtoFDSet(fd_set *fdset);
    
public:
    TcpServerController *tcp_ctrlr;

    TcpClientServiceManager(TcpServerController* );
    ~TcpClientServiceManager();

    void StartTcpClientServiceManagerThread();
    void StartTcpClientServiceManagerThreadInternal();
    void ClientFDStartListen(TcpClient *);
    
    TcpClient* LookUpClientDB(uint32_t ip_addr, uint16_t port_no);
    void AddClientToDB(TcpClient *tcp_client);
    void StopTcpClientServiceManagerThread();

    void Stop();
};


#endif