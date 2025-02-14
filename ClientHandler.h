#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <string>
#include "Patterns.h"      // Observer와 Mediator 통합 헤더
#include "ChatRoom.h"

/*
 * [CONTROLLER] / [OBSERVER 패턴 구현]
 * ClientHandler는 각 클라이언트와의 통신(메시지 송수신)을 담당하며,
 * ChatRoom(모델/메디에이터)과 상호작용합니다.
 */
class ClientHandler : public Observer {
public:
    ClientHandler(int sockfd, ChatRoom *chatRoom);
    ~ClientHandler();

    // [CONTROLLER] 클라이언트로부터 최초에 전달받은 일회용 닉네임 설정
    void setNickname(const std::string &name);
    std::string getNickname() const;

    // [OBSERVER] ChatRoom으로부터 메시지 업데이트를 전달받아 클라이언트 소켓에 전송
    void update(const std::string &message) override;

    // [CONTROLLER] 클라이언트와의 통신 처리 (메시지 수신 및 가공 후 ChatRoom에 전달)
    void run();

private:
    int sockfd;
    std::string nickname;
    ChatRoom *chatRoom;

    // [HELPER - CONTROLLER] 서울 기준 현재 시간 (YYYY-MM-DD) (HH:MM) 문자열 반환
    std::string getCurrentTime();
};

#endif

