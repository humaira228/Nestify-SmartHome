#include "user.h"

#include <unordered_map>

static std::unordered_map<std::string, std::string> userDatabase;

bool User::login(const std::string& username, const std::string& password) {
    return userDatabase.find(username) != userDatabase.end() && userDatabase[username] == password;
}

bool User::registerUser(const std::string& username, const std::string& password) {
    if (userDatabase.find(username) != userDatabase.end()) {
        return false; 
    }
    userDatabase[username] = password;
    return true;
}
