#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#define FILE_TO_SEND "/home/andrew/Desktop/voprosi.docx"
using namespace std;
void die(char *s);
bool ifFileExist(string fileName);
int SendAllData(int sock,void *buf,int len,int flags);
void StartTcpServer(char * address, int port);
void StartUpdServer(char * address, int port);
int main(int argc, char *argv[])
{
        
        char *address = argv[1];
        int port = atoi(argv[2]);
        cout << "Server @ - "<<address << ", port - " << port << "\n";
        
        switch(fork())
        {
                case -1:
                        perror("fork");
                        break;
                case 0:
                        StartTcpServer(address,port);
                        break;
                default:
                        StartUpdServer(address,port);
                        break;
        }
        
        
        
        

}

int SendAllData(int sock,void *buf,int len,int flags)
{
        int totalSend = 0;
        int n;
        
        while(totalSend < len)
        {
                n = send(sock,buf+totalSend,len - totalSend,flags);
                if(n== -1)
                        break;
                totalSend += n;
        }
        
        return (n==-1 ? -1 : totalSend);
}

void StartTcpServer(char * address, int port)
{
        int sock, listener;
        struct sockaddr_in addr;
        
        
        
        listener = socket(AF_INET, SOCK_STREAM, 0);
        if(listener < 0)
        {
                die("socket");
        }
        
        addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
        inet_aton(address, &addr.sin_addr);
        
        if(bind(listener, (struct sockaddr *)&addr, sizeof(addr))<0)
        {
                die("bind");
        }
        listen(listener, 1);
        
        while(1)
        {
                char buf[1024];
                int bytes_read;
                sock = accept(listener, NULL, NULL);
                if(sock < 0)
                {
                        die("accept");
                }                
                bytes_read = recv(sock, buf, 1024, 0);
                if(bytes_read <= 0) 
                {
                        close(sock);
                }
                else 
                {
                        string toSendFile(buf, bytes_read);
                        cout << key +"\n";
                        if(ifFileExist(toSendFile))
                        {
                                FILE *fileToSend = fopen(FILE_TO_SEND,"r");
                                if(fileToSend == NULL)
                                {
                                        cout<< "Cannot open file" << "\n";
                                        close(sock);
                                }
                                else
                                {
                                        fseek(fileToSend, 0, SEEK_END);
                                        int sz = ftell(fileToSend);
                                        send(sock,(char *)&sz,sizeof(sz),0);
                                        int possition;
                                        recv(sock,&possition,sizeof(possition),0);
                                        fseek(fileToSend,possition, SEEK_SET);
                                        while(possition < sz)
                                        {
                                               
                                                char buffer[1024];
                                                int read = fread(buffer,sizeof(char),1024,fileToSend);
                                                int send = SendAllData(sock,buffer,read,0);
                                                if(send != read)
                                                {
                                                        perror("sending");
                                                        break;
                                                }
                                                possition += read;
                                        }
                                        cout << "ready" << "\n";
                                        fclose(fileToSend);
                                        close(sock);
                                }
                        }
                        else
                        {
                                int size = 0;
                                send(sock,&size,sizeof(size),0);
                                close(sock);
                        }
                        
                }    
                

        }
        
}

void StartUpdServer(char * address, int port)
{
        int sock, recv_len;

        struct sockaddr_in addr, si_other;
        socklen_t slen = sizeof(si_other);
        char buf[1024];
        sock = socket(AF_INET,SOCK_DGRAM,0);
        
        if(sock <0)
        {
                perror("socket");
                exit(1);
        }
        
        
        addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
        inet_aton(address, &addr.sin_addr);
        
        if(bind(sock,(struct sockaddr *)&addr,sizeof(addr)) <0)
        {
                perror("bind");
                exit(1);
        }
        
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
                die("recvfrom()");
        }
        if(ifFileExist(string(buf)))
        {
            string approve("\nOK THIS FILE\n");
             if (sendto(sock, approve.c_str(), approve.length(), 0 , (struct sockaddr *) &addr, slen)==-1)
             {
                die("sendto()");
             }
        }
        while(1)
        {
            printf("Waiting for data...");
            //fflush(stdout);
            memset(buf,'\0', sizeof(buf));

            //try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &si_other, &slen)) == -1)
            {
                die("recvfrom()");
            }
             
            //print details of the client/peer and the data received
            printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
            cout << "data len" << strlen(buf) << "\n";
            printf("Data: %s\n" , buf);
             
            //now reply the client with the same data
            if (sendto(sock, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            {
                die("sendto()");
            }
        }
 
    close(sock);
}


bool ifFileExist(string fileName)
{
    FILE *file;
    string fullPath = "/home/andrew/Desktop/";
    fullPath += fileName;
    cout << fullPath.c_str();
    if (file = fopen(fullPath.c_str(), "r"))
    {
        fclose(file);
        return 1;
    }

    return 0;
}

void die (char *s)
{
    perror(s);
    exit(1);
}

