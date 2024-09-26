#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>

#define SOCKET_PATH "/tmp/unix_socket"
#define BUFFER_SIZE 1024

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;

    // 소켓 생성
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 이미 존재하는 소켓 파일 삭제
    unlink(SOCKET_PATH);

    // 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("바인딩 실패");
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_fd, 5) == -1) {
        perror("리스닝 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버가 %s에서 연결을 기다리고 있습니다...\n", SOCKET_PATH);

    // 클라이언트 연결 수락
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("연결 수락 실패");
        exit(EXIT_FAILURE);
    }

    printf("클라이언트가 연결되었습니다.\n");

    char buffer[BUFFER_SIZE];
    struct timeval tv;
    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 500000; // 0.5초

        int activity = select(client_fd + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0) {
            perror("select 오류");
            break;
        }

        if (FD_ISSET(client_fd, &readfds)) {
            ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received <= 0) {
                if (bytes_received == 0) {
                    printf("클라이언트가 연결을 종료했습니다.\n");
                } else {
                    perror("recv 오류");
                }
                break;
            }

            buffer[bytes_received] = '\0';
            printf("받은 메시지: %s\n", buffer);
        }
    }

    // 소켓 닫기
    close(client_fd);
    close(server_fd);

    // 소켓 파일 삭제
    unlink(SOCKET_PATH);

    return 0;
}