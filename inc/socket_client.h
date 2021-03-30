//
// Created by Andrey on 13.03.2021.
//

#ifndef UNOD2_SOCKET_CLIENT_H
#define UNOD2_SOCKET_CLIENT_H

#include <winsock2.h>


#define _MAX_SOCKET_CLIENTS 256

typedef struct {
    char            * addr;
    unsigned short  port = 0;
    SOCKET          sock = 0;
    bool            connected = false;
    long lastRecv_ms;

} T_SOCKET_CLIENT, * PT_SOCKET_CLIENT;

// size = -1 - отправка не удалась
typedef char * (* T_ON_CLIENT_ANSWER)(char * data, int size);


void INIT_CLIENT_ALL();
int ADD_SOCKET_CLIENT(unsigned short port, char * addr);
int DEL_SOCKET_CLIENT(int clientIdx);
int SEND_CLIENT(int clientIdx, char * data, int size, T_ON_CLIENT_ANSWER on_answer);




#endif //UNOD2_SOCKET_CLIENT_H
