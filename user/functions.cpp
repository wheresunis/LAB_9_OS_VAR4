#include "functions.h"

void registerUser(const std::string& username, const std::string& password) {
    std::ofstream file("users.txt", std::ios::app);
    if (!file) {
        QMessageBox::information(nullptr, "Error", "Unable to open users file");
        return;
    }
    // записуємо номер користувача, ім'я та пароль у файл
    file << username << " " << password << "\n";
    file.close();
}

bool loginUser(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    if (!file) {
        QMessageBox::information(nullptr, "Error", "Unable to open users file");
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // розділяємо рядок на ім'я користувача та пароль
        std::string fileUsername = line.substr(0, line.find(' '));
        std::string filePassword = line.substr(line.find(' ') + 1);

        if (fileUsername == username && filePassword == password) {
            return true; // логін успішний
        }
    }

    return false; // логін неуспішний
}


bool checkIfNameExistsAlready(const std::string& username)
{
    std::ifstream file("users.txt");
    if (!file) {
        QMessageBox::information(nullptr, "Error", "Unable to open users file");
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        // виділяємо ім'я користувача (до пробілу)
        std::string fileUsername = line.substr(0, line.find(' '));

        if (fileUsername == username) {
            return true; // користувач знайдений
        }
    }

    return false; // користувача немає
}

bool sendMessage(const std::string& message) {
    HANDLE pipe = CreateFile(TEXT("\\\\.\\pipe\\ForumPipe"), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (pipe == INVALID_HANDLE_VALUE) {
        QMessageBox::information(nullptr, "Error", "Unable to connect to forum");
        return false;
    }
    DWORD bytesWritten;
    WriteFile(pipe, message.c_str(), message.size() + 1, &bytesWritten, nullptr);
    CloseHandle(pipe);
    return true;
}
