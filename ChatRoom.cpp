#include "ChatRoom.h"
#include <algorithm>

ChatRoom::ChatRoom() { }

ChatRoom::~ChatRoom() { }

void ChatRoom::registerClient(Observer *client) {
    std::lock_guard<std::mutex> lock(mtx);
    clients.push_back(client);
}

void ChatRoom::removeClient(Observer *client) {
    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
}

/*
 * [수정]
 * sender를 제외하지 않고 모두에게 메시지를 전송하여, 자신의 메시지도 수신하게 함.
 */
void ChatRoom::sendMessage(const std::string &message, Observer *sender) {
    std::lock_guard<std::mutex> lock(mtx);
    for(auto client : clients) {
        client->update(message);
    }
}

void ChatRoom::broadcastMessage(const std::string &message) {
    std::lock_guard<std::mutex> lock(mtx);
    for(auto client : clients) {
        client->update(message);
    }
}

