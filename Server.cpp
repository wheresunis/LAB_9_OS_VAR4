#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <windows.h>
#include <algorithm>
#include <map>

std::mutex mtx;
int totalBannedWords = 0;  // �������� ������� ��������� ����������� ���
std::vector<std::string> foundBannedWords; // ������ ��������� ���
std::map<std::string, int> censoredWords; // ����� ��� ������������ ��� �� ����������

// �������� ����� �� ��������� �������
std::string cleanWord(const std::string& word) {
    std::string clean = word;
    clean.erase(std::remove_if(clean.begin(), clean.end(), [](char c) {
        return !std::isalnum(static_cast<unsigned char>(c));
        }), clean.end());
    std::transform(clean.begin(), clean.end(), clean.begin(), ::tolower);
    return clean;
}

// ������� ������ ����������� ��� �� �����
std::vector<std::string> loadBannedWords() {
    std::vector<std::string> bannedWords;
    std::ifstream file("banned_words.txt");
    if (!file) {
        std::cerr << "Error: Could not open banned words file.\n";
        return bannedWords;
    }

    std::string word;
    while (file >> word) {
        bannedWords.push_back(cleanWord(word));
    }
    file.close();
    return bannedWords;
}

// �������� ����������� �� ����� �����
std::vector<std::string> splitMessage(const std::string& message) {
    std::vector<std::string> words;
    std::istringstream iss(message);
    std::string word;
    while (iss >> word) {
        words.push_back(cleanWord(word)); // ������� ����� ������
    }
    return words;
}

// ��������� �������� ���� ��������� �����
void moderatorTask(const std::string& bannedWord, const std::vector<std::string>& words) {
    int count = 0;
    for (const auto& word : words) {
        if (word == bannedWord) {
            ++count;
        }
    }

    std::lock_guard<std::mutex> lock(mtx);
    totalBannedWords += count;
    if (count > 0) {
        censoredWords[bannedWord] += count;
        for (int i = 0; i < count; ++i) {
            foundBannedWords.push_back(bannedWord);
        }
    }
}

// ������ ����������� �� ������
std::string receiveMessageFromForum() {
    HANDLE pipe = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\ServerPipe"),
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

// ���������� ������ ������
void sendResponseToForum(bool isApproved, const std::string& message) {
    HANDLE pipe = CreateFile(TEXT("\\\\.\\pipe\\ResponsePipe"), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to connect to response pipe.\n";
        return;
    }

    std::string response = isApproved ? "APPROVED\n" + message : "REJECTED\nFound " + std::to_string(totalBannedWords) + " banned words: ";
    if (!isApproved) {
        for (const auto& word : foundBannedWords) {
            response += word + " ";
        }
    }

    DWORD bytesWritten;
    WriteFile(pipe, response.c_str(), response.size() + 1, &bytesWritten, nullptr);
    CloseHandle(pipe);
}

// ����� ���������� � ����
void writeBanWordsCountToFile(const std::string& message) {
    std::ofstream file("banwordsfound.txt", std::ios::app); // ��������� �������� �����
    if (!file) {
        std::cerr << "Error: Could not open banwordsfound.txt.\n";
        return;
    }

    file << "Message: \"" << message << "\"\n";
    file << "Found " << totalBannedWords << " banned words: ";
    for (const auto& word : foundBannedWords) {
        file << word << " ";
    }
    file << "\n\n";

    file.close();
}

// ������������ �����������
std::string censorMessage(const std::string& message) {
    std::vector<std::string> words = splitMessage(message);
    std::string result;

    for (const auto& word : words) {
        std::string clean = cleanWord(word);
        if (censoredWords.find(clean) != censoredWords.end()) {
            result += std::string(clean.size(), '*') + " ";
        }
        else {
            result += word + " ";
        }
    }

    if (!result.empty() && result.back() == ' ') {
        result.pop_back(); // ��������� ������ � ����
    }
    return result;
}

int main() {
    while (true) {
        std::string message = receiveMessageFromForum();
        if (message.empty()) break;

        auto bannedWords = loadBannedWords();
        auto words = splitMessage(message);

        totalBannedWords = 0;
        foundBannedWords.clear();
        censoredWords.clear();

        // ��������� ����������
        std::vector<std::thread> moderators;
        for (const auto& bannedWord : bannedWords) {
            moderators.emplace_back(moderatorTask, bannedWord, words);
        }

        // ������ ���������� ������ ��� ����������
        for (auto& moderator : moderators) {
            moderator.join();
        }

        bool isApproved = (totalBannedWords <= 3);
        std::string censoredMessage = censorMessage(message);
        sendResponseToForum(isApproved, censoredMessage);
        writeBanWordsCountToFile(message);
    }

    return 0;
}
