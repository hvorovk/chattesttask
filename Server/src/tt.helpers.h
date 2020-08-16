#ifndef TT_HELPERS_H
#define TT_HELPERS_H

#include <string>
#include <list>
#include <regex>

class TTHelpers
{
public:
    static std::list<std::string> split(const std::string& input, const std::string& regex) {
        // passing -1 as the submatch index parameter performs splitting
        std::regex re(regex);
        std::sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
            last;
        return {first, last};
    }

    static bool hasEnding (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }
};


#endif // TT_HELPERS_H
