#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "/tmp/unix_socket"
#define MESSAGE_COUNT 10

int main(void)
{
    int client_fd;
    struct sockaddr_un server_addr;

    // 소켓 생성
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 서버에 연결
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("서버 연결 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버에 연결되었습니다.\n");

    char message[100];
    int count = 0;

    while (count < MESSAGE_COUNT) {
        snprintf(message, sizeof(message), "메시지 #%d", count + 1);
        
        if (send(client_fd, message, strlen(message), 0) == -1) {
            perror("메시지 전송 실패");
            break;
        }
        
        printf("전송된 메시지: %s\n", message);
        
        count++;
        usleep(50000); // 0.5초 대기
    }

    printf("모든 메시지를 전송했습니다.\n");

    // 소켓 닫기
    close(client_fd);

    return 0;
}
