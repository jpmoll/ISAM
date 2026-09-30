#include <stdio.h>
#include <stdlib.h>
#include "indice.h"
#include "datos.h"
#include "config.h"

/*
 * PERSONA B: implementar aquí.
 * Sugerencia de flujo:
 *   1. indice_construir(): for (i = 0; i < datos_num_bloques(); i++)
 *      datos_leer_bloque(i, &bloque); guardar {bloque.regs[0].id_bus, i}
 *      en un arreglo en memoria (usar malloc/realloc o un arreglo estático
 *      con capacidad máxima razonable).
 *   2. indice_guardar()/indice_cargar(): fwrite/fread del arreglo completo
 *      de EntradaIndice a ARCHIVO_INDICE (guardar primero la cantidad de
 *      entradas, luego el arreglo).
 *   3. indice_buscar_bloque(): búsqueda binaria sobre el arreglo ordenado
 *      por clave_min, devolviendo el último num_bloque tal que
 *      clave_min <= clave buscada.
 */

static EntradaIndice *indice = NULL;
static int n_entradas = 0;

bool indice_construir(void) {
    // TODO: implementar
    return false;
}

bool indice_guardar(const char *ruta_archivo) {
    // TODO: implementar
    (void)ruta_archivo;
    return false;
}

bool indice_cargar(const char *ruta_archivo) {
    // TODO: implementar
    (void)ruta_archivo;
    return false;
}

int indice_buscar_bloque(int clave) {
    // TODO: implementar (búsqueda binaria)
    (void)clave;
    return -1;
}

int indice_num_entradas(void) {
    return n_entradas;
}
