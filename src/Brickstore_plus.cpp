//============================================================================
// Name        : Brickstore_plus.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>


#define PORT 5000
#define BUFFER_SIZE 4096
void enviarYRecibir(SOCKET sock, const char *comando) {
    char respuesta[BUFFER_SIZE];

    send(sock, comando, strlen(comando), 0);

    memset(respuesta, 0, BUFFER_SIZE);

    int bytes = recv(sock, respuesta, BUFFER_SIZE - 1, 0);

    if (bytes > 0) {
        respuesta[bytes] = '\0';
        printf("%s\n", respuesta);
    } else {
        printf("Error recibiendo respuesta del servidor.\n");
    }
}

int loginAdmin(SOCKET sock) {
    char email[80];
    char password[30];
    char comando[200];
    char respuesta[4096];

    printf("\n--- LOGIN ADMINISTRADOR ---\n");

    printf("Email: ");
    scanf("%s", email);

    printf("Password: ");
    scanf("%s", password);

    sprintf(comando, "LOGIN;%s;%s", email, password);

    printf("[CLIENTE] Enviando: %s\n", comando);

    send(sock, comando, strlen(comando), 0);

    memset(respuesta, 0, sizeof(respuesta));

    int bytes = recv(sock, respuesta, sizeof(respuesta) - 1, 0);

    if (bytes > 0) {
        respuesta[bytes] = '\0';
        printf("[CLIENTE] Respuesta servidor: %s\n", respuesta);

        if (strncmp(respuesta, "OK", 2) == 0) {
            return 1;
        }
    }

    return 0;
}

void menuAdmin(SOCKET sock) {
    int opcion = 0;

    while (opcion != 7) {
        printf("\n--- MENU DE ADMINISTRACION ---\n");
        printf("1. Visualizar catalogo de productos\n");
        printf("2. Añadir nuevo producto\n");
        printf("3. Modificar producto Stock/Precio\n");
        printf("4. Eliminar producto\n");
        printf("5. Ver valoraciones de clientes\n");
        printf("6. Registrar nuevo administrador\n");
        printf("7. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        if (opcion == 1) {
            enviarYRecibir(sock, "LISTAR");
        }

        else if (opcion == 2) {
            int id, stock;
            float precio;
            char nombre[50];
            char comando[300];

            printf("ID producto: ");
            scanf("%d", &id);

            printf("Nombre: ");
            scanf(" %[^\n]", nombre);

            printf("Stock: ");
            scanf("%d", &stock);

            printf("Precio: ");
            scanf("%f", &precio);

            sprintf(comando, "ADD;%d;%s;%d;%.2f", id, nombre, stock, precio);

            enviarYRecibir(sock, comando);
        }

        else if (opcion == 3) {
            int id, stock;
            float precio;
            char comando[200];

            printf("ID producto: ");
            scanf("%d", &id);

            printf("Nuevo stock: ");
            scanf("%d", &stock);

            printf("Nuevo precio: ");
            scanf("%f", &precio);

            sprintf(comando, "UPDATE;%d;%d;%.2f", id, stock, precio);

            enviarYRecibir(sock, comando);
        }

        else if (opcion == 4) {
            int id;
            char comando[100];

            printf("ID producto a eliminar: ");
            scanf("%d", &id);

            sprintf(comando, "DELETE;%d", id);

            enviarYRecibir(sock, comando);
        }

        else if (opcion == 5) {
            enviarYRecibir(sock, "VALORACIONES");
        }

        else if (opcion == 6) {
            char nombre[50];
            char email[80];
            char password[30];
            char comando[300];

            printf("Nombre nuevo administrador: ");
            scanf(" %[^\n]", nombre);

            printf("Email: ");
            scanf("%s", email);

            printf("Password: ");
            scanf("%s", password);

            sprintf(comando, "REG_ADMIN;%s;%s;%s", nombre, email, password);

            enviarYRecibir(sock, comando);
        }

        else if (opcion == 7) {
            enviarYRecibir(sock, "SALIR");
            printf("Cerrando cliente...\n");
        }

        else {
            printf("Opcion no valida.\n");
        }
    }
}

int main() {
    setbuf(stdout, NULL);

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    int tipoUsuario;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Error conectando con el servidor. Codigo: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Conectado al servidor BrickStore.\n");

    printf("\n=== BRICKSTORE ===\n");
    printf("1. Administrador\n");
    printf("2. Usuario corriente\n");
    printf("Seleccione tipo de usuario: ");
    scanf("%d", &tipoUsuario);

    if (tipoUsuario == 1) {
        if (loginAdmin(sock)) {
            menuAdmin(sock);
        } else {
            printf("No se puede acceder al menu de administrador.\n");
        }
    }

    else if (tipoUsuario == 2) {
        printf("Parte de usuario corriente no implementada.\n");
    }

    else {
        printf("Opcion no valida.\n");
    }

    printf("Cerrando cliente...\n");

    closesocket(sock);
    WSACleanup();

    system("pause");
    return 0;
}
