#ifndef PATTERNS_H
#define PATTERNS_H

#include <string>

/*
 * [디자인 패턴: Observer + Mediator]
 *
 * Observer 인터페이스는 클라이언트(뷰)에게 업데이트(새로운 메시지)를 전달하기 위한
 * 순수 가상함수를 정의합니다.
 *
 * Mediator 인터페이스는 중앙에서 클라이언트 간의 통신(메시지 전달)을 중재합니다.
 */

class Observer {
public:
    virtual ~Observer() {}
    // 클라이언트(뷰)에게 메시지 업데이트를 전달 (예: 소켓에 전송)
    virtual void update(const std::string &message) = 0;
};

class Mediator {
public:
    virtual ~Mediator() {}
    // 메시지를 sender를 제외한 다른 Observer(클라이언트)에게 전달
    virtual void sendMessage(const std::string &message, Observer *sender) = 0;
};

#endif

