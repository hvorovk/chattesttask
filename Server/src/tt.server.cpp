#include "tt.server.h"
#include <iostream>
#include <regex>
#include <sys/poll.h>
#include "src/tt.client.h"
#include "src/tt.authmanager.h"
#include "src/tt.historymanager.h"
#include "src/tt.helpers.h"

TTServer::~TTServer()
{
    if (m_globalState == State::Up) {
        stop();
    }
    delete m_authManager;
    delete m_historyManager;
}

void TTServer::stop()
{
    std::cout << "Server start to shutdown..." << std::endl;
    m_globalState = State::Down;
    close(m_socketDescriptor);
    m_handlerThread.join();

    for(std::thread& thread : m_clientThreads) {
        thread.join();
    }

    m_finishedClientThreadIds.clear();
    m_clientThreads.clear();
    m_clients.clear();
    std::cout << "Server finished shutdown" << std::endl;
}

TTServer::State TTServer::start()
{
    m_authManager->init();

    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(m_port);
    server.sin_family = AF_INET;
    m_socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(m_socketDescriptor == -1)
        return m_globalState = State::ErrorInit;

    int yes = 1;

    if (setsockopt(m_socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        return m_globalState = State::ErrorSetsoc;

    if(bind(m_socketDescriptor, (struct sockaddr *) &server, sizeof(server)) < 0)
        return m_globalState = State::ErrorBind;

    if(listen(m_socketDescriptor, 3) < 0)
        return m_globalState = State::ErrorListening;


    m_globalState = State::Up;

    std::cout<< "Server started on port: " << m_port << std::endl;

    m_handlerThread = std::thread([this]{handlingLoop();});

    return m_globalState;
}

void TTServer::handlingLoop()
{
    std::cout << "Start poling" << std::endl;

    struct pollfd fds[2];

    fds[0].fd = m_socketDescriptor;
    fds[0].events = POLLIN;

    while (m_globalState == State::Up) {
        struct sockaddr_in clientAddr;
        int addrlen = sizeof (struct sockaddr_in);

        int pollStatus = poll(fds, 1, 4000);

        if(pollStatus < 0) {
            m_globalState = State::ErrorPoll;
            return;
        } else if (pollStatus > 0) {

            if (fds[0].revents & POLLIN) {
                fds[0].revents = 0;

                std::cout << "Start new thread" << std::endl;
                int clientSocket = -1;
                if (m_globalState == State::Up && (clientSocket = accept(m_socketDescriptor, (struct sockaddr*)&clientAddr, (socklen_t*)&addrlen)) >= 0) {

                    // Process new connection
                    m_clientThreads.push_back(std::thread([this, clientSocket, clientAddr] {
                        std::cout<<"Connected host: "<< hostToString(clientAddr.sin_addr.s_addr, clientAddr.sin_port)<<std::endl;

                        while (true) {
                            TTClient *client = authSection(clientSocket);
                            if (client == nullptr)
                                break;

                            m_clientsMutex.lock();
                            m_clients.push_back(client);
                            m_clientsMutex.unlock();

                            if (!workerLoop(clientSocket, client)) {
                                break;
                            }
                        }

                        std::cout<<"Disconnected host: "<< hostToString(clientAddr.sin_addr.s_addr, clientAddr.sin_port)<<std::endl;
                        m_finishedClientThreadIds.push_back(std::this_thread::get_id());
                    }));
                }
            }
        }

        shutDownFinished();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

TTServer::State TTServer::currentState()
{
    return m_globalState.load();
}

void TTServer::writeToGlobal(std::string data, std::string login)
{
    const std::lock_guard<std::mutex> lock(m_clientsMutex);

    m_historyManager->addMessageToHistory("[GLOBAL][" + login + "]" + data + "[MESSAGEEND]\n");

    for (TTClient *client : m_clients) {
        client->sendToClient("[GLOBAL][" + login + "]" + data + "[MESSAGEEND]\n");
    }
}

void TTServer::joinLoop()
{
    m_handlerThread.join();
}

void TTServer::globalMessageHandle(std::string message)
{
    if (TTHelpers::hasEnding(message, "[MESSAGEEND]\n")) {
        const std::lock_guard<std::mutex> lock(m_clientsMutex);

        m_historyManager->addMessageToHistory(message);

        for (TTClient *client : m_clients) {
            client->sendToClient(message);
        }
    }
}

std::string TTServer::registration(std::string login, std::string password)
{
    switch (m_authManager->registration(login, password)) {
    case TTAuthManager::RegisterError::Succes:
        return "[SuccessRegistration]";
    case TTAuthManager::RegisterError::AlreadyExist:
        return "[AlreadyExist]";
    case TTAuthManager::RegisterError::ShortPassword:
        return "[ShortPassword]";
    case TTAuthManager::RegisterError::HaveRestrictedSymbols:
        return "[HaveRestrictedSymbols]";
    }
    return "";
}

TTClient *TTServer::authSection(int clientSocket)
{
    std::string message = "[AuthRequired]\n";
    if (send(clientSocket, message.c_str(), message.length(), 0) < 0) {
        std::cerr << "AuthSection: can't send auth request" << std::endl;
        return nullptr;
    }

    std::string data;
    std::regex authStringRegExp("\\w+\\|\\w{6,20}");
    while (true) {
        char buffer[clientBufferSize];
        int result = recv(clientSocket, buffer, clientBufferSize, 0);

        if (result <= 0) {
            std::cerr << "AuthSection: can't read client response" << std::endl;
            return nullptr;
        }

        data.assign(buffer, result);

        bool isRegister = false;
        if (data.rfind("[REGISTER]", 0) == 0) {
            data.replace(0, 10, "");
            isRegister = true;
        }

        if (!std::regex_match(data, authStringRegExp)) {
            std::string message = isRegister ? "[BadRegister]" : "[BadAuth]";
            int result = send(clientSocket, message.c_str(), message.length(), 0);

            if (result < 0) {
                std::cerr << "AuthSection: error while sending bad status of incoming data" << std::endl;
                return nullptr;
            }
            continue;
        }
        // TODO: add additional verification

        std::list<std::string> list = TTHelpers::split(data, "\\|");

        if (isRegister) {
            message = registration(list.front(), list.back());
            if (send(clientSocket, message.c_str(), message.length(), 0) < 0) {
                std::cerr << "RegistrationSection: error while sending registration response" << std::endl;
                return nullptr;
            }
            continue;
        }

        switch (m_authManager->login(list.front(), list.back())) {

        case TTAuthManager::AuthError::Succes:
            message = "[SuccessAuth]" + list.front() + "\n";

            if (send(clientSocket, message.c_str(), message.length(), 0) < 0) {
                std::cerr << "AuthSection: error while sending sucess message to client" << std::endl;
                return nullptr;
            }
            return new TTClient(std::this_thread::get_id(), clientSocket, list.front());
        case TTAuthManager::AuthError::UserNotExist:
            message = "[UserNotExist]";
            break;

        case TTAuthManager::AuthError::WrongPassOrLogin:
            message = "[WrongPassOrLogin]";
            break;

        case TTAuthManager::AuthError::AlreadyAuthorized:
            message = "[AlreadyAuthorized]";
            break;
        }

        if (send(clientSocket, message.c_str(), message.length(), 0) < 0) {
            std::cerr << "AuthSection: error message send error" << std::endl;
            return nullptr;
        }
    }

    return nullptr;
}

void TTServer::shutDownFinished()
{
    if(m_finishedClientThreadIds.empty()) {
        return;
    }
    // It is not a best solution but it works
    for(std::list<std::thread::id>::iterator idIt = m_finishedClientThreadIds.begin (); !m_finishedClientThreadIds.empty() ; idIt = m_finishedClientThreadIds.begin()) {
        for(std::list<std::thread>::iterator thrIt = m_clientThreads.begin (); thrIt != m_clientThreads.end () ; ++thrIt) {
            if(thrIt->get_id () == *idIt) {
                thrIt->join(); // Wait for finish the thread
                m_clientThreads.erase(thrIt);
                m_finishedClientThreadIds.erase(idIt);
                break;
            }
        }
    }
}

std::string TTServer::hostToString(uint32_t host, uint16_t port)
{
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&host)[0])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&host)[1])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&host)[2])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&host)[3])) + ':' +
           std::to_string(port);
}

