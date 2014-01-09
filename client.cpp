#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <utility>
#define PATH "./"
using namespace std;
void die (char *s);
void startTcpClient(char * adress,int port, char *fileName);
void startUdpClient(char *address, int port);
bool ifFileExist(string fileName);
int main (int argc, char *argv[])
{

        if(argc < 5)
        {
                perror("ip port {-t|-u} filename");
                exit(1);
        }
        else
        {    
                char *address = argv[1];
                int port = atoi(argv[2]);
                char *fileName = argv[4];
                if(!strcmp(argv[3],"-t"))
                        startTcpClient(address,port,fileName);
                else
                {
                        if(!strcmp(argv[3],"-u"))
                                startUdpClient(address,port);
                        else
                                return 0;
                }
        }
        return 0;
}



void startTcpClient(char * address,int port, char *fileName)
{
        int sock;
        struct sockaddr_in addr;
        
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if(sock <0)
        {
                perror("socket");
                exit(1);
        }
        
        addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
        inet_aton(address, &addr.sin_addr);
        
        if(connect(sock,(struct sockaddr *)&addr,sizeof(addr)) < 0)
        {
                perror("connect");
                exit(1);                
        }
        
        //string fileName;//zaterminalit'
        //cout << "Enter file you want to download"<< "\n";
        //cin >> fileName;
        //if send //if recv < 0
        send(sock,fileName, strlen(fileName),0);
        int sizeOfFile;
        recv(sock,&sizeOfFile,sizeof(sizeOfFile),0);
        if(sizeOfFile == 0)
        {
                perror("No file on server");
                exit(1);
        }
        cout << "File size - " << sizeOfFile << "\n";
        //HARCODE PLACE TO SAVE
        string filePath(PATH);
        
        filePath += fileName;
        cout << filePath << "\n";
        FILE *fileToSave = fopen(filePath.c_str(),"a+");
        if(fileToSave == NULL)
        {
                perror("error while opening file");
                exit(1);
        }
        fseek(fileToSave,0,SEEK_END);
        int curPossition = ftell(fileToSave);
        if(curPossition != 0)        
        {
                cout << "Find same file, trying to reload" << "\n";
 
        }
        send(sock,&curPossition,sizeof(curPossition),0);
        char buffer[1024];
        int readbytes;
        while(curPossition < sizeOfFile)
        {
                readbytes = recv(sock,&buffer,sizeof(buffer),0);
                if(readbytes > 0)
                {
                        fwrite(buffer,sizeof(char),readbytes,fileToSave);
                }
                if(readbytes <0)
                {
                        fclose(fileToSave);
                        perror("While recive");
                        exit(1);
                }
                curPossition += readbytes;
        }
        fclose(fileToSave);
        cout<< "Ready" << "\n";
        close(sock);
}

void startUdpClient(char *address, int port) 
{
    
    int sock, recv_len;

    struct sockaddr_in addr;
    socklen_t slen = sizeof(addr);
    char buf[1024];
    string message;

    sock = socket(AF_INET,SOCK_DGRAM,0);
        
    if(sock <0)
    {
        perror("socket");
        exit(1);
    }
 
    memset((char *) &addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(address, &addr.sin_addr);
 
    printf("Enter file name: ");
    cin >> message;
    int lll = message.length();
    cout << lll; 
    //send the message
    if (sendto(sock, message.c_str(), message.length(), 0 , (struct sockaddr *) &addr, slen)==-1)
    {
        die("sendto()");
    }

    while(1)
    {
        printf("Enter message : ");
        cin >> message;
        int lll = message.length();
        cout << lll; 
        //send the message
        if (sendto(sock, message.c_str(), message.length(), 0 , (struct sockaddr *) &addr, slen)==-1)
        {
            die("sendto()");
        }
         
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', sizeof(buf));
        cout << "\nafter del buf size" << sizeof(buf) << "\n" << buf << "\n";
        //try to receive some data, this is a blocking call
        if (recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &addr, &slen) == -1)
        {
            die("recvfrom()");
        }
         
        puts(buf);
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

