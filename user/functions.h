#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include <fstream>
#include <QMessageBox>
#include <windows.h>

void registerUser(const std::string& username, const std::string& password);
bool checkIfNameExistsAlready(const std::string& username);
bool loginUser(const std::string& username, const std::string& password);
bool sendMessage(const std::string& message);

#endif // FUNCTIONS_H
