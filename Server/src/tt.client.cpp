#include "tt.client.h"

#include <sys/socket.h>
#include <sys/types.h>

TTClient::TTClient(std::thread::id id, int clientSocket, std::string login)
    : m_id(id),
      m_login(login),
      m_clientSocket(clientSocket)
{

}

TTClient::~TTClient()
{

}

int TTClient::sendToClient(std::string data)
{
    return send(m_clientSocket, data.c_str(), data.length(), 0);
}
