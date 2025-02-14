#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include "ChatRoom.h"
#include "ClientHandler.h"

/*
 * [CONTROLLER]
 * 서버의 진입점으로, TCP 소켓을 생성하여 클라이언트 연결을 수락하고,
 * 각 클라이언트에 대해 ClientHandler(컨트롤러)를 생성 후, ChatRoom(모델/메디에이터)에 등록합니다.
 */
int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    int port = std::atoi(argv[1]);

    // 서울 기준 시간 사용 (TZ 환경변수 설정)
    setenv("TZ", "Asia/Seoul", 1);
    tzset();

    // TCP 소켓 생성
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    // 주소 재사용 옵션 설정
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt error" << std::endl;
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if(listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    // [MODEL] 단 하나의 공용 채팅방(모델 및 메디에이터)
    ChatRoom chatRoom;

    while(true) {
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int new_socket = accept(server_fd, (struct sockaddr*)&clientAddr, &addrLen);
        if(new_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // [CONTROLLER] 새 클라이언트를 위한 ClientHandler(컨트롤러) 생성 (동적할당)
        ClientHandler *handler = new ClientHandler(new_socket, &chatRoom);

        // 클라이언트로부터 일회용 닉네임 수신 (최초 메시지로 가정)
        char nickBuffer[256];
        std::memset(nickBuffer, 0, sizeof(nickBuffer));
        int n = recv(new_socket, nickBuffer, sizeof(nickBuffer)-1, 0);
        if(n <= 0) {
            close(new_socket);
            delete handler;
            continue;
        }
        std::string nickname(nickBuffer);
        if(!nickname.empty() && nickname.back() == '\n') {
            nickname.pop_back();
        }
        handler->setNickname(nickname);

        // [MODEL/CONTROLLER] 채팅방(모델)에 클라이언트 등록 후, Hello 메시지 브로드캐스트
        chatRoom.registerClient(handler);
        std::string helloMessage = "Hello " + nickname + "!\n";
        chatRoom.broadcastMessage(helloMessage);

        // 각 클라이언트와의 통신은 별도 스레드에서 처리
        std::thread t(&ClientHandler::run, handler);
        t.detach();
    }
    close(server_fd);
    return 0;
}

