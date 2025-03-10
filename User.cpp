#include "user.h"

// Dummy storage for users (replace with database or file storage)
#include <unordered_map>

static std::unordered_map<std::string, std::string> userDatabase;

bool User::login(const std::string& username, const std::string& password) {
    return userDatabase.find(username) != userDatabase.end() && userDatabase[username] == password;
}

bool User::registerUser(const std::string& username, const std::string& password) {
    if (userDatabase.find(username) != userDatabase.end()) {
        return false; // User already exists
    }
    userDatabase[username] = password;
    return true;
}
