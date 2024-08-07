#include "TcpServerController.h"
#include "TcpClient.h"
#include "network_utils.h"
#include <arpa/inet.h>
#include "CommandParser/libcli.h"
#include "CommandParser/cmdtlv.h"
#include <list>

#define TCP_SERVER_CREATE 1
#define TCP_SERVER_START 2
#define TCP_SERVER_SHOW 3
#define TCP_SERVER_STOP_CONN_ACCEPT 4

std::list<TcpServerController *> tcp_server_list;
uint16_t default_port_no = 5000;
const char *default_ip_addr = "127.0.0.1";

static void print_client(const TcpClient *tcp_client)
{
    printf("[%s, %d]\n", network_convert_ip_n_to_p(htonl(tcp_client->ip_addr),0), htons(tcp_client->port_no));
}

static void print_server(const TcpServerController *tcp_server)
{
    printf("[%s, %d]\n", network_convert_ip_n_to_p(htonl(tcp_server->ip_addr),0), htons(tcp_server->port_no));
}


static void appln_client_connected(const TcpServerController *tcp_sever, const TcpClient *tcp_client) 
{
    printf("\nTcp Server\n");
    print_server(tcp_sever);
    printf("Appln: Client connected\n");
    print_client(tcp_client);
}

static void appln_client_disconnected(const TcpServerController *tcp_sever, const TcpClient *tcp_client)
{

}

static void appln_client_msg_recvd(const TcpServerController *tcp_sever, const TcpClient *tcp_client, unsigned char *msg, uint16_t msg_size)
{
    printf("%s() Bytes recvd: %d msg: %s\n", __FUNCTION__, msg_size, msg);
}

static TcpServerController* TcpServer_lookup(std::string tcp_server_name)
{
    TcpServerController *ctrlr;

    std::list<TcpServerController *>::iterator it;
    for(it = tcp_server_list.begin(); it != tcp_server_list.end(); it++) {
        ctrlr = *it;
        if(ctrlr->name == tcp_server_name) {
            return ctrlr;
        }
    }
    return NULL;
}

int config_tcp_server_handler(param_t *param, ser_buff_t *ser_buff, op_mode enable_or_disable)
{
    int cmd_code;
    const char *server_name = NULL;
    tlv_struct_t *tlv;
    TcpServerController *tcp_server = NULL;
    char *ip_addr = (char *) default_ip_addr;
    uint16_t port_no = default_port_no;

    cmd_code = EXTRACT_CMD_CODE(ser_buff);

    TLV_LOOP_BEGIN(ser_buff, tlv) {
        if(strncmp(tlv->leaf_id, "tcp-server-name", strlen("tcp-server-name")) == 0) {
            server_name = tlv->value;
        }
        else if(strncmp(tlv->leaf_id, "tcp-server-addr", strlen("tcp-server-addr")) == 0) {
            ip_addr = tlv->value;
        }
        else if(strncmp(tlv->leaf_id, "tcp-server-port", strlen("tcp-server-port")) == 0) {
            port_no = atoi(tlv->value);
        }
    } TLV_LOOP_END;

    switch (cmd_code)
    {
        case TCP_SERVER_CREATE:
            /* config tcp-server <name> */
            tcp_server = TcpServer_lookup(std::string(server_name));
            if(tcp_server) {
                printf("Error: Tcp Server Already Exist\n");
                return -1;
            }
            tcp_server = new TcpServerController(std::string(ip_addr), port_no, std::string(server_name));
            tcp_server_list.push_back(tcp_server);
            tcp_server->SetServerNotifCallbacks(appln_client_connected, appln_client_disconnected, appln_client_msg_recvd);

            break;
        case TCP_SERVER_START:
            /* config tcp-server <name> start */
            tcp_server = TcpServer_lookup(std::string(server_name));
            if(!tcp_server) {
                printf("Error: Tcp Server do not Exist\n");
                return -1;
            }
            tcp_server->Start();
            
            break;
        
        case TCP_SERVER_SHOW:
            tcp_server = TcpServer_lookup(std::string(server_name));
            if(!tcp_server) {
                printf("Error: Tcp Server do not Exist\n");
                return -1;
            }
            tcp_server->Display();
            break;
        
        case TCP_SERVER_STOP_CONN_ACCEPT:
            tcp_server = TcpServer_lookup(std::string(server_name));
            if(!tcp_server) {
                printf("Error: Tcp Server do not Exist\n");
                return -1;
            }
            switch (enable_or_disable)
            {
                case CONFIG_ENABLE:
                    tcp_server->StopConnectionAcceptorSvc();
                    break;
                
                case CONFIG_DISABLE:
                    tcp_server->StartConnectionAcceptorSvc();
                    break;
            }
            break;

        default:
            break;
    }
    return 0;
}



