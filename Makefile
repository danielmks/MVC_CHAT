CXX = g++
CXXFLAGS = -std=c++11 -pthread -Wall

# 서버 관련 소스 파일 (모델, 컨트롤러, 패턴 구현 등)
SERVER_SRCS = main.cpp ChatRoom.cpp ClientHandler.cpp
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
SERVER_TARGET = server

# 클라이언트 관련 소스 파일 (뷰/컨트롤러)
CLIENT_SRCS = client.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)
CLIENT_TARGET = client

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_TARGET) $(CLIENT_TARGET)