bool TTServer::workerLoop(int clientSocket, TTClient *client)
{

    m_historyManager->sendHistory(client);

    writeToGlobal(client->login() + " connected.", "System");

    bool isLogOut = false;
    // Process client requests
    while (m_globalState == State::Up) {
        char buffer[clientBufferSize];

        int size = recv(clientSocket, buffer, clientBufferSize, 0);

        if (size == -1) {
            std::cout << "Recv error" << std::endl;;
            break;
        }
        
        if (size == 0) {
            break; // Means disconnect
        }

        std::string data(buffer, size);

        if (data == "[LOGOUT]") {
            isLogOut = true;
            break;
        }

        if (data.rfind("[GLOBAL]", 0) == 0) {
            globalMessageHandle(data);
            continue;
        }
    }

    m_clientsMutex.lock();
    for(std::list<TTClient*>::iterator clientIt = m_clients.begin (); clientIt != m_clients.end () ; ++clientIt) {
        TTClient *clientFromCollection = (*clientIt);

        if (client->login() == clientFromCollection->login()) {
            m_clients.erase(clientIt);
            break;
        }
    }
    m_clientsMutex.unlock();

    m_authManager->logout(client->login());
    writeToGlobal(client->login() + " disconnected.", "System");
    delete client;

    return isLogOut;
}

TTServer::TTServer(uint16_t port) :
    m_port(port),
    m_authManager(new TTAuthManager()),
    m_historyManager(new TTHistoryManager())
{

}
