#ifndef TTSERVER_H
#define TTSERVER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <list>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class TTClient;

class TTAuthManager;
class TTHistoryManager;

static constexpr uint16_t clientBufferSize = 4096;

/**
 * @brief The TTServer class - the main class that stores all the server functionality
 */
class TTServer
{
public:
    enum class State: uint16_t {
        Init,
        Up,
        Down,
        ErrorInit,
        ErrorBind,
        ErrorListening,
        ErrorPoll,
        ErrorSetsoc
    };
    
    TTServer(uint16_t port = 8080);
    ~TTServer();
    void stop();
    
    State start();
    
    void handlingLoop();
    
    State currentState();

    void writeToGlobal(std::string data, std::string login = "Administrator");

    void joinLoop();

private:
    void globalMessageHandle(std::string message);
    std::string registration(std::string login, std::string password);

    TTClient* authSection(int clientSocket);

    void shutDownFinished();

    void systemMessage(std::string data, int clientSocket);

    std::string hostToString(uint32_t host, uint16_t port);

    bool workerLoop(int clientSocket, TTClient* client);

    std::atomic<State> m_globalState;
    
    int m_socketDescriptor = -1;
    uint16_t m_port = 8080;

    std::thread m_handlerThread;

    std::list<std::thread> m_clientThreads;
    std::list<std::thread::id> m_finishedClientThreadIds;
    std::list<TTClient*> m_clients;

    std::mutex m_clientsMutex;

    TTAuthManager *m_authManager = nullptr;
    TTHistoryManager *m_historyManager = nullptr;
};

#endif // TTSERVER_H
