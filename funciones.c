#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include "snake.h"

// Variables globales
int score = 0;
int x[MAX_LENGTH], y[MAX_LENGTH];
int xdir = 1, ydir = 0;
int head = 0, tail = 0;
int applex = -1, appley;

//Funciones de la logica del juego y configuracion de terminal
void configurar_terminal(struct termios *oldt) {
    struct termios newt;
    tcgetattr(STDIN_FILENO, oldt);
    newt = *oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void restaurar_terminal(const struct termios *oldt) {
    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
}

void ocultar_cursor() {
    printf("\e[?25l");
}

void mostrar_cursor() {
    printf("\e[?25h");
}

void dibujar_tabla() {
    printf("┌");
    for (int i = 0; i < COLS; i++) printf("─");
    printf("┐\n");

    for (int j = 0; j < ROWS; j++) {
        printf("│");
        for (int i = 0; i < COLS; i++) printf("·");
        printf("│\n");
    }

    printf("└");
    for (int i = 0; i < COLS; i++) printf("─");
    printf("┘\n");
}

void mover_cursor_arriba() {
    printf("\e[%iA", ROWS + 2);
}

void crear_manzana() {
    do {
        applex = rand() % COLS;
        appley = rand() % ROWS;
    } while (x[tail] == applex && y[tail] == appley);

    printf("\e[%iB\e[%iC\033[31m❤\033[0m", appley + 1, applex + 1);
    printf("\e[%iF", appley + 1);
}

void limpiar_pantalla() {
    printf("\033[H\033[J"); // Mueve el cursor al inicio y limpia la pantalla
    fflush(stdout);         // Asegura que se aplique inmediatamente
}

void manejar_entrada(int *quit) {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    if (FD_ISSET(STDIN_FILENO, &fds)) {
        int ch = getchar();
        if (ch == 27 || ch == 'q') {
            *quit = 1;
        } else if (ch == 'a' && xdir != 1) {
            xdir = -1; ydir = 0;
        } else if (ch == 'd' && xdir != -1) {
            xdir = 1; ydir = 0;
        } else if (ch == 's' && ydir != -1) {
            xdir = 0; ydir = 1;
        } else if (ch == 'w' && ydir != 1) {
            xdir = 0; ydir = -1;
        }
    }
}

void mostrar_mensaje_fin_juego() {
    printf("\e[%iB\e[%iC Game Over! \n", ROWS / 2, COLS / 2 - 5);
    printf("\e[%iB\e[%iC Tu puntaje fue: %d \n", 1, COLS / 2 - 6, score);
    printf("\e[%iF", ROWS / 2 + 1);
    fflush(stdout);

    mostrar_cursor(); // Asegurarse de que el cursor sea visible
    sleep(5);         // Pausa para que el usuario pueda ver el mensaje

    // Limpiar la pantalla
    printf("\033[H\033[J"); // Limpia la terminal y mueve el cursor al inicio
    fflush(stdout);
}

void iniciar_snake(int *gameover) {
    int new_x = x[head] + xdir;
    int new_y = y[head] + ydir;

    if (new_x < 0 || new_x >= COLS || new_y < 0 || new_y >= ROWS) {
        *gameover = 1;
        return;
    }

    printf("\e[%iB\e[%iC·", y[tail] + 1, x[tail] + 1);
    printf("\e[%iF", y[tail] + 1);

    if (x[head] == applex && y[head] == appley) {
        applex = -1;
        printf("\a");
        score++;
    } else {
        tail = (tail + 1) % MAX_LENGTH;
    }

    head = (head + 1) % MAX_LENGTH;
    x[head] = new_x;
    y[head] = new_y;

    for (int i = tail; i != head; i = (i + 1) % MAX_LENGTH) {
        if (x[i] == x[head] && y[i] == y[head]) {
            *gameover = 1;
            return;
        }
    }

    printf("\e[%iB\e[%iC\033[32m▓\033[0m", y[head] + 1, x[head] + 1);
    printf("\e[%iF", y[head] + 1);
    fflush(stdout);
}

void reiniciar_estado_juego() {
    // Reinicia las posiciones de la serpiente
    for (int i = 0; i < MAX_LENGTH; i++) {
        x[i] = 0;
        y[i] = 0;
    }

    // Reinicia la dirección de la serpiente
    xdir = 1;
    ydir = 0;

    // Reinicia los índices de la cabeza y la cola
    head = 0;
    tail = 0;

    // Reinicia la posición de la manzana
    applex = -1;
    appley = -1;

    // Reinicia el puntaje
    score = 0;
}

//Funciones de menu

void mostrar_bienvenida(Usuario *usuario) {
    printf("**********************************************\n");
    printf("*     Bienvenido al juego SNAKE              *\n");
    printf("* Por favor introduce tu nombre de usuario   *\n");
    printf("* (el nombre no puede incluir espacios)      *\n");
    printf("**********************************************\n");
    printf("> ");
    scanf("%49s", usuario->nombre); // Leer nombre, máximo 49 caracteres
    usuario->puntaje = 0; // Inicializar puntaje en 0
    printf("\nHola, %s. ¡Prepárate para jugar Snake!\n\n", usuario->nombre);
}

int mostrar_menu() {
    int opcion;
    printf("*********************************************\n");
    printf("*                 MENÚ                      *\n");
    printf("* 1 - Iniciar juego                         *\n");
    printf("* 2 - Ver puntajes almacenados              *\n");
    printf("* 3 - Salir                                 *\n");
    printf("* 4 - ¿Cómo jugar?                          *\n");
    printf("* 5 - Creditos y agradecimientos            *\n");
    printf("*********************************************\n");
    printf("> ");
    scanf("%d", &opcion);
    while (opcion < 1 || opcion > 5) { // Validar entrada
        printf("Opción inválida. Presiona un número entre 1 y 5.\n");
        printf("> ");
        scanf("%d", &opcion);
    }
    return opcion;
}

//Funciones de puntaje
void guardar_puntaje(Usuario *usuario) {
    FILE *archivo = fopen("puntajes.txt", "a+");
    if (!archivo) {
        perror("No se pudo abrir el archivo de puntajes");
        return;
    }

    // Escribir el nuevo puntaje en el archivo
    fprintf(archivo, "%s %d\n", usuario->nombre, usuario->puntaje);
    fclose(archivo);

    // Leer todos los puntajes y ordenarlos
    Usuario usuarios[100];
    int cantidad = 0;
    archivo = fopen("puntajes.txt", "r");
    while (fscanf(archivo, "%49s %d", usuarios[cantidad].nombre, &usuarios[cantidad].puntaje) == 2) {
        cantidad++;
    }
    fclose(archivo);

    // Ordenar los puntajes de mayor a menor con bubble sort
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = i + 1; j < cantidad; j++) {
            if (usuarios[i].puntaje < usuarios[j].puntaje) {
                Usuario temp = usuarios[i];
                usuarios[i] = usuarios[j];
                usuarios[j] = temp;
            }
        }
    }

    // Guardar solo los primeros 10 puntajes
    archivo = fopen("puntajes.txt", "w");
    for (int i = 0; i < cantidad && i < 10; i++) {
        fprintf(archivo, "%s %d\n", usuarios[i].nombre, usuarios[i].puntaje);
    }
    fclose(archivo);
}

