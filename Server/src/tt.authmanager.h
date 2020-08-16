#ifndef TTAUTHMANAGER_H
#define TTAUTHMANAGER_H

#include <map>
#include <string>
#include <list>
#include <mutex>

/**
 * @brief The TTAuthManager class for work with credentials
 */
class TTAuthManager
{
public:
    enum class AuthError {
        Succes,
        UserNotExist,
        WrongPassOrLogin,
        AlreadyAuthorized
    };
    enum class RegisterError {
        Succes,
        AlreadyExist,
        ShortPassword,
        HaveRestrictedSymbols
    };

    TTAuthManager();
    /**
     * @brief init fill all iternal containers
     */
    void init();
    AuthError login(std::string login, std::string password);
    RegisterError registration(std::string login, std::string password);

    bool logout(std::string login);

private:
    bool isUniq(std::string login);
    void processLoginInformation(std::string loginSequence);
    std::map<std::string, std::string> m_passLoginMap;
    std::list<std::string> m_authorizedList; // List of already authorized logins
    std::mutex m_authMutex;
};

#endif // TTAUTHMANAGER_H
