#ifndef OVERFLOW_H
#define OVERFLOW_H

#include <stdbool.h>
#include "registro.h"

/*
 * Cada nodo de overflow es un registro + puntero al siguiente nodo de la
 * MISMA cadena (encadenamiento por bloque ancla). -1 indica fin de cadena.
 * Esto es lo que permite insertar registros nuevos sin reescribir todo
 * datos.dat: cuando un bloque se llena, el excedente va aquí, encadenado
 * al bloque que le correspondía.
 */
typedef struct {
    Registro reg;
    int siguiente;   // posición (índice de nodo) del siguiente en overflow.dat, o -1
    bool activo;      // false si fue eliminado lógicamente
} NodoOverflow;

/* ============ Responsable: Persona C (módulo OVERFLOW) ============
 * Es independiente de datos.c e indice.c: solo maneja su propio archivo.
 * Quien integra (main.c) es quien decide CUÁNDO llamar a estas funciones
 * (p. ej. al insertar en un bloque que ya está lleno).
 */

bool overflow_crear(const char *ruta_archivo);   // crea/inicializa el archivo, vacío
bool overflow_abrir(const char *ruta_archivo);
void overflow_cerrar(void);

// Inserta `reg` en una posición libre de overflow.dat y la encadena
// a continuación de `ptr_cabeza` (-1 si la cadena aún no existe).
// Retorna la posición del nuevo nodo (para que quien llama actualice
// el puntero ptr_overflow del bloque ancla), o -1 si falla.
int overflow_insertar(Registro reg, int ptr_cabeza);

// Busca `clave` recorriendo la cadena que empieza en `ptr_cabeza`.
bool overflow_buscar(int ptr_cabeza, int clave, Registro *resultado);

// Elimina lógicamente (activo = false) el nodo con `clave` en la cadena.
bool overflow_eliminar(int ptr_cabeza, int clave);

// Recorre TODA la cadena que empieza en `ptr_cabeza` (no está ordenada,
// a diferencia del área de datos, así que no se puede cortar antes) y
// copia en `out` (hasta `capacidad` registros) los que tengan clave en
// [clave_min, clave_max]. Retorna la cantidad TOTAL de coincidencias
// encontradas, aunque no todas hayan cabido en `out`.
int overflow_buscar_rango(int ptr_cabeza, int clave_min, int clave_max,
                           Registro *out, int capacidad);

#endif
