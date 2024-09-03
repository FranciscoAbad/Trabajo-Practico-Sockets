#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

//ESTA FUNCION SE ENCARGA DE CREAR LA CONTRASEÑA
void createPassword(char* password, int tam)
{
    //CREO UN DICCIONARIO CON TODOS LOS CARACTERES ALFANUMERICOS
    char dictionary[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < tam; i++)
    {
        password[i] = dictionary[rand() % (sizeof(dictionary)-1)];
    }
}

//ESTA FUNCION SE ENCARGA DE CHECKEAR QUE LA CONTRASEÑA TENGA AL MENOS UNA MAYUSCULA, UNA MINUSCULA Y UN NUMERO
int containsNumberLowerAndUpper(char* password)
{
    int hasUpper=0;
    int hasLower=0;
    int hasNumber=0;

    for (int i=0; i<strlen(password); i++)
    {
        if(isdigit(password[i])) hasNumber=1;
        if(islower(password[i])) hasLower=1;
        if(isupper(password[i])) hasUpper=1;
    }
    return hasLower && hasUpper && hasNumber;
}

void createUsername(char *username, int tam) {

    //DOS DICCIONARIOS CON VOCALES Y CONSONANTES
    char vowels[] = "aeiou";
    char consonants[] = "bcdfghjklmnpqrstvwxyz";
    int numVowels = strlen(vowels);
    int numConsonants = strlen(consonants);

    //SE DECIDE DE MANERA ALEATORIA SI COMIENZA CON VOCAL O CON CONSONANTE
    int startsWithVowel = rand() % 2;

    for (int i = 0; i < tam; i++) {
        if ((i % 2 == 0 && startsWithVowel) || (i % 2 != 0 && !startsWithVowel)) {
            username[i] = vowels[rand() % numVowels];
        } else {
            username[i] = consonants[rand() % numConsonants];
        }
    }
}

int main()
{
    srand(time(0));

    //INICIALIZO WINSOCK
    WSADATA wsaData;
    WORD wVersionRequerida = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequerida, &wsaData) != 0)
    {
        printf("NO SE PUDO INICIALIZAR WINSOCK, ERROR---> %d\n", WSAGetLastError());
        return 1;
    }

    // CREO EL SOCKET
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET)
    {
        printf("NO SE PUDO CREAR EL SOCKET, ERROR---> %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //ESPECIFICO LA DIRECCION DEL SERVIDOR
    SOCKADDR_IN LOCAL_ADDRESS;
    memset(&LOCAL_ADDRESS, 0, sizeof(LOCAL_ADDRESS));
    LOCAL_ADDRESS.sin_family = AF_INET;
    LOCAL_ADDRESS.sin_port = htons(5000);

    // ASOCIO LA DIRECCION AL SOCKET
    if (bind(listenSock, (SOCKADDR*)&LOCAL_ADDRESS, sizeof(LOCAL_ADDRESS)) == SOCKET_ERROR)
    {
        printf("NO SE PUEDO ASOCIAR LA DIRECCIÓN%d\n", WSAGetLastError());
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    // COMIENZO A ESCUCHAR EN ESPERA DE CONEXIONES
    if (listen(listenSock, 5) == SOCKET_ERROR)
    {
        printf("NO SE PUDO INICIAR LA ESCUCHA%d\n", WSAGetLastError());
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    printf("ESPERANDO CONEXIONES...\n");

    // ESTE CICLO WHILE PERMITE QUE EL SERVIDOR PERMANEZCA EN UN ESTADO DE ESPERA CONSTANTE ACEPTANDO NUEVAS CONEXIONES CADA VEZ QUE TERMINA UNA
    while (1)
    {
        SOCKET sockConecction = accept(listenSock, NULL, NULL);
        if (sockConecction == INVALID_SOCKET)
        {
            printf("FALLO EN LA CONEXION, ERROR--->: %d\n", WSAGetLastError());
            closesocket(listenSock);
            WSACleanup();
            return 1;
        }

        printf("Cliente conectado.\n");

        //ESTE CICLO SE ENCARGA DE RECIBIR Y PROCESAR DE MANERA CONTINUA LAS SOLICITUDES DE EL CLIENTE
        while (1)
        {
            int option;
            unsigned tam;
            const char* optionError = "INGRESE UNA OPCION VALIDA";
            const char* sizeError = "LA LONGITUD INGRESADA ES INVALIDA";


            // RECIBO LA OPCION ESCOGIDA
            int bytesRecibidos = recv(sockConecction, (char*)&option, sizeof(option), 0);
            if (bytesRecibidos <= 0)
            {
                printf("NO SE PUDO RECIBIR LA OPCIÓN\n");
                closesocket(sockConecction);
                break; // Salir del bucle de manejo del cliente
            }

            // RECIBO EL TAMAÑO ESCOGIDO
            bytesRecibidos = recv(sockConecction, (char*)&tam, sizeof(tam), 0);
            if (bytesRecibidos <= 0)
            {
                printf("NO SE PUDO RECIBIR EL TAMAÑO\n");
                closesocket(sockConecction);
                break;
            }

            switch (option)
            {
            case 1:
                if (tam >= 5 && tam <= 15)
                {

                    char user[15];
                    memset(user, 0, sizeof(user));
                    createUsername(user, tam);
                    //ENVIO EL USUARIO
                    send(sockConecction, user, strlen(user), 0);
                }
                else {
                    //ENVIO EL MENSAJE DE ERROR DE VALIDACION
                    send(sockConecction, sizeError, strlen(sizeError), 0);
                }
                break;
            case 2:
                if (tam >= 8 && tam <= 50)
                {

                    char password[50];
                    memset(password, 0, sizeof(password));
                    // HASTA QUE LA CONTRASEÑA CREADA NO SEA VALIDADA (QUE CONTENGA M,m,0) SE VOLVERA A GENERAR
                    do
                    {
                        createPassword(password, tam);

                    }
                    while(containsNumberLowerAndUpper(password)!=1);
                    //ENVIO LA CONTRASEÑA
                    send(sockConecction, password, strlen(password), 0);
                }
                else
                {
                    //ENVIO EL MENSAJE DE ERROR DE VALIDACION
                    send(sockConecction, sizeError, strlen(sizeError), 0);
                }
                break;
            default:
                send(sockConecction, optionError, strlen(optionError), 0);
                break;
            }
        }
        closesocket(sockConecction);
        printf("CLIENTE DESCONECTADO\n");
    }


    WSACleanup();

    return 0;
}
