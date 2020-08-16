#ifndef TTCLIENT_H
#define TTCLIENT_H

#include <string>
#include <thread>

/**
 * @brief The TTClient class stores basic information about client
 */
class TTClient
{
public:
    TTClient(std::thread::id id, int clientSocket, std::string login);
    ~TTClient();

    std::thread::id id() { return m_id; };
    std::string login() {return m_login; };
    int sendToClient(std::string data);

private:
    std::thread::id m_id; // Thread id
    std::string m_login; // Only for IM
    int m_clientSocket = -1;
};

#endif // TTCLIENT_H
