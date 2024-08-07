#ifndef __TCPCLIENTDBMANAGER_H__
#define __TCPCLIENTDBMANAGER_H__

#include <list>

class TcpClient;
class TcpServerController;

class TcpClientDbManager
{
private:
    std::list<TcpClient*> tcp_client_db;
public:
    TcpServerController *tcp_ctrlr;

    TcpClientDbManager(TcpServerController *);
    ~TcpClientDbManager();

    void StartTcpClientDbInit();
    void AddClientToDB(TcpClient *tcp_client);
    void DisplayClientDb();
};




#endif