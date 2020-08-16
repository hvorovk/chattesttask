#include "tt.historymanager.h"

#include "tt.client.h"

const int k_maxHistoryMessages = 20;

TTHistoryManager::TTHistoryManager()
{

}

void TTHistoryManager::addMessageToHistory(std::string message)
{
    std::lock_guard<std::mutex> lock(m_historyMutex);
    m_history.push_back(message);

    while (m_history.size() > k_maxHistoryMessages) {
        m_history.pop_front();
    }
}

void TTHistoryManager::sendHistory(TTClient *client)
{
    std::lock_guard<std::mutex> lock(m_historyMutex);
    for (std::string message : m_history) {
        client->sendToClient(message);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
