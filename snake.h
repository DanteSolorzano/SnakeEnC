#ifndef SNAKE_H
#define SNAKE_H

#include <termios.h> // Para configurar el terminal

//Estructura de usuario
typedef struct {
    char nombre[50];
    int puntaje;
} Usuario;


// Constantes del juego
#define COLS 60
#define ROWS 30
#define MAX_LENGTH 1000

// Variables globales
extern int score; // Puntaje
extern int x[MAX_LENGTH], y[MAX_LENGTH]; // Coordenadas de la serpiente
extern int xdir, ydir; // Dirección de movimiento
extern int head, tail; // Cabeza y cola de la serpiente
extern int applex, appley; // Coordenadas de la manzana

// Declaración de funciones
//MENU
void mostrar_bienvenida(Usuario *usuario);
int mostrar_menu();
void mostrar_instrucciones();
void mostrar_creditos();
//JUEGO 
void configurar_terminal(struct termios *oldt);
void restaurar_terminal(const struct termios *oldt);
void ocultar_cursor();
void mostrar_cursor();
void dibujar_tabla();
void mover_cursor_arriba();
void crear_manzana();
void limpiar_pantalla();
void manejar_entrada(int *quit);
void mostrar_mensaje_fin_juego();
void iniciar_snake(int *gameover);
void reiniciar_estado_juego();
//PUNTAJES 
void guardar_puntaje(Usuario *usuario);
void mostrar_puntajes();





#endif // SNAKE_H