#ifndef DATOS_H
#define DATOS_H

#include <stdbool.h>
#include "registro.h"
#include "config.h"

/* ============================================================
 * DATA AREA — Area primaria del ISAM
 *
 * Responsabilidad de ESTE componente (los otros dos los hacen tus
 * companeros):
 *   - Guardar los registros en bloques de tamaño fijo, ordenados
 *     por clave dentro de cada bloque y entre bloques.
 *   - Construir el area desde una lista de registros (carga masiva).
 *   - Dar acceso de lectura/escritura a un bloque por su numero,
 *     para que el componente de INDICE pueda recorrerla al construir
 *     su propio indice, y para que el area de OVERFLOW sepa a que
 *     bloque pertenece un puntero cuando alguien inserta despues.
 *
 * Lo que NO hace este componente (y es importante que lo sepan
 * los tres, para no duplicar trabajo):
 *   - No sabe nada de busqueda por clave ni de indices — eso lo
 *     decide el componente de indice, que usa df_leer_bloque() para
 *     ir a buscar el bloque que el decida.
 *   - No sabe leer la cadena de overflow — solo GUARDA el puntero
 *     (el offset) al primer registro de esa cadena, por bloque.
 *     Seguir la cadena es trabajo del componente de overflow.
 *
 * Layout de bloque en disco:
 *   cantidad          int32_t   4 bytes  (cuantos registros hay, <= ISAM_REGS_POR_BLOQUE)
 *   puntero_overflow  int64_t   8 bytes  (offset del primer registro de
 *                                         overflow de ESTE bloque, o
 *                                         ISAM_PUNTERO_NULO si no tiene)
 *   registros         ISAM_REGS_POR_BLOQUE * registro_bytes()
 * ============================================================ */

typedef struct DataArea DataArea;

/* Abre el archivo (lo crea si no existe). */
DataArea *da_abrir(const char *ruta);
void da_cerrar(DataArea *da);

/* Vacia el archivo por completo. Se usa antes de una carga masiva
 * o de una reorganizacion (cuando se reescribe todo desde cero). */
bool da_truncar(DataArea *da);

/* Tamaño en bytes de un bloque completo (cabecera + registros). */
size_t da_bloque_bytes(void);

/* Cuantos bloques hay actualmente en el archivo. */
long da_contar_bloques(DataArea *da);

/*
 * CONSTRUCCION (carga masiva).
 * Recibe 'registros' YA ORDENADOS por clave (el area de datos no
 * ordena por ustedes — eso puede hacerlo quien llama, con qsort,
 * antes de pasar el arreglo aca). Los reparte en bloques de a
 * ISAM_FACTOR_CARGA registros (deja espacio libre en cada bloque
 * a proposito, para que las primeras inserciones no vayan directo
 * a overflow). Trunca el archivo antes de escribir.
 */
bool da_construir(DataArea *da, const IsamRegistro *registros_ordenados, size_t n);

/*
 * Lee el bloque 'numero_bloque' completo: cuantos registros tiene,
 * cuales son, y el puntero a su cadena de overflow (que ESTE
 * componente no sabe interpretar, solo lo devuelve tal cual esta
 * guardado).
 *
 * 'regs_out' debe tener espacio para ISAM_REGS_POR_BLOQUE registros.
 */
bool da_leer_bloque(DataArea *da, long numero_bloque,
                     IsamRegistro *regs_out, int *cantidad_out,
                     int64_t *puntero_overflow_out);

/*
 * Reescribe el bloque 'numero_bloque' completo. Quien llama es
 * responsable de que 'regs' ya este ordenado por clave y de pasar
 * el puntero de overflow correcto (si no cambia, hay que volver a
 * pasar el mismo valor que devolvio da_leer_bloque(), si no se
 * pierde la cadena existente).
 */
bool da_escribir_bloque(DataArea *da, long numero_bloque,
                         const IsamRegistro *regs, int cantidad,
                         int64_t puntero_overflow);

/* Instrumentacion para el benchmark (accesos a disco = lecturas + escrituras). */
long da_accesos(DataArea *da);
void da_reiniciar_accesos(DataArea *da);

#endif
