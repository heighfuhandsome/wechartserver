#pragma once

#include <string>

class User
{
public:
    User() : online_(false) {}
    ~User() = default;
    void setId(unsigned int id) { id_ = id; }
    void setAccount(const std::string &account) { account_ = account; }
    void setPassword(const std::string &password) { password_ = password; }
    void setNickname(const std::string &nickname) { nickname_ = nickname; }
    void setOnline(bool b) { online_ = b; }

    std::string getAccount() const { return account_; }
    std::string getPassword() const { return password_; }
    std::string getNickname() const { return nickname_; }
    unsigned int getId() const { return id_; }
    bool isOnline() const { return online_; }

private:
    unsigned int id_;
    std::string account_;
    std::string password_;
    std::string nickname_;
    bool online_;
};