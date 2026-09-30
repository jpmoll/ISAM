#ifndef DATOS_H
#define DATOS_H

#include <stdbool.h>
#include "registro.h"
#include "config.h"

/*
 * Un bloque = una página: la unidad de lectura/escritura del archivo de
 * datos. `regs` es un arreglo DINÁMICO (se reserva con
 * datos_reservar_bloque) porque cuántos registros caben por página
 * depende del tamaño real de la tabla activa (ver datos_factor_bloque).
 */
typedef struct {
    Registro *regs;      // arreglo reservado con datos_reservar_bloque()
    int n;                 // registros ocupados (n <= datos_factor_bloque())
    int ptr_overflow;      // -1 si no tiene cadena de overflow, o posición en overflow.dat
} Bloque;

/* ============ Responsable: Persona A (módulo DATOS) ============
 * Encargado de crear y mantener el archivo binario de datos de UNA tabla
 * (la carpeta/ruta se la pasa quien integra): la parte "estática" de la
 * ISAM (opción 1: todo en un .dat en disco).
 */

// Cuántos registros caben en UNA página, según la tabla activa
// (llamar DESPUÉS de registro_configurar_tam(), si no siempre da el mismo
// número cacheado). Se calcula como:
//   TAM_PAGINA / (sizeof(int) + registro_tam_actual())
// Esto es lo que hace que una tabla con filas chicas quepa más por página
// que una con filas grandes — ya NO es un número fijo como antes.
int datos_factor_bloque(void);

// Reserva bloque->regs con el tamaño correcto (datos_factor_bloque()
// elementos) y deja bloque->n = 0, bloque->ptr_overflow = -1.
// Llamar ANTES de datos_leer_bloque()/datos_escribir_bloque().
bool datos_reservar_bloque(Bloque *bloque);

// Libera bloque->regs. Llamar cuando ya no se use ese Bloque.
void datos_liberar_bloque(Bloque *bloque);

// Crea el archivo de datos en `ruta_archivo` a partir de un arreglo YA
// ORDENADO por clave, repartiéndolo en páginas de datos_factor_bloque()
// registros (carga inicial/estática).
bool datos_crear(const char *ruta_archivo, const Registro *regs_ordenados, int n_regs);

// Abre/cierra el archivo de datos para lectura y escritura.
bool datos_abrir(const char *ruta_archivo);
void datos_cerrar(void);

// Lee/escribe el bloque (página) número `num_bloque` (0-indexado).
// `bloque` debe venir ya reservado con datos_reservar_bloque().
bool datos_leer_bloque(int num_bloque, Bloque *bloque);
bool datos_escribir_bloque(int num_bloque, const Bloque *bloque);

// Cantidad total de páginas que existen actualmente en el archivo.
int datos_num_bloques(void);

// Busca `clave` SOLO dentro del bloque `num_bloque` (no sigue overflow;
// eso lo resuelve quien integra, usando el módulo overflow si hace falta).
bool datos_buscar_en_bloque(int num_bloque, int clave, Registro *resultado);

#endif
