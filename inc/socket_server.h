//
// Created by Andrey on 12.03.2021.
//

#ifndef UNOD2_SOCKET_SERVER_H
#define UNOD2_SOCKET_SERVER_H

#include <winsock2.h>
#include <stdio.h>

#define _MAX_SOCKET_SERVICES 16
#define _MAX_SOCKET_CONNECTIONS 256
#define _MAX_SOCKET_THREADS 256
#define _WAIT_SOCKET_ACTIVITY_SECONDS 1
#define _SIZE_BUFFER_START 4096


typedef char * (* T_CALLBACK_SOCKET_DATA)(char * data, int size, int clientIdx);

char * ReceiveData (char * data, int size, int clientIdx);
char * ReceiveData2 (char * data, int size, int connectionIdx);
char * ReceiveNat (char * data, int size, int connectionIdx);
long getMillisecond();

typedef struct {
    unsigned short port;
    T_CALLBACK_SOCKET_DATA callback;
    SOCKET sock;
} T_SOCKET_SERVER, * PT_SOCKET_SERVER;



typedef struct {
    int number;
    int connectionIdx;
    HANDLE hThread;
} T_SOCKET_THREAD, * PT_SOCKET_THREAD;

typedef struct {
    SOCKET sock;
    sockaddr_in addr;
    long lastActivity;
    PT_SOCKET_SERVER pServer;

    int size;
    int size_buff;
    char * buff;

//    PT_SOCKET_THREAD pThread;
} T_SOCKET_CONNECTION, * PT_SOCKET_CONNECTION;


extern T_SOCKET_SERVER  SocketServices[_MAX_SOCKET_SERVICES];
extern T_SOCKET_CONNECTION  SocketConnections[_MAX_SOCKET_CONNECTIONS];
extern T_SOCKET_THREAD  SocketThreads[_MAX_SOCKET_THREADS];

void INIT_SERVER_ALL();

void ADD_SOCKET_SERVER(unsigned short port, T_CALLBACK_SOCKET_DATA callback);

void SEND_TO_CLIENT(int clientIdx, char * message, int len);

#endif //UNOD2_SOCKET_SERVER_H
