#include <stdio.h>
#include <windows.h>
#include <winsock.h>




int main()
{


    //INICIALIZO WINSOCK
    WSADATA WsaData;
    WORD  wVersionRequerida = MAKEWORD (2, 2);

    WSAStartup (wVersionRequerida, &WsaData);

    //CREO EL SOCKET
    SOCKET Sock;
    Sock = socket (AF_INET, SOCK_STREAM, 0);

    //ESPECIFICO LA DIRECCION EN LA IP: 127.0.0.1 PUERTO: 5000
    SOCKADDR_IN SERVER_ADRESS;
    memset (&SERVER_ADRESS, 0, sizeof (SERVER_ADRESS));
    SERVER_ADRESS.sin_family = AF_INET;
    SERVER_ADRESS.sin_addr.S_un.S_un_b.s_b1 = 127;
    SERVER_ADRESS.sin_addr.S_un.S_un_b.s_b2 = 0;
    SERVER_ADRESS.sin_addr.S_un.S_un_b.s_b3 = 0;
    SERVER_ADRESS.sin_addr.S_un.S_un_b.s_b4 = 1;
    SERVER_ADRESS.sin_port = htons(5000);


        //ME CONECTO A UN SOCKET LISTENER DE MI SERVIDOR
    int iResult = connect (Sock, (SOCKADDR*) &SERVER_ADRESS, sizeof(SERVER_ADRESS));
    if (iResult)
    {
        printf ("NO SE PUDO CONECTAR\n");
        return 1;
    }

    int option;
    int tam;
    char ans[1024] = {0};

    while (1) {
        printf("SELECCIONE UNA DE LAS SIGUIENTES OPCIONES:\n");
        printf("1. GENERAR USUARIO\n");
        printf("2. GENERAR CONTRASEÑA\n");
        printf("3. SALIR\n");
       do {
        ;
        // VERIFICO SI LA ENTRADA ES UN ENTERO
        if (scanf("%d", &option) == 1) {
            break;
        } else {
            printf("ENTRADA NO VALIDA, PORFAVOR INGRESE UN NUMERO\n");

            //LIMPIO EL BUFFER
            while (getchar() != '\n');
        }
    } while (1);




    switch (option) {
        case 1:
            printf("PROPORCIONE LA LONGITUD DE SU USUARIO [5-15]\n");
            break;
        case 2:
            printf("PROPORCIONE LA LONGITUD DEL PASSWORD [8-50]\n");
            break;
        case 3:
            printf("ADIOS...\n");
            return 0;
        default:
            printf("PORFAVOR, INGRESE UNA DE LAS OPCIONES PROPORCIONADAS\n");
            break;
    }


        scanf("%d", &tam);

        // ENVIO LA OPCION AL SERVIDOR
        send(Sock, (char*)&option, sizeof(option), 0);
        // ENVIO EL TAMAÑO DE MI CONTRASEÑA O USUARIO AL SERVIDOR
        send(Sock, (char*)&tam, sizeof(tam), 0);
        memset(ans, 0, sizeof(ans));
        // RECIBO LA RESPUESTA Y CHECKEO SI LA RECIBI CORRECTAMENTE
        int ansBytes = recv(Sock, ans, sizeof(ans) - 1, 0);
        if (ansBytes > 0){
            printf("RESPUESTA ------>: %s\n", ans);
        } else {
            printf("RESPUESTA ------>: ERROR AL RECIBIR.\n");
        }
    }

    //CIERRO EL SOCKET
    closesocket (Sock);

    //CIERRO LA CONEXION
    WSACleanup();

    return 0;
}
