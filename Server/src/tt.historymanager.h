#ifndef TTHISTORYMANAGER_H
#define TTHISTORYMANAGER_H

#include <list>
#include <mutex>
#include <string>

class TTClient;

/**
 * @brief The TTHistoryManager class for work with chat history
 */
class TTHistoryManager
{
public:
    TTHistoryManager();
    void addMessageToHistory(std::string message);
    void sendHistory(TTClient *client);

private:
    std::list<std::string> m_history;
    std::mutex m_historyMutex;
};

#endif // TTHISTORYMANAGER_H
