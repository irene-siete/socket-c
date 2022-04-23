#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {

    //initialize winsock

    WSADATA wsdata;
    WORD ver= MAKEWORD(2,2);

    int wsOk= WSAStartup(ver, &wsdata);

    if(wsOk!=0){
        cerr<<"cant initialize winsock, adio"<<endl;
        return 0;
    }

    //create socket

    SOCKET listening=socket(AF_INET, SOCK_STREAM, 0);
    if(listening==INVALID_SOCKET){
        cerr<<"cant create a socket"<<endl;
        return 0;
    }

    //bind the ip address and port to a socket

    sockaddr_in hint;
    hint.sin_family= AF_INET;
    hint.sin_port= htons(3000);
    hint.sin_addr.S_un.S_addr= INADDR_ANY;//could also use inet_pton

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    //tell winsock the socket is for listening

    listen(listening, SOMAXCONN);

    //wait for connection
    sockaddr_in client;
    int clientsize= sizeof(client);

    SOCKET clientSocket=accept(listening, (sockaddr*)&client, &clientsize);
    if(clientSocket==INVALID_SOCKET){
        cerr<<"invalid socket"<<endl;
        return 0;
    }
    char host[NI_MAXHOST];      //client's remote name
    char service[NI_MAXHOST];   //service(i.e. port)the client is connected on

    ZeroMemory(host, NI_MAXHOST);//same as memset(host, 0, NI_MAXHOST); in unix
    ZeroMemory(service, NI_MAXHOST);

    if(getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0)==0){
        cout<< host<<"connected on port"<<service<<endl;
    }
    else{
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout<<host<<"connected on port "<<ntohs(client.sin_port)<<endl;
    }

    //close listening socket
    closesocket(listening);

    //while loop: accept and echo message back to client
    char buf[4066];
    while(true){
        ZeroMemory(buf, 4096);
        //wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if(bytesReceived==SOCKET_ERROR){
            cerr<<"error in recv()"<<endl;
            break;
        }
        if(bytesReceived==0){
            cout<<"client disconnected"<<endl;
            break;
        }
        //echo message back to client
        send(clientSocket, buf, bytesReceived+1,0);
    }

    //close the socket
    closesocket(clientSocket);


    //cleanup winsock
    WSACleanup();
}