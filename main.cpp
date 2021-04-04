#include <iostream>
#include <conio.h>
#include "inc/socket_server.h"
#include "inc/socket_client.h"

DWORD a;


/**
 * Ветка куда что то вставляем, ешё что то добавляем
 * @return
 */


/**
 * А тут мы напишем что то для DEV ветки
 * @return
 */

int main() {
    char command[100];

    std::cout << "'q' - quit, '?' - help" << std::endl;

    INIT_SERVER_ALL();
    INIT_CLIENT_ALL();

//    ADD_SOCKET_SERVER(6660, &ReceiveData);
//    ADD_SOCKET_SERVER(6661, &ReceiveData);
    ADD_SOCKET_SERVER(7070, &ReceiveNat);

    int cmdNo = 0;
    int port = 0;
    char addr[100];
    char cmd = 0;
    while (true) {
        printf("> ", ++cmdNo);
//        scanf("%s", &command);
        Sleep(100);

//        while (!kbhit()) Sleep(150);
        cmd = _getch();
        printf("%c", cmd);

        if(cmd=='q') break;
        switch (cmd) {
            case '?':
                printf("\t ------ HELP\n");
                printf("q - exit\ns [port]- start server\nc [port] - connect to server\n? - help\n\n");
                break;
            case 's':
                printf("\t ------ CREATE SERVER\n");
                printf("port> ");
                scanf("%d",&port);
                printf("Create server, port %d\n", port);
                break;
            case 'c':
                printf("\t ------ CREATE CONNECTION\n");
                printf("Connect to server\n");
                printf("ip or address> ");
                scanf("%s",&addr);
                printf("port> ");
                scanf("%d",&port);
                printf("Connect to %s:%d\n", addr, port);
                break;
            default:
                printf("\n");
        }



    }
    return 0;
}
