#include <stdio.h>
#include "overflow.h"
#include "config.h"

/*
 * PERSONA C: implementar aquí.
 * Sugerencia de flujo:
 *   1. overflow_crear(): fopen(ARCHIVO_OVERFLOW, "wb"); fclose (archivo
 *      vacío al inicio; no necesita bloques predefinidos).
 *   2. overflow_abrir()/overflow_cerrar(): fopen/fclose en modo "r+b".
 *      Si no existe, crearlo primero con overflow_crear().
 *   3. overflow_insertar(): escribir un NodoOverflow al FINAL del archivo
 *      (fseek a SEEK_END), con siguiente = ptr_cabeza y activo = true.
 *      Retornar la posición (offset / sizeof(NodoOverflow)) donde quedó.
 *   4. overflow_buscar(): mientras ptr_cabeza != -1, leer el nodo en esa
 *      posición, comparar clave si activo == true, si no coincide avanzar
 *      con nodo.siguiente.
 *   5. overflow_eliminar(): igual que buscar, pero al encontrarlo, releer,
 *      poner activo = false y reescribir el nodo en su misma posición.
 */

static FILE *fp = NULL;

bool overflow_crear(const char *ruta_archivo) {
    // TODO: implementar
    (void)ruta_archivo;
    return false;
}

bool overflow_abrir(const char *ruta_archivo) {
    // TODO: implementar
    (void)ruta_archivo;
    return false;
}

void overflow_cerrar(void) {
    // TODO: implementar
    if (fp) {
        fclose(fp);
        fp = NULL;
    }
}

int overflow_insertar(Registro reg, int ptr_cabeza) {
    // TODO: implementar
    (void)reg;
    (void)ptr_cabeza;
    return -1;
}

bool overflow_buscar(int ptr_cabeza, int clave, Registro *resultado) {
    // TODO: implementar
    (void)ptr_cabeza;
    (void)clave;
    (void)resultado;
    return false;
}

bool overflow_eliminar(int ptr_cabeza, int clave) {
    // TODO: implementar
    (void)ptr_cabeza;
    (void)clave;
    return false;
}

int overflow_buscar_rango(int ptr_cabeza, int clave_min, int clave_max,
                           Registro *out, int capacidad) {
    // TODO: implementar (recorrer toda la cadena, sin poder cortar antes
    // porque no está ordenada; ver comentario en overflow.h)
    (void)ptr_cabeza;
    (void)clave_min;
    (void)clave_max;
    (void)out;
    (void)capacidad;
    return 0;
}
