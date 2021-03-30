//
// Created by Andrey on 12.03.2021.
//

#include <time.h>
#include <mutex>
#include "socket_server.h"

T_SOCKET_SERVER  SocketServices[_MAX_SOCKET_SERVICES];
T_SOCKET_CONNECTION  SocketConnections[_MAX_SOCKET_CONNECTIONS];
T_SOCKET_THREAD  SocketThreads[_MAX_SOCKET_THREADS];

WSADATA wsaData;

char * ReceiveData (char * data, int size, int clientIdx) {
    printf("ReceiveData. Size: %d, clientIdx = %d\n", size, clientIdx);
    for(int i=0; i < _MAX_SOCKET_CONNECTIONS; i++) {
        if(clientIdx!=i && SocketConnections[i].sock > 0) {
            SEND_TO_CLIENT(i, data, size);
        }
    }
    return nullptr;
}

char * ReceiveData2 (char * data, int size, int connectionIdx) {
    printf("ReceiveData. Size: %d, clientIdx = %d\n", size, connectionIdx);
    return nullptr;
}
char answer1000[1024];

char * ReceiveNat (char * data, int size, int connectionIdx) {
    for(int i=0;i<30;i++) answer1000[i]='A'+i;

//        HOSTENT *hst;
//        hst = gethostbyaddr((char *) &client_addr.sin_addr.s_addr,4, AF_INET);
//        hst = gethostbyaddr((char *) &client_addr.sin_addr.s_addr,4, AF_INET);
//        printf("Host name: %s (%d)\n", hst->h_name, countClients);

    answer1000[30] = 0;
    printf("ReceiveNat. Size: %d, clientIdx = %d\n", size, connectionIdx);
    return answer1000;
}

void INIT_SOCKET_LIB() {
    if (WSAStartup(0x0202, &wsaData)) {
        perror("Winsock lib init error");
        WSACleanup();
        exit(-1);
    }

}

long getMillisecond() {
    struct timeval tp;
    mingw_gettimeofday(&tp, nullptr);
    return tp.tv_sec*1000 + tp.tv_usec/1000;
}


void CLOSE_CLIENT_SOCKET(int idx) {

    for(int i=0;i<_MAX_SOCKET_THREADS;i++) {
        if (SocketThreads[i].connectionIdx == idx) { // Нужно индекс записать в SocketClients и то гда без перебора
            printf("CLOSE_CLIENT_SOCKET(%d)\n", idx);
            SocketThreads[i].connectionIdx = -1;
            closesocket(SocketConnections[idx].sock);
            SocketConnections[idx].sock = 0;
            SuspendThread(SocketThreads[i].hThread);
            break;
        }
    }
}


void CLEAR_SOCKET_CLIENTS() {
    long now = getMillisecond();
    for(int i=0; i < _MAX_SOCKET_CONNECTIONS; i++) {
        if(SocketConnections[i].sock > 0 && SocketConnections[i].lastActivity + _WAIT_SOCKET_ACTIVITY_SECONDS * 1000 < now) {
            printf("CLOSE_CLIENT_SOCKET.NEED(%d)\n", i);
            CLOSE_CLIENT_SOCKET(i);
        }
    }
}



DWORD WINAPI  CLEAR_SOCKET_CLIENTS_2(LPVOID data) {
    char buff[1];
    buff[0]='.';
    while (1) {
        Sleep(500);
        for (int i = 0; i < _MAX_SOCKET_CONNECTIONS; i++) {
            if (SocketConnections[i].sock > 0 && (SocketConnections[i].lastActivity + _WAIT_SOCKET_ACTIVITY_SECONDS * 1000 < getMillisecond())) {
                if (send(SocketConnections[i].sock, buff, 0, 0) == -1) CLOSE_CLIENT_SOCKET(i);
            }
        }
    }
}


DWORD WINAPI DO_CLIENT_WAIT(LPVOID data) {
    PT_SOCKET_SERVER ptSocketServer = (PT_SOCKET_SERVER) data;

    SOCKET client_socket;
    sockaddr_in client_addr;
    int client_addr_size=sizeof(client_addr);

    while((client_socket=accept(ptSocketServer->sock, (sockaddr *) &client_addr, &client_addr_size))!=-1) {
        printf("client_socket = %d\n", client_socket);
//        CLEAR_SOCKET_CLIENTS();


        HOSTENT *hst;
        hst = gethostbyaddr((char *) &client_addr.sin_addr.s_addr,4, AF_INET);
        printf("Host name: %s, type=%d, socket = %d \n", hst->h_name, hst->h_addrtype, client_socket);


        int clientIdx = -1;
        for(int i=0; i < _MAX_SOCKET_CONNECTIONS; i++) {
            if(SocketConnections[i].sock == 0) {
                clientIdx=i;
                printf("find client socket %d : %d\n", i, SocketConnections[i].sock);
                break;
            }
        }

        if(clientIdx==-1) {
            printf("No socket clients  space for %d\n", ptSocketServer->port);
            closesocket(client_socket);
            continue;
        }

        SocketConnections[clientIdx].sock = client_socket;
        SocketConnections[clientIdx].addr = client_addr;
        SocketConnections[clientIdx].pServer = ptSocketServer;
        SocketConnections[clientIdx].lastActivity = getMillisecond();

        int threadIdx = -1;
        for(int i=0;i<_MAX_SOCKET_THREADS;i++) {
            if(SocketThreads[i].connectionIdx == -1) {
                threadIdx = i;
                break;
            }
        }

        if(threadIdx==-1) {
            printf("No socket thread space for %d\n", ptSocketServer->port);
            closesocket(client_socket);
            continue;
        }

        printf("find Thread %d : %d\n", threadIdx, SocketConnections[clientIdx].sock);

        SocketThreads[threadIdx].connectionIdx = clientIdx;
        SocketThreads[threadIdx].number++;
        ResumeThread(SocketThreads[threadIdx].hThread);
    }

    closesocket(ptSocketServer->sock);
    ptSocketServer->sock = 0;
    ptSocketServer->port = 0;
    return 0;
}

