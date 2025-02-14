#include "ClientHandler.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>   // send, recv 함수 선언

ClientHandler::ClientHandler(int sockfd, ChatRoom *chatRoom)
    : sockfd(sockfd), chatRoom(chatRoom) { }

ClientHandler::~ClientHandler() { }

void ClientHandler::setNickname(const std::string &name) {
    nickname = name;
}

std::string ClientHandler::getNickname() const {
    return nickname;
}

void ClientHandler::update(const std::string &message) {
    // [VIEW 역할] 클라이언트 소켓에 메시지 전송
    if(send(sockfd, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Error sending message to " << nickname << std::endl;
    }
}

std::string ClientHandler::getCurrentTime() {
    std::time_t t = std::time(nullptr);
    struct tm tm_result;
    localtime_r(&t, &tm_result);  // TZ 환경변수로 서울 기준 시간 사용
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "(%Y-%m-%d) (%H:%M)", &tm_result);
    return std::string(buffer);
}

void ClientHandler::run() {
    char buffer[1024];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(sockfd, buffer, sizeof(buffer)-1, 0);
        if (bytesReceived <= 0) {
            break;  // 연결 종료 또는 오류 발생
        }
        std::string msg(buffer);
        if (!msg.empty() && msg.back() == '\n') {
            msg.pop_back();
        }
        // [추가] !exit 입력 시 루프 종료 (메시지 전송 없이 종료)
        if (msg == "!exit") {
            break;
        }
        // [CONTROLLER] 받은 메시지를 포맷팅하여 모델(채팅방)에 전달
        std::string formattedMessage = nickname + " : " + msg + " " + getCurrentTime() + "\n";
        chatRoom->sendMessage(formattedMessage, this);
    }
    // 클라이언트 연결 종료 시 Bye 메시지 브로드캐스트
    std::string byeMessage = "Bye " + nickname + "!\n";
    chatRoom->broadcastMessage(byeMessage);
    chatRoom->removeClient(this);
    close(sockfd);
    delete this; // 동적 할당 객체 삭제
}

