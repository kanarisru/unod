//
// Created by Andrey on 30.03.2021.
//

#ifndef UNOD2_FUNC_H
#define UNOD2_FUNC_H

#include <winsock2.h>


typedef struct {
    int size;
    char * buff;
} T_CHAR_DATA, * P_CHAR_DATA;

P_CHAR_DATA receive_from_socket_first_size(SOCKET sock);


int _bespoleznaya_function();

#endif //UNOD2_FUNC_H
