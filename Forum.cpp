#include <iostream>
#include <windows.h>

void setConsoleTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Прийом повідомлення від користувача
std::string receiveMessageFromUser() {
    HANDLE pipe = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\ForumPipe"),
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        1, 0, 0, 0, nullptr);

    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to create named pipe.\n";
        return "";
    }

    ConnectNamedPipe(pipe, nullptr);

    char buffer[1024] = { 0 };
    DWORD bytesRead;
    ReadFile(pipe, buffer, sizeof(buffer), &bytesRead, nullptr);
    CloseHandle(pipe);

    return std::string(buffer);
}
// Надсилання повідомлення серверу
bool sendToServer(const std::string& message) {
    HANDLE pipe = CreateFile(TEXT("\\\\.\\pipe\\ServerPipe"), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to connect to server.\n";
        return false;
    }

    DWORD bytesWritten;
    WriteFile(pipe, message.c_str(), message.size() + 1, &bytesWritten, nullptr);
    CloseHandle(pipe);
    return true;
}

// Прийом відповіді від сервера
void receiveServerResponse() {
    HANDLE pipe = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\ResponsePipe"),
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        1, 0, 0, 0, nullptr);

    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to create response pipe.\n";
        return;
    }

    ConnectNamedPipe(pipe, nullptr);

    char buffer[1024] = { 0 };
    DWORD bytesRead;
    ReadFile(pipe, buffer, sizeof(buffer), &bytesRead, nullptr);
    CloseHandle(pipe);

    std::string response(buffer);
    if (response.find("APPROVED") != std::string::npos) {
        setConsoleTextColor(10); // Зелений колір
        std::cout << "Message published: \n";
        std::cout << response.substr(response.find('\n') + 1) << "\n\n"; // Вивести цензуроване повідомлення
    }
    else {
        setConsoleTextColor(12); // Червоний колір
        std::cout << "Message rejected due to excessive banned words.\n\n";
    }

    setConsoleTextColor(7); // Повернення до стандартного кольору
}


int main() {
    while (true) {
        std::string message = receiveMessageFromUser();

        if (message.empty()) {
            std::cout << "Exiting...\n";
            break;
        }

        if (!sendToServer(message)) {
            std::cerr << "Error: Could not send message to server.\n";
            continue;
        } 

        receiveServerResponse();
    }

    return 0;
}
