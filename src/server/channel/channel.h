#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../../common.h"

#include <list>
#include <map>
#include <tr1/unordered_map>
#include <exception>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define UNORDERED_MAP std::tr1::unordered_map
typedef UNORDERED_MAP<std::string, Channel>

class ChannelException : public exception 
{
	public:
		ChannelException(const std::string &message, bool inclSysMsg = false) throw();
		~ChannelException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

class Channel
{
    public:
        Channel(uint32 owner, std::string name, std::string password = "", uint8 secure = 0, bool persistent = false);
        ~Channel();
        std::list<uint32> user_list;        
        std::string m_name;        

        void ResetTime() { gettimeofday(&m_createTime, NULL); }        
        uint32 GetTime(); // In millisecondi
    
        bool IsSecure() { return m_secure != 0; }
        uint32 GetOwner() { return m_owner; }

        bool Access(uint32 id, bool& get_admin, std::string password, uint8 secure) throw(ChannelException);
        bool Exit(uint32 id, uint32& new_owner);
    private:
        timeval m_createTime;
        std::string m_password;
        uint8 m_secure;
        uint32 m_owner;
        bool m_persistent;
}

class ChannelManagerException : public exception 
{
	public:
		ChannelManagerException(const std::string &message, bool inclSysMsg = false) throw();
		~ChannelManagerException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

        bool AccessChannel(uint32 id, std::string name, std::string password, uint8 secure)
        uint32 ExitChannel(uint32 id, std::string name);

	private:
        ChannelManager()
};

#endif