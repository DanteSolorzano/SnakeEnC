#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include "snake.h"

int main() {
    struct termios oldt;
    Usuario usuario; // Crear la estructura del usuario

    // Mostrar bienvenida y capturar nombre
    limpiar_pantalla();
    mostrar_bienvenida(&usuario);

    int salir = 0; // Variable para controlar si el programa debe salir

    while (!salir) {
        // Mostrar menú y capturar opción
        int opcion = mostrar_menu();

        if (opcion == 1) {
            // Reinicia el juego para actualizar valores de una nueva paritda
            reiniciar_estado_juego();
            // Iniciar el juego
            srand(time(NULL));
            configurar_terminal(&oldt);
            ocultar_cursor();

            int quit = 0;   // Controla cuándo el usuario sale de la partida
            int gameover = 0; // Controla cuándo termina el juego actual

            while (!quit && !gameover) {
                dibujar_tabla();
                mover_cursor_arriba();

                x[head] = COLS / 2;
                y[head] = ROWS / 2;

                while (!quit && !gameover) {
                    if (applex < 0) crear_manzana();
                    manejar_entrada(&quit);
                    iniciar_snake(&gameover);
                    usleep(5 * 1000000 / 45);
                }

                if (gameover) {
                    usuario.puntaje = score;
                    guardar_puntaje(&usuario);
                    mostrar_mensaje_fin_juego();
                }
            }

            mostrar_cursor();
            restaurar_terminal(&oldt);

        } else if (opcion == 2) {

            mostrar_puntajes();
        } else if (opcion == 3) {
            // Salir del programa
            salir = 1;
            printf("Gracias por jugar, %s. ¡Hasta la próxima!\n", usuario.nombre);
            limpiar_pantalla();
        } else if(opcion == 4){
            //Mostrar creditos 
            mostrar_instrucciones();
        } else if(opcion == 5){
            //Mostrar creditos 
            mostrar_creditos();
        }
    }

    return 0;
}