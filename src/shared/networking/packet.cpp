#include "packet.h"
#include <openssl/rand.h>

int Packet::Encrypt(ByteBuffer par)
{
    ByteBuffer ciphertext;
    
    int ret=0;

    switch(mode)
    {
        case MODE_AES:
        {
            INFO("debug", "PACKET: encrypting AES packet\n");
            ret = AesEncrypt(par, (ByteBuffer)(*this), ciphertext);
        }
        break;
        case MODE_RSA:
        {
            INFO("debug", "PACKET: encrypting RSA packet\n");
            string pub = (const char*) par.contents();
            ret = RsaEncrypt(pub, (ByteBuffer)(*this), ciphertext);
        }
        break;
        case MODE_PLAIN:
            INFO("debug", "PACKET: can't encrypt. plain mode selected\n");
            return 0;
        break;
        default:
            INFO("debug", "PACKET: wrong mode selected\n");
        break;
    }
    
    ciphertext.hexlike();
    
    if (!ret)
    {
        INFO("debug", "PACKET: encrypted\n");
        m_encrypted = true;
        this->clear();
        if (ciphertext.size())
            this->append(ciphertext.contents(), ciphertext.size());
    }
    else
    {
        INFO("debug", "PACKET: encryption failed\n");
    }
    
    return ret;
}

int Packet::Decrypt(ByteBuffer par)
{
    ByteBuffer *ciphertext = (ByteBuffer*) this;
    ByteBuffer plaintext;
    int ret=0;

    switch(mode)
    {
        case MODE_AES:
        {
            INFO("debug", "PACKET: decrypting AES packet\n");
            ciphertext->hexlike();
            ret = AesDecrypt(par, *ciphertext, plaintext);
        }
        break;
        case MODE_RSA:
        {
            string pwd, priv;
            
            INFO("debug", "PACKET: decrypting RSA packet\n");
            ciphertext->hexlike();
            
            try
            {
                par >> priv;
                par >> pwd;
            }
            catch (...)
            {
                INFO("debug", "PACKET: reading private key with no password!\n");
                ret = RsaDecrypt(priv, NULL, (ByteBuffer)(*this), plaintext);
            }
            ret = RsaDecrypt(priv, pwd.c_str(), (ByteBuffer)(*this), plaintext);
        }
        break;
        case MODE_PLAIN:
            INFO("debug", "PACKET: can't decrypt. plain mode selected\n");
            return 0;
        break;
        default:
            INFO("debug", "PACKET: wrong mode selected\n");
        break;
    }
  
    if (!ret)
    {
        INFO("debug", "PACKET: decrypted\n");
        m_encrypted = false;
        this->clear();
        this->append(plaintext);
    }
    else
        INFO("debug", "PACKET: decryption failed\n");
     
    return ret;
}

void Packet::Incapsulate(Packet& pkt, uint32& u_id_send, bool check)
{
    *this << uint16(pkt.GetOpcode());
    *this << uint16(pkt.size());

    if (check)
    {
        *this << uint32(u_id_send);
        u_id_send++;
    }

    /*uint32 buf;
    RAND_pseudo_bytes((unsigned char*)(&buf), SALT_SIZE);
    *this << uint32(buf);*/

    if (pkt.size())
        this->append(pkt.contents(), pkt.size());

    INFO("debug","PACKET: packet incapsulated [header opcode %d, length %d]\n", pkt.GetOpcode(), pkt.size());
}

Packet* Packet::Decapsulate(uint32& u_id_receive, bool check)
{    
    if (empty())
        return NULL;

    uint16 opcode; 
    uint16 size;
    uint32 u_id_recv = 0;
    // uint32 rand;
    
    *this >> opcode;
    *this >> size;

    if (check)
        *this >> u_id_recv;

    // *this >> rand;

    INFO("debug","PACKET: packet decapsulated [header opcode %d, length %d]\n", opcode, size);

    Packet* new_pkt = new Packet(opcode, size);
    if (!new_pkt)
        return NULL;
    
    if (size)
    {
        new_pkt->append(contents()+rpos(), size);
        read_skip(size);
    }

    if (check)
    {
        if (u_id_recv < u_id_receive)
        {
            INFO("debug", "PACKET: Tentativo di Packet Replay, pacchetto ignorato\n");
            delete new_pkt;
            return NULL;     
        }
        else
        {
            u_id_receive = u_id_recv;
        }
    }

    return new_pkt;
}
