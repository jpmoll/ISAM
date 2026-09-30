#ifndef REGISTRO_H
#define REGISTRO_H

#include "config.h"

/*
 * Registro GENÉRICO: el motor ISAM (datos.c, indice.c, overflow.c) solo
 * conoce esto. No sabe nada de columnas, tablas ni tipos reales.
 *
 * - `clave` es siempre un entero, siempre lo primero (es lo único que el
 *   motor necesita para ordenar, buscar e indexar).
 * - `datos` son los bytes crudos del resto de la fila, tal cual la
 *   tabla que estén usando. Cada tabla tiene su PROPIO struct real
 *   (por ejemplo Bus, Viaje, Persona...) que se convierte a/desde estos
 *   bytes con dos funciones: `<tabla>_a_registro` y `registro_a_<tabla>`.
 *   Vean tablas/bus.h como ejemplo.
 */
typedef struct {
    int clave;
    unsigned char datos[TAM_REGISTRO_MAX - sizeof(int)];
} Registro;

// Tamaño real (en bytes) que ocupa la tabla que se esté usando AHORA,
// dentro de `datos` (siempre <= TAM_REGISTRO_MAX - sizeof(int)).
// Se fija UNA vez al arrancar el programa, según la tabla elegida.
void registro_configurar_tam(int tam_bytes_datos);
int  registro_tam_actual(void);

#endif