DWORD WINAPI DO_CLIENT_THREAD(LPVOID data) {


    int number = 0;
    PT_SOCKET_THREAD ptSocketThread = (PT_SOCKET_THREAD) data;

    printf("DO_CLIENT_THREAD: %d\n", ptSocketThread->connectionIdx);

    PT_SOCKET_CONNECTION ptSocketClient;
    int summaryBytes = 0;
    char * answer1000;
//    int delayMs = 1;

    while (true) {
        if (ptSocketThread->connectionIdx < 0) {
            printf("ZERO THREAD CLIENT, SLEEP 5 sec\n");
            continue;
        }

        if(number!=ptSocketThread->number) {
            printf("number(%d)!=ptSocketThread->number(%d)\n", number, ptSocketThread->number);
            number = ptSocketThread->number;
        }


        ptSocketClient = &SocketConnections[ptSocketThread->connectionIdx];

        int packSize;
        int recvCount = 0;
        int recvTotal = -1;

        if(recv(ptSocketClient->sock, (char*)&packSize, sizeof(packSize), 0)>0) {
            if(ptSocketClient->size_buff<packSize) {
                ptSocketClient->buff = (char *) realloc(ptSocketClient->buff, packSize);
                if(ptSocketClient->buff== nullptr) {
                    printf("Не могу выделить память %d", packSize);
                    exit(1);
                }

                while ( (recvCount= recv(ptSocketClient->sock, ptSocketClient->buff + recvTotal, packSize-recvTotal , 0))>0 ) {
                    recvTotal+=recvCount;
                }

                if(recvTotal!=packSize) {
                    printf("Ошибка передачи данных %d!=%d", recvTotal, packSize);
                    exit(1);
                }
            }
        }


        if(recvTotal<=0) {
            printf("Closesocket for client #%d\n", ptSocketThread->connectionIdx);
            closesocket(ptSocketClient->sock);
            ptSocketClient->sock = 0;
            printf("SuspendThread for client #%d\n", ptSocketThread->connectionIdx);
            printf("Receive total %d bytes\n", summaryBytes);

            ptSocketThread->connectionIdx = -1;
            SuspendThread(ptSocketThread->hThread);

//            break;
        } else {
            answer1000 = ptSocketClient->pServer->callback(ptSocketClient->buff, recvTotal, ptSocketThread->connectionIdx);
            if(answer1000!= nullptr) {
                int ansSize = 0;
                for(int i=0;i<1024;i++) if(answer1000[i]==0) { ansSize = i; break; }
                send(ptSocketClient->sock, answer1000, ansSize, 0);
            }
        }

    }



    return 0;
}


void INIT_SERVER_SOCKET(PT_SOCKET_SERVER ptSocketServer) {

    sockaddr_in addr_in;

    addr_in.sin_family=AF_INET;
    addr_in.sin_port=htons(ptSocketServer->port);
    addr_in.sin_addr.s_addr=0;

    ptSocketServer->sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ptSocketServer->sock<0) {
        printf("Error Socket_Server.socket %d\n", WSAGetLastError());
        WSACleanup();
        exit(-1);
    }

    if (bind(ptSocketServer->sock, (sockaddr *) &addr_in, sizeof(addr_in))) {
        printf("Error bind (%d) %d\n", ptSocketServer->port, WSAGetLastError());
        closesocket(ptSocketServer->sock);  // закрываем сокет!
        exit(-1);
    }

    if (listen(ptSocketServer->sock, 0x100)) {
        printf("Error listen %d\n",WSAGetLastError());
        closesocket(ptSocketServer->sock);
        exit(-1);
    }

}

void INIT_SERVER_ALL() {
    HANDLE hThread;
    INIT_SOCKET_LIB();

    for(int i=0;i<_MAX_SOCKET_SERVICES;i++) SocketServices[i].port = 0;

    for(int i=0;i<_MAX_SOCKET_CONNECTIONS;i++) {
        SocketConnections[i].size_buff = _SIZE_BUFFER_START;
        SocketConnections[i].buff = (char*) malloc(_SIZE_BUFFER_START);
        SocketConnections[i].size = -1;
    }

    for(int i=0;i<_MAX_SOCKET_THREADS;i++) {
        SocketThreads[i].connectionIdx=-1;
        hThread = CreateThread(nullptr, 0, &DO_CLIENT_THREAD, &SocketThreads[i] , CREATE_SUSPENDED, nullptr);
        SocketThreads[i].hThread = hThread;
        SocketThreads[i].number = 0;

    }

    CreateThread(nullptr, 0, &CLEAR_SOCKET_CLIENTS_2, nullptr, 0, nullptr);

}


void ADD_SOCKET_SERVER(unsigned short port, T_CALLBACK_SOCKET_DATA callback) {
    int idx = -1;
    for(int i=0;i<_MAX_SOCKET_SERVICES;i++) {
        if(SocketServices[i].port==0) {
            idx = i;
            break;
        }
    }

    if(idx==-1) {
        printf("Error create service %d", port);
        exit(-1);
    }

    SocketServices[idx].port = port;
    SocketServices[idx].callback = callback;

    INIT_SERVER_SOCKET(&SocketServices[idx]);
    CreateThread(nullptr, 0, &DO_CLIENT_WAIT, &SocketServices[idx], 0, nullptr);
}

void SEND_TO_CLIENT(int clientIdx, char * message, int len) {
    send(SocketConnections[clientIdx].sock, message, len, 0);
}