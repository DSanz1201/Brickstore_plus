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
#include <vector>
#include <string>

struct ItemCarrito {
    int id_producto;
    int cantidad;
};

#define PORT 5000
#define BUFFER_SIZE 4096
void eliminarUnProductoCarrito(std::vector<ItemCarrito> &carrito) {
    if (carrito.empty()) {
        printf("El carrito esta vacio.\n");
        return;
    }

    int idEliminar;
    bool encontrado = false;

    printf("\n--- TU CARRITO ---\n");
    for (size_t i = 0; i < carrito.size(); i++) {
        printf("ID Producto: %d | Cantidad: %d\n",
               carrito[i].id_producto,
               carrito[i].cantidad);
    }

    printf("ID del producto que quieres eliminar: ");
    scanf("%d", &idEliminar);

    for (size_t i = 0; i < carrito.size(); i++) {
        if (carrito[i].id_producto == idEliminar) {
            carrito.erase(carrito.begin() + i);
            encontrado = true;
            printf("Producto eliminado del carrito.\n");
            break;
        }
    }

    if (!encontrado) {
        printf("Ese producto no esta en el carrito.\n");
    }
}

void vaciarCarrito(std::vector<ItemCarrito> &carrito) {
    if (carrito.empty()) {
        printf("El carrito ya esta vacio.\n");
        return;
    }

    carrito.clear();
    printf("Carrito vaciado correctamente.\n");
}
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

int registrarUsuario(SOCKET sock) {
    char nombre[50];
    char email[80];
    char password[30];
    char comando[300];

    printf("\n--- REGISTRO DE NUEVO USUARIO ---\n");
    printf("Nombre: ");
    scanf(" %[^\n]", nombre);
    printf("Email: ");
    scanf("%s", email);
    printf("Password: ");
    scanf("%s", password);

    // Siguiendo el formato de punto y coma ';' que usa tu servidor actual
    sprintf(comando, "REGISTER;%s;%s;%s", nombre, email, password);
    enviarYRecibir(sock, comando);
    return 1;
}

