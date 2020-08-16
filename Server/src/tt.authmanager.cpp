#include "tt.authmanager.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <src/tt.helpers.h>

const std::string k_storagePath = "authStorage";
const std::string k_delemiter = "|";
const int k_minimalPasswordLength = 6;
const std::list<std::string> k_restrictedSymbols = {"|"};

TTAuthManager::TTAuthManager()
{

}

void TTAuthManager::init()
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    m_passLoginMap.clear();
    m_authorizedList.clear();

    std::ofstream touchStream;
    touchStream.open(k_storagePath, std::ios::out | std::ios::app);
    touchStream.close();

    std::ifstream readStream;

    readStream.open(k_storagePath, std::ios::in);

    if (!readStream.is_open()) {
        std::cerr << "Can't open auth storage!" << std::endl;
    }

    std::string lineData;
    while (std::getline(readStream, lineData)) {
        processLoginInformation(lineData);
    }

    readStream.close();
}

bool TTAuthManager::isUniq(std::string login)
{
    return m_passLoginMap.find(login) == m_passLoginMap.end();
}

TTAuthManager::AuthError TTAuthManager::login(std::string login, std::string password)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (std::find(m_authorizedList.begin(), m_authorizedList.end(), login) != m_authorizedList.end()) {
        return AuthError::AlreadyAuthorized;
    }

    std::map<std::string, std::string>::iterator authIterator = m_passLoginMap.find(login);

    if (authIterator == m_passLoginMap.end()) {
        return AuthError::UserNotExist;
    }

    if (authIterator->second != password) {
        return AuthError::WrongPassOrLogin;
    }

    m_authorizedList.push_back(login);

    return AuthError::Succes;
}

TTAuthManager::RegisterError TTAuthManager::registration(std::string login, std::string password)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    if (!isUniq(login)) {
        return RegisterError::AlreadyExist;
    }

    if (password.length() < k_minimalPasswordLength) {
        return  RegisterError::ShortPassword;
    }

    for(std::string symbol : k_restrictedSymbols) {
        if (login.find(symbol) != std::string::npos
                || password.find(symbol) != std::string::npos) {
            return RegisterError::HaveRestrictedSymbols;
        }
    }

    m_passLoginMap.emplace(login, password);

    std::ofstream outStreamAuthFile;
    outStreamAuthFile.open(k_storagePath, std::ios::out | std::ios::app);
    outStreamAuthFile << login + k_delemiter + password << std::endl;
    outStreamAuthFile.close();

    return RegisterError::Succes;
}

bool TTAuthManager::logout(std::string login)
{
    std::lock_guard<std::mutex> lock(m_authMutex);

    std::list<std::string>::iterator iterator = std::find(m_authorizedList.begin(), m_authorizedList.end(), login);

    if (iterator == m_authorizedList.end() ) {
        return false;
    }

    m_authorizedList.erase(iterator);

    return true;
}

void TTAuthManager::processLoginInformation(std::string loginSequence)
{
    std::list<std::string> list = TTHelpers::split(loginSequence, "\\|");

    if (list.size() == 2) {
        m_passLoginMap.emplace(list.front(), list.back());
    }
}
