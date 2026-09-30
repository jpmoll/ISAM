#ifndef INDICE_H
#define INDICE_H

#include <stdbool.h>

/*
 * Entrada del índice disperso: una entrada por CADA bloque de datos,
 * apuntando a la clave menor de ese bloque. Es el índice clásico de ISAM
 * (no denso, no hay una entrada por registro sino por bloque).
 */
typedef struct {
    int clave_min;   // clave del primer registro del bloque
    int num_bloque;  // número de bloque en datos.dat
} EntradaIndice;

/* ============ Responsable: Persona B (módulo INDICE) ============
 * Depende únicamente de poder leer bloques del módulo DATOS
 * (usa datos_leer_bloque / datos_num_bloques declaradas en datos.h).
 * Mientras tanto puede desarrollar con datos.c en modo stub / con datos
 * de prueba hardcodeados.
 */

// Construye el índice en memoria recorriendo TODOS los bloques ya creados
// en datos.dat (debe llamarse después de datos_crear()).
bool indice_construir(void);

// Persiste el índice en `ruta_archivo`, para no reconstruirlo cada vez
// que se abre el programa.
bool indice_guardar(const char *ruta_archivo);

// Carga el índice desde `ruta_archivo` a memoria.
bool indice_cargar(const char *ruta_archivo);

// Devuelve el número de bloque de datos donde DEBERÍA estar `clave`
// según el índice (búsqueda binaria sobre clave_min). Solo ubica el
// bloque candidato; no garantiza que la clave exista ahí.
int indice_buscar_bloque(int clave);

// Cantidad de entradas del índice actualmente en memoria.
int indice_num_entradas(void);

#endif
