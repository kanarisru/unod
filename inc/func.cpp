//
// Created by Andrey on 30.03.2021.
//

#include "func.h"


P_CHAR_DATA receive_from_socket_first_size(SOCKET sock) {
    int r;
    int size;
    char * data;
    P_CHAR_DATA res;

    r = recv(sock, (char*) &size, sizeof(size), 0);
    if(r==-1) return nullptr;


    data = (char *) malloc(size);
    r = recv(sock, data, size, 0);
    if(r==-1) {
        free(data);
        return nullptr;
    }
    return res;
}


int _bespoleznaya_function() {
    return -7;
}