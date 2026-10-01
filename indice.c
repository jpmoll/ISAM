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
    int total_bloques = datos_num_bloques();
    if (total_bloques <= 0) return false;

    free(indice);
    indice = NULL;
    n_entradas = 0;

    indice = malloc(sizeof(EntradaIndice) * (size_t)total_bloques);
    if (indice == NULL) return false;

    Bloque bloque;
    if (!datos_reservar_bloque(&bloque)) {
        free(indice);
        indice = NULL;
        return false;
    }

    for (int i = 0; i < total_bloques; i++) {
        if (!datos_leer_bloque(i, &bloque)) {
            datos_liberar_bloque(&bloque);
            free(indice);
            indice = NULL;
            n_entradas = 0;
            return false;
        }

        // Un bloque vacío no tiene regs[0] válido: se omite
        if (bloque.n == 0) continue;

        indice[n_entradas].clave_min  = bloque.regs[0].clave;
        indice[n_entradas].num_bloque = i;
        n_entradas++;
    }

    datos_liberar_bloque(&bloque);
    return n_entradas > 0;
}

bool indice_guardar(const char *ruta_archivo) {
    if (indice == NULL || n_entradas <= 0) return false;

    FILE *f = fopen(ruta_archivo, "wb");
    if (f == NULL) return false;

    // 1) Primero la cantidad de entradas
    if (fwrite(&n_entradas, sizeof(int), 1, f) != 1) {
        fclose(f);
        return false;
    }

    // 2) Luego el arreglo completo
    if (fwrite(indice, sizeof(EntradaIndice), (size_t)n_entradas, f) != (size_t)n_entradas) {
        fclose(f);
        return false;
    }

    return fclose(f) == 0;
}

bool indice_cargar(const char *ruta_archivo) {
    FILE *f = fopen(ruta_archivo, "rb");
    if (f == NULL) return false;

    int n;
    if (fread(&n, sizeof(int), 1, f) != 1 || n <= 0) {
        fclose(f);
        return false;
    }

    EntradaIndice *tmp = malloc(sizeof(EntradaIndice) * (size_t)n);
    if (tmp == NULL) {
        fclose(f);
        return false;
    }

    if (fread(tmp, sizeof(EntradaIndice), (size_t)n, f) != (size_t)n) {
        free(tmp);
        fclose(f);
        return false;
    }
    fclose(f);

    free(indice);       // descartar un índice anterior si lo había
    indice = tmp;
    n_entradas = n;
    return true;
}

int indice_buscar_bloque(int clave) {
    if (indice == NULL || n_entradas <= 0) return -1;

    int lo = 0;
    int hi = n_entradas - 1;
    int resultado = -1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        if (indice[mid].clave_min <= clave) {
            resultado = mid;
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    if (resultado == -1) return -1;
    return indice[resultado].num_bloque;
}

int indice_num_entradas(void) {
    return n_entradas;
}

