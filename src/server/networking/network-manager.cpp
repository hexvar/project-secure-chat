#include "network-manager.h"

class SocketThread: public MethodRequest
{
    private:

        NetworkManager& m_netmanager;
        uint32 m_diff;

    public:

        SocketThread(NetworkManager& netmanager, uint32 d) : 
        MethodRequest(), m_netmanager(netmanager), m_diff(d)
        {
            
        }

        ~SocketThread()
        {

        }

        int Call()
        {
            //inizializzazione callback
            while (1)
            {
                // Cose buffe epoll
            }
            return 0;
        }
};

class NetworkThread: public MethodRequest
{
    private:

        NetworkManager& m_netmanager;
        uint32 m_diff;

    public:

        NetworkThread(NetworkManager& netmanager, uint32 d) : 
        MethodRequest(), m_netmanager(netmanager), m_diff(d)
        {
            
        }

        ~NetworkThread()
        {

        }

        int Call()
        {
            netsession_pair net_ses;
            while (1)
            {                
                net_ses = m_netmanager.GetNextSession();
                // prendi pacchetto dal socket
                // elabora pacchetto 
                // Inserisci in coda pacchetto nella session                
            }
            return 0;
        }
};

NetworkManager::NetworkManager(uint32 n_thread) : sem(m_mutex)
{
    m_thread = n_thread;
    // +1 per l'epoll thread
    net_engine.Initialize(m_thread + 1);
}

int NetworkManager::ActivateEpoll()
{
    if (s_sched_engine->Execute(new SocketThread(*this, 0)) != 0)
    {
        // TODO Log Errore
        return -1;
    }

    return 0;
}

int NetworkManager::ActivateThreadsNetwork()
{
    for (uint8 i = 0; i < m_thread; i++)
    {
        if (s_sched_engine->Execute(new NetworkThread(*this, 0)) != 0)
        {
            // TODO Log Errore
            return -1;
        }
    }

    return 0;
}

int NetworkManager::QueueSend(Session_smart m_ses)
{   
    if (!m_ses.get())
        return -1;

    q_request.add(netsession_pair(m_ses, SEND));
    sem.Signal();
    return 0;
}

int NetworkManager::QueueRecive(Session_smart m_ses)
{   
    if (!m_ses.get())
        return -1;

    q_request.add(netsession_pair(m_ses, RECIVE));
    sem.Signal();
    return 0;
}

netsession_pair NetworkManager::GetNextSession()
{
    sem.Wait();
    netsession_pair ret;
    q_request.next(ret);
    return ret;
}