int loginUsuarioCorriente(SOCKET sock, int &idUsuario) {
    char email[80];
    char password[30];
    char comando[200];
    char respuesta[4096];

    printf("\n--- LOGIN USUARIO CORRIENTE ---\n");
    printf("Email: ");
    scanf("%s", email);
    printf("Password: ");
    scanf("%s", password);

    sprintf(comando, "LOGIN;%s;%s", email, password);
    send(sock, comando, strlen(comando), 0);

    memset(respuesta, 0, sizeof(respuesta));
    int bytes = recv(sock, respuesta, sizeof(respuesta) - 1, 0);

    if (bytes > 0) {
        respuesta[bytes] = '\0';
        printf("[CLIENTE] Respuesta servidor: %s\n", respuesta);

        if (strncmp(respuesta, "OK", 2) == 0) {
            sscanf(respuesta, "OK;USER;%d", &idUsuario);
            return 1;
        }
    }
    return 0;
}
int comprobarStockProducto(SOCKET sock, int idProducto, int cantidad) {
    char comando[100];
    char respuesta[BUFFER_SIZE];

    sprintf(comando, "CHECK_STOCK;%d;%d", idProducto, cantidad);

    send(sock, comando, strlen(comando), 0);

    memset(respuesta, 0, BUFFER_SIZE);

    int bytes = recv(sock, respuesta, BUFFER_SIZE - 1, 0);

    if (bytes > 0) {
        respuesta[bytes] = '\0';
        printf("%s\n", respuesta);

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
void menuUsuario(SOCKET sock, int idUsuario) {
    int opcion = 0;
    std::vector<ItemCarrito> carrito;

    while (opcion != 10) {
        printf("\n--- TIENDA LEGO: MENU DE USUARIO ---\n");
        printf("1. Ver catalogo de productos\n");
        printf("2. Ver detalle de un producto\n");
        printf("3. Añadir producto al carrito\n");
        printf("4. Eliminar un producto del carrito\n");
        printf("5. Vaciar carrito completo\n");
        printf("6. Ver carrito y confirmar pedido\n");
        printf("7. Consultar historial de pedidos\n");
        printf("8. Ver todas las valoraciones\n");
        printf("9. Dejar una valoracion\n");
        printf("10. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        if (opcion == 1) {
            enviarYRecibir(sock, "LISTAR");
        }

        else if (opcion == 2) {
            int id;
            char comando[100];

            printf("Introduce el ID del producto que deseas consultar: ");
            scanf("%d", &id);

            sprintf(comando, "GET_PRODUCT_DETAIL;%d", id);
            enviarYRecibir(sock, comando);
        }

        else if (opcion == 3) {
            int id, cant;

            printf("ID del producto a añadir: ");
            scanf("%d", &id);

            printf("Cantidad: ");
            scanf("%d", &cant);

            if (cant <= 0) {
                printf("La cantidad debe ser mayor que 0.\n");
            }
            else if (comprobarStockProducto(sock, id, cant)) {
                ItemCarrito item;
                item.id_producto = id;
                item.cantidad = cant;

                carrito.push_back(item);

                printf("Producto añadido al carrito local.\n");
            }
            else {
                printf("No se ha añadido el producto al carrito.\n");
            }
        }

        else if (opcion == 4) {
            eliminarUnProductoCarrito(carrito);
        }

        else if (opcion == 5) {
            vaciarCarrito(carrito);
        }

        else if (opcion == 6) {
            if (carrito.empty()) {
                printf("El carrito esta vacio.\n");
            } else {
                printf("\n--- TU CARRITO ---\n");

                float total = 0.0;

                for (size_t i = 0; i < carrito.size(); i++) {
                    char comando[100];
                    char respuesta[BUFFER_SIZE];

                    sprintf(comando, "GET_PRODUCT_DETAIL;%d", carrito[i].id_producto);

                    send(sock, comando, strlen(comando), 0);
                    memset(respuesta, 0, BUFFER_SIZE);
                    recv(sock, respuesta, BUFFER_SIZE - 1, 0);

                    float precio = 0.0;
                    char* ptr = strstr(respuesta, "Precio: ");

                    if (ptr != NULL) {
                        sscanf(ptr, "Precio: %f", &precio);
                    }

                    float subtotal = precio * carrito[i].cantidad;
                    total += subtotal;

                    printf("ID Producto: %d | Cantidad: %d | Precio ud: %.2f EUR | Subtotal: %.2f EUR\n",
                           carrito[i].id_producto,
                           carrito[i].cantidad,
                           precio,
                           subtotal);
                }

                printf("----------------------------------------------------------\n");
                printf("PRECIO TOTAL DEL PEDIDO: %.2f EUR\n", total);
                printf("----------------------------------------------------------\n");

                char confirmar;
                printf("Deseas confirmar el pedido? (s/n): ");
                scanf(" %c", &confirmar);

                if (confirmar == 's' || confirmar == 'S') {
                    char comando[1024];
                    char infoProductos[512] = "";

                    sprintf(comando, "CREATE_ORDER;%d;", idUsuario);

                    for (size_t i = 0; i < carrito.size(); i++) {
                        char temp[50];

                        sprintf(temp, "%d:%d",
                                carrito[i].id_producto,
                                carrito[i].cantidad);

                        strcat(infoProductos, temp);

                        if (i < carrito.size() - 1) {
                            strcat(infoProductos, ",");
                        }
                    }

                    strcat(comando, infoProductos);

                    enviarYRecibir(sock, comando);
                    carrito.clear();
                } else {
                    printf("Pedido cancelado.\n");
                }
            }
        }

        else if (opcion == 7) {
            char comando[100];

            sprintf(comando, "GET_ORDERS;%d", idUsuario);
            enviarYRecibir(sock, comando);
        }

        else if (opcion == 8) {
            enviarYRecibir(sock, "VALORACIONES");
        }

        else if (opcion == 9) {
            int id_prod, puntuacion;
            char comentario[200];
            char comando[512];

            printf("ID del producto a valorar: ");
            scanf("%d", &id_prod);

            printf("Puntuacion (1 a 5): ");
            scanf("%d", &puntuacion);

            printf("Comentario: ");
            scanf(" %[^\n]", comentario);

            sprintf(comando,
                    "ADD_VALORACION;%d;%d;%d;%s",
                    idUsuario,
                    id_prod,
                    puntuacion,
                    comentario);

            enviarYRecibir(sock, comando);
        }

        else if (opcion == 10) {
            enviarYRecibir(sock, "SALIR");
            printf("Cerrando sesión de usuario...\n");
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
        int opcionAuth = 0;
        int idUsuarioLogueado = -1;
        bool sesionActiva = false;

        while (!sesionActiva && opcionAuth != 3) {
            printf("\n--- MENU DE AUTENTICACION ---\n");
            printf("1. Iniciar Sesion\n");
            printf("2. Registrarse\n");
            printf("3. Salir\n");
            printf("Seleccione una opcion: ");
            scanf("%d", &opcionAuth);

            if (opcionAuth == 1) {
                if (loginUsuarioCorriente(sock, idUsuarioLogueado)) {
                    sesionActiva = true;
                    menuUsuario(sock, idUsuarioLogueado);
                } else {
                    printf("Login incorrecto. Por favor, intentalo de nuevo.\n");
                }
            }
            else if (opcionAuth == 2) {
                registrarUsuario(sock);
            }
            else if (opcionAuth == 3) {
                enviarYRecibir(sock, "SALIR");
            }
            else {
                printf("Opcion no valida.\n");
            }
        }
    }

    printf("Cerrando cliente...\n");

    closesocket(sock);
    WSACleanup();

    system("pause");
    return 0;
}
