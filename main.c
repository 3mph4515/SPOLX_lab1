#include <stdio.h>
#include <sys/types.h>
//#include <WinInet.h>
#include <WinSock2.h>

int main (int argc, char *argv[])
{
	int sock, listener;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;
	WSADATA wsaData;

	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listener < 0)
    {
        perror("socket");
		printf("%d", WSAGetLastError());
        exit(1);
    }

	addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr((argv[1]));
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);
    
    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }

        while(1)
        {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) 
				break;
			else 
				for(int i = 0; i < bytes_read; i++)
					printf("%c", buf[i]);
        }
    
        closesocket(sock);
    }
}
