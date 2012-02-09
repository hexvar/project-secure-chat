#include "channel-manager.h"

// ChannelException Code
ChannelException::ChannelException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) 
{
	if (inclSysMsg) 
	{
		userMessage.append(": ");
		userMessage.append(strerror(errno));
	}
}

ChannelException::~ChannelException() throw() 
{
}

const char *ChannelException::what() const throw() 
{
	return userMessage.c_str();
}

// ChannelManagerException Code
ChannelManagerException::ChannelManagerException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) 
{
	if (inclSysMsg) 
	{
		userMessage.append(": ");
		userMessage.append(strerror(errno));
	}
}

ChannelManagerException::~ChannelManagerException() throw() 
{
}

const char *ChannelManagerException::what() const throw() 
{
	return userMessage.c_str();
}

Channel::Channel(uint32 owner, std::string name, std::string password, uint8 secure, bool persistent)
{    
    m_name = name;
    m_owner = owner;
    m_password = password;
    m_secure = secure;
    m_persistent = persistent;

    if (owner)
        user_list.push_back(owner);

    gettimeofday(&m_createTime, NULL);
    pthread_mutex_init(&mutex_Channel, NULL);
}

Channel::~Channel()
{
    pthread_mutex_destroy(&mutex_Channel);
}

bool Channel::Access(uint32 id, std::string password, uint8 secure) throw(ChannelException)
{
    getlock_Channel();
    if (secure != m_secure)
    {
        releaselock_Channel();
        throw ChannelException("Try access a Secure Channel without be logged");
    }
    if (password != m_password);
    {
        releaselock_Channel();
        throw ChannelException("Password is wrong");
    }
    for (std::list<uint32>::iterator itr = user_list.begin(); itr!= user_list.end(); itr++)
        if (*itr == id)
        {
            releaselock_Channel();
            throw ChannelManagerException("Multiple some id Access to some Channel");
        }

    if (user_list.empty())
    {
        m_owner = id;
        releaselock_Channel();
        return true;
    }

    user_list.push_back(id);
    releaselock_Channel();
    return false;        
}

bool Channel::Exit(uint32 id, uint32& new_owner)
{
    getlock_Channel();
    for (std::list<uint32>::iterator itr = user_list.begin(); itr!= user_list.end(); itr++)
        if (*itr == id)
            user_list.erase(itr);
    
    if (user_list.empty() && !m_persistent)
    {
        releaselock_Channel();
        return false;  // delete channel
    }

    if (m_owner == id && !user_list.empty())
    {
        m_owner = user_list.front();
        new_owner = m_owner;
    }
    releaselock_Channel();
    return true;   
}

bool Channel::SendPacketToAll(Packet* new_packet, uint32 exclude_id)
{
    getlock_Channel();
    for (std::list<uint32>::iterator itr = user_list.begin(); itr!= user_list.end(); itr++)
        if (*itr != exclude_id)
            s_manager->SendPacketTo(*itr, new_packet);

    releaselock_Channel();
}

uint32 Channel::GetTime() // In millisecondi
{
    uint32 seconds  = m_createTime.tv_sec;  // - start.tv_sec;  TODO Quando è stato avviato il programma
    uint32 useconds = m_createTime.tv_usec; // - start.tv_usec; TODO Quando è stato avviato il programma
    return ((seconds) * 1000 + useconds/1000.0) + 0.5;
}

ChannelManager::ChannelManager()
{    
    pthread_mutex_init(&mutex_mapChannel, NULL);

    m_mapChannel.insert (std::make_pair("default", new Channel(0, "default", "", 0, true)));
}

ChannelManager::~ChannelManager()
{
    pthread_mutex_destroy(&mutex_mapChannel);
}

bool ChannelManager::CreateChannel(uint32 id, std::string name, std::string password, uint8 secure, bool persistent)
{
    getlock_mapChannel();
    if (m_mapChannel.find(name) != m_mapChannel.end())
    {
        releaselock_mapChannel();
        throw ChannelException("There is a Channel with some Name");
    }

    m_mapChannel.insert (std::make_pair(name, new Channel(id, name, password, secure, persistent)));
    releaselock_mapChannel();
}

bool ChannelManager::AccessChannel(uint32 id, std::string name, std::string password, uint8 secure)
{
    getlock_mapChannel();
    mapChannel::iterator itr = m_mapChannel.find(name);
    if (itr == m_mapChannel.end())
    {
        releaselock_mapChannel();
        throw ChannelException("Channel Name not Found");
    }
    try
    {
        (*itr).second->Access(id, password, secure);
        releaselock_mapChannel();
        return true;        
    }
    catch (...)
    {
        releaselock_mapChannel();
            throw;
    }
    return false;
}

uint32 ChannelManager::ExitChannel(uint32 id, std::string name)
{
    getlock_mapChannel();
    mapChannel::iterator itr = m_mapChannel.find(name);
    if (itr == m_mapChannel.end())
    {
        releaselock_mapChannel();
        throw ChannelManagerException("Channel Name not Found");
    }

    uint32 new_owner = 0;
    try
    {
        if (!(*itr).second->Exit(id, new_owner))
        {
            delete (*itr).second;
            m_mapChannel.erase(itr);            
        }
        releaselock_mapChannel();
        return new_owner;        
    }
    catch (...)
    {
      releaselock_mapChannel();
      throw;
    }
}

void ChannelManager::SendPacketToChannel(std::string name, Packet* new_packet, uint32 exclude_id)
{
    getlock_mapChannel();
    mapChannel::iterator itr = m_mapChannel.find(name);
    if (itr == m_mapChannel.end())
    {
        releaselock_mapChannel();
        throw ChannelManagerException("Channel Name not Found");
    }

    
    try
    {
        (*itr).second->SendPacketToAll(new_packet, exclude_id);
        releaselock_mapChannel();     
    }
    catch (...)
    {
      releaselock_mapChannel();
      throw;
    }
}

void ChannelManager::ChannelList(std::vector<std::string>& v_name)
{
    getlock_mapChannel();
    for (mapChannel::iterator itr = m_mapChannel.begin(); itr != m_mapChannel.end(); itr++)        
        v_name.push_back((*itr).first);
    releaselock_mapChannel();
}
