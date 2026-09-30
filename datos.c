#include <stdio.h>
#include <stdlib.h>
#include "datos.h"

/*
 * PERSONA A: implementar los TODO de abajo.
 *
 * `datos_factor_bloque`, `datos_reservar_bloque` y `datos_liberar_bloque`
 * ya están resueltas (dependen solo de registro_tam_actual(), no de cómo
 * decidan guardar el archivo), no hace falta tocarlas.
 *
 * Sugerencia de flujo para lo que falta:
 *   1. datos_crear(): fopen(ruta_archivo, "wb"), recorrer regs_ordenados
 *      de datos_factor_bloque() en datos_factor_bloque(), armar cada
 *      Bloque (datos_reservar_bloque + llenar) y escribirlo.
 *      OJO al escribir a disco: no usen sizeof(Registro) completo (eso
 *      escribiría TAM_REGISTRO_MAX bytes por registro, desperdiciando
 *      espacio); escriban solo
 *      `sizeof(int) + registro_tam_actual()` bytes por registro, que es
 *      lo que en verdad ocupa la tabla activa.
 *   2. datos_abrir()/datos_cerrar(): fopen/fclose en modo "r+b".
 *   3. datos_leer_bloque()/datos_escribir_bloque(): fseek al offset
 *      num_bloque * TAMAÑO_PAGINA_EN_DISCO (usando el tamaño real por
 *      registro del punto 1, más espacio para n y ptr_overflow) y luego
 *      leer/escribir esa página.
 *   4. datos_num_bloques(): tamaño del archivo (fseek a SEEK_END + ftell)
 *      dividido entre el tamaño real de una página en disco.
 *   5. datos_buscar_en_bloque(): datos_reservar_bloque + datos_leer_bloque
 *      y búsqueda binaria (o lineal) sobre sus `n` registros ocupados.
 */

static FILE *fp = NULL;
static int factor_bloque_cache = 0;

int datos_factor_bloque(void) {
    int tam_en_disco = (int)sizeof(int) + registro_tam_actual();
    factor_bloque_cache = TAM_PAGINA / tam_en_disco;
    if (factor_bloque_cache < 1) factor_bloque_cache = 1; // registro más grande que una página
    return factor_bloque_cache;
}

bool datos_reservar_bloque(Bloque *bloque) {
    int factor = datos_factor_bloque();
    bloque->regs = malloc(sizeof(Registro) * (size_t)factor);
    bloque->n = 0;
    bloque->ptr_overflow = -1;
    return bloque->regs != NULL;
}

void datos_liberar_bloque(Bloque *bloque) {
    free(bloque->regs);
    bloque->regs = NULL;
}

bool datos_crear(const char *ruta_archivo, const Registro *regs_ordenados, int n_regs) {
    // TODO: implementar
    (void)ruta_archivo;
    (void)regs_ordenados;
    (void)n_regs;
    return false;
}

bool datos_abrir(const char *ruta_archivo) {
    // TODO: implementar
    (void)ruta_archivo;
    return false;
}

void datos_cerrar(void) {
    // TODO: implementar
    if (fp) {
        fclose(fp);
        fp = NULL;
    }
}

bool datos_leer_bloque(int num_bloque, Bloque *bloque) {
    // TODO: implementar
    (void)num_bloque;
    (void)bloque;
    return false;
}

bool datos_escribir_bloque(int num_bloque, const Bloque *bloque) {
    // TODO: implementar
    (void)num_bloque;
    (void)bloque;
    return false;
}

int datos_num_bloques(void) {
    // TODO: implementar
    return 0;
}

bool datos_buscar_en_bloque(int num_bloque, int clave, Registro *resultado) {
    // TODO: implementar
    (void)num_bloque;
    (void)clave;
    (void)resultado;
    return false;
}
