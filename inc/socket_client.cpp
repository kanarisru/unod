//
// Created by Andrey on 13.03.2021.
//

#include "socket_client.h"


T_SOCKET_CLIENT SocketClient[_MAX_SOCKET_CLIENTS];


int getFreeSocketClientIndex() {
    for(int i=0;i<_MAX_SOCKET_CLIENTS;i++) if(SocketClient[i].port==0) return i;
    return -1;
}



DWORD WINAPI CHECK_CLIENT_CONNECTION(LPVOID data) {
    SOCKADDR_IN addr;
    while (1) {
        for(int i=0;i<_MAX_SOCKET_CLIENTS;i++) {
            if(SocketClient[i].port==0 || SocketClient[i].connected) continue;
            addr.sin_addr.s_addr = inet_addr(SocketClient[i].addr);
            addr.sin_port = htons(SocketClient[i].port);
            addr.sin_family = AF_INET;
            SocketClient[i].sock = socket(AF_INET, SOCK_STREAM, 0);
            if(connect(SocketClient[i].sock, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
                SocketClient[i].connected = true;
            }

        }
        Sleep(100);
    }
}

void INIT_CLIENT_ALL() {
    for(int i=0;i<_MAX_SOCKET_CLIENTS;i++) {
        SocketClient[i].port = 0;
        SocketClient[i].sock = 0;
    }

    // Поддержание соединения
    CreateThread(nullptr, 0, &CHECK_CLIENT_CONNECTION, nullptr, 0, nullptr);
}

int ADD_SOCKET_CLIENT(unsigned short port, char * addr) {
    int idx = getFreeSocketClientIndex();
    if(idx>0) {
        SocketClient[idx].port = port;
        SocketClient[idx].addr = addr;
        SocketClient[idx].connected = false;
    }
    return idx;
}

int DEL_SOCKET_CLIENT(int clientIdx) {
    return 0;
}

int CLIENT_CLOSE(int clientIdx) {
    closesocket(SocketClient[clientIdx].sock);
    SocketClient[clientIdx].connected = false;
    return -1;
}


int SEND_CLIENT(int clientIdx, char * data, int size, T_ON_CLIENT_ANSWER on_answer) {
    int r;
    if(!SocketClient[clientIdx].connected) return -1;
    r = send(SocketClient[clientIdx].sock, (char*)&size,  sizeof(size), 0);
    if(r==-1) return CLIENT_CLOSE(clientIdx);
    r = send(SocketClient[clientIdx].sock, data,  size, 0);
    if(r==-1) return CLIENT_CLOSE(clientIdx);

}