static void tcp_build_config_cli_tree()
{
    /**
     * Design same as a tree 
    */

    param_t *config_hook = libcli_get_config_hook();
    {
        /* config tcp-server... */
        static param_t tcp_server;
        init_param(&tcp_server, CMD, "tcp-server", NULL, NULL, INVALID, NULL, "config tcp-server");
        libcli_register_param(config_hook, &tcp_server);
        {
            /* config tcp-server <name> */
            static param_t tcp_server_name;
            init_param(&tcp_server_name, LEAF, NULL, config_tcp_server_handler, NULL, STRING, "tcp-server-name", "Tcp Server Name");
            libcli_register_param(&tcp_server, &tcp_server_name);
            set_param_cmd_code(&tcp_server_name, TCP_SERVER_CREATE);
            {
                /* config tcp-server <name> [no] disable-conn-accept */
                static param_t dis_conn_accept;
                init_param(&dis_conn_accept, CMD, "disable-conn-accept", config_tcp_server_handler, NULL, INVALID, NULL, "Connection Accept Settings");
                libcli_register_param(&tcp_server_name, &dis_conn_accept);
                set_param_cmd_code(&dis_conn_accept, TCP_SERVER_STOP_CONN_ACCEPT);
            }

            {
                /* config tcp-server <name> [<ip_addr>] ...*/
                static param_t tcp_server_addr;
                init_param(&tcp_server_addr, LEAF, 0, NULL, NULL, IPV4, "tcp-server-addr", "Tcp Server Address");
                libcli_register_param(&tcp_server_name, &tcp_server_addr);
                {
                    /* config tcp-server <name> [<ip_addr>] [<port_no>]*/
                    static param_t tcp_server_port;
                    init_param(&tcp_server_port, LEAF, 0, config_tcp_server_handler, 0, INT, "tcp-server-port", "Tcp Server Port");
                    libcli_register_param(&tcp_server_addr, &tcp_server_port);
                    set_param_cmd_code(&tcp_server_port, TCP_SERVER_CREATE);
                }
            }

            {
                /* config tcp-server <name> start */
                /* INVALID for CMD */
                static param_t start;
                init_param(&start, CMD, "start", config_tcp_server_handler, NULL, INVALID, NULL, "Start");
                libcli_register_param(&tcp_server_name, &start);
                set_param_cmd_code(&start, TCP_SERVER_START);
            }
            support_cmd_negation(&tcp_server_name);
            /* do not add */
        }
    }

    param_t *show_hook = libcli_get_show_hook();
    /* show tcp-server <server-name> */
    {
        static param_t tcp_server;
        init_param(&tcp_server, CMD, "tcp-server", NULL, NULL, INVALID, NULL, "show tcp-server");
        libcli_register_param(show_hook, &tcp_server);
        {
            /* show tcp-server <name> */
            static param_t tcp_server_name;
            init_param(&tcp_server_name, LEAF, NULL, config_tcp_server_handler, NULL, STRING, "tcp-server-name", "Tcp Server Name");
            libcli_register_param(&tcp_server, &tcp_server_name);
            set_param_cmd_code(&tcp_server_name, TCP_SERVER_SHOW);
        }
    }
}

static void tcp_build_show_cli_tree()
{
    param_t *show_hook = libcli_get_show_hook();
}

static void tcp_build_cli()
{
    tcp_build_config_cli_tree();
    tcp_build_show_cli_tree();

}

int main(int argc, char **argv)
{


#if 0 
    TcpServerController *server1 = new TcpServerController("127.0.0.1", 5000, "Default TCP Server");

    server1->SetServerNotifCallbacks(appln_client_connected, appln_client_disconnected, appln_client_msg_recvd);
    server1->Start();
    scanf("\n");

    server1->Display();
#endif

    init_libcli();

    tcp_build_cli();

    start_shell();
    /* dead code */
    return 0;
}