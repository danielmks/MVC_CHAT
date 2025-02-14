#ifndef CHATROOM_H
#define CHATROOM_H

#include <vector>
#include <mutex>
#include "Patterns.h"  // Observer와 Mediator 통합 헤더

/*
 * [MODEL] / [MEDIATOR 패턴 구현]
 * ChatRoom은 단 하나의 공용 채팅방 역할을 하며, 클라이언트(Observer)들을 관리하고,
 * 메시지 전달을 중재(Mediator)합니다.
 */
class ChatRoom : public Mediator {
public:
    ChatRoom();
    ~ChatRoom();

    // 모델: 클라이언트(Observer) 등록 및 제거
    void registerClient(Observer *client);
    void removeClient(Observer *client);

    // 메디에이터: sender를 제외한 클라이언트에게 메시지 전달
    void sendMessage(const std::string &message, Observer *sender) override;

    // 메디에이터: 모든 클라이언트에게 메시지 전달 (브로드캐스트)
    void broadcastMessage(const std::string &message);

private:
    std::vector<Observer*> clients;
    std::mutex mtx;
};

#endif