void mostrar_puntajes() {
    FILE *archivo = fopen("puntajes.txt", "r");
    if (!archivo) {
        printf("No hay puntajes almacenados.\n");
        return;
    }

    printf("*********************************************\n");
    printf("* INDICE  | USUARIO         | PUNTAJE       *\n");
    printf("*********************************************\n");

    Usuario usuario;
    int indice = 1;
    while (fscanf(archivo, "%49s %d", usuario.nombre, &usuario.puntaje) == 2) {
        printf("* %2d      | %-15s | %5d        *\n", indice, usuario.nombre, usuario.puntaje);
        indice++;
    }
    printf("*********************************************\n\n");

    fclose(archivo);
}

void mostrar_instrucciones() {
    printf("\n*********************************************\n");
    printf("*                CÓMO JUGAR                 *\n");
    printf("*********************************************\n");
    printf("* Usa las siguientes teclas para mover la   *\n");
    printf("* serpiente:                                *\n");
    printf("*                                           *\n");
    printf("*   W - Arriba                              *\n");
    printf("*   A - Izquierda                           *\n");
    printf("*   S - Abajo                               *\n");
    printf("*   D - Derecha                             *\n");
    printf("*                                           *\n");
    printf("* Objetivo:                                 *\n");
    printf("* Come tantas manzanas como puedas para     *\n");
    printf("* aumentar tu puntaje. Evita chocar contra  *\n");
    printf("* las paredes o contigo mismo.              *\n");
    printf("*                                           *\n");
    printf("* ¡Buena suerte!                            *\n");
    printf("*********************************************\n\n");
}

void mostrar_creditos() {
    printf("\n**********************************************\n");
    printf("*                CRÉDITOS                    *\n");
    printf("**********************************************\n");
    printf("* Este juego fue desarrollado por Dante      *\n");
    printf("* Solorzano Ferrer, para mostrar los         *\n");
    printf("* conocimientos obtenidos durante la clase   *\n");
    printf("* 'elementos de las ciencias computacionales'*\n");
    printf("*                                            *\n");
    printf("*                                            *\n");
    printf("* Agradecimientos a:                         *\n");
    printf("* - El profesor Alonso Ramirez Manzanares    *\n");
    printf("* - El tallerista David Antonio de la Rosa   *\n");
    printf("* - El tallerista Jonathan GArcia Pernia     *\n");
    printf("*                                            *\n");
    printf("*  Por: compartirnos de sus conocimientos    *\n");
    printf("*  y dedicar el tiempo al aprendizaje de     *\n");
    printf("*  todo el grupo.                            *\n");
    printf("*                                            *\n");
    printf("* ¡Gracias por jugar Snake!                  *\n");
    printf("*********************************************\n\n");
}