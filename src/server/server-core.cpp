#include "server-core.h"

void init_commands() // va in un file commands-server.cpp/h che include functions
{
   //qui da aggiungere tutti i comandi con handler ecc
}

void start_server_core()
{
    TCPServerSocket server(CFG_GET_INT("server_port"), 128);
    TCPSocket *temp_sock = NULL;
    UserSession *temp_session = NULL;
    INFO("debug", "* listening on port: %d\n", CFG_GET_INT("server_port"));

    init_commands();

    db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
    db_manager->init_db();

    network_threads net;
    net.start_net_threads(4);

    execution_threads exec;
    exec.start_exec_threads(4);

    while(1)
    {
        temp_sock = server.accept();
        s_manager->createSession(temp_sock);
        INFO("debug", "* new client session created!\n");
    }
}