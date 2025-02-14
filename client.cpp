#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * [VIEW] - 서버로부터 메시지를 수신하여 출력하고,
 * 현재 입력 프롬프트를 다시 그리는 함수.
 */
void receiveMessages(int sockfd, const std::string &nickname) {
    char buffer[1024];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int n = recv(sockfd, buffer, sizeof(buffer)-1, 0);
        if (n <= 0) {
            std::cerr << "\n서버와의 연결이 종료되었습니다." << std::endl;
            break;
        }
        // ANSI escape sequence로 현재 줄 지우기 및 커서 이동
        std::cout << "\33[2K\r";  // 현재 줄 전체 삭제 후 커서를 처음으로 이동
        // 수신된 메시지 출력
        std::cout << buffer << std::endl;
        // 다시 프롬프트 출력 (커서가 여기서 깜빡임)
        std::cout << nickname << " : " << std::flush;
    }
}

/*
 * [CONTROLLER] - 사용자 입력을 읽어 서버로 전송하는 함수.
 * 매 입력 전에 프롬프트를 출력합니다.
 */
void sendMessages(int sockfd, const std::string &nickname) {
    std::string input;
    while (true) {
        // 프롬프트 출력 (입력 대기)
        std::cout << nickname << " : " << std::flush;
        if (!std::getline(std::cin, input))
            break;
        input += "\n";
        // "!exit" 입력 시 서버에 전송하고 루프 종료
        if (input == "!exit\n") {
            if (send(sockfd, input.c_str(), input.size(), 0) < 0) {
                std::cerr << "메시지 전송 중 오류 발생" << std::endl;
            }
            break;
        }
        if (send(sockfd, input.c_str(), input.size(), 0) < 0) {
            std::cerr << "메시지 전송 중 오류 발생" << std::endl;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port> <nickname>" << std::endl;
        return 1;
    }
    std::string server_ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string nickname = argv[3];

    // TCP 소켓 생성
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        std::cerr << "소켓 생성 실패" << std::endl;
        return 1;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if(inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "잘못된 주소 또는 지원되지 않는 주소" << std::endl;
        return 1;
    }
    if(connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "서버 연결 실패" << std::endl;
        return 1;
    }
    
    // 접속 후, [초기 설정] 닉네임 전송 (일회용)
    std::string nickMessage = nickname + "\n";
    send(sockfd, nickMessage.c_str(), nickMessage.size(), 0);
    
    // 별도의 스레드에서 메시지 수신과 송신 처리 ([VIEW]와 [CONTROLLER] 역할 분담)
    std::thread recvThread(receiveMessages, sockfd, nickname);
    std::thread sendThread(sendMessages, sockfd, nickname);
    
    recvThread.join();
    sendThread.join();
    
    close(sockfd);
    return 0;
}

