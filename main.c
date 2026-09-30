#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "registro.h"
#include "datos.h"
#include "indice.h"
#include "overflow.h"
#include "tablas/coordenada.h"   // <-- tabla que van a usar; para otra tabla
                                  //     cambien este include y `Coordenada`
                                  //     más abajo por su struct/adaptador.

/*
 * INTEGRACIÓN (la hace el equipo junto, al final, cuando los 3 módulos
 * ya compilan). Estas funciones son las que se usan desde un menú o
 * desde las pruebas del profe: buscar / insertar / eliminar / rango.
 * No dependen de CÓMO cada módulo resuelve su archivo, solo de su .h.
 */

// Búsqueda: índice -> bloque candidato -> dentro del bloque -> overflow si hace falta.
bool isam_buscar(int clave, Registro *resultado) {
    int num_bloque = indice_buscar_bloque(clave);
    if (num_bloque < 0) return false;

    if (datos_buscar_en_bloque(num_bloque, clave, resultado)) {
        return true;
    }

    Bloque bloque;
    if (!datos_reservar_bloque(&bloque)) return false;
    bool leido = datos_leer_bloque(num_bloque, &bloque);
    int ptr_overflow = bloque.ptr_overflow;
    datos_liberar_bloque(&bloque);
    if (!leido || ptr_overflow == -1) return false;

    return overflow_buscar(ptr_overflow, clave, resultado);
}

// Inserción: ubicar bloque, si hay espacio libre en el bloque insertar ahí
// (manteniendo orden), si no, mandar a overflow encadenado a ese bloque.
bool isam_insertar(Registro nuevo) {
    int num_bloque = indice_buscar_bloque(nuevo.clave);
    if (num_bloque < 0) return false;

    Bloque bloque;
    if (!datos_reservar_bloque(&bloque)) return false;
    if (!datos_leer_bloque(num_bloque, &bloque)) {
        datos_liberar_bloque(&bloque);
        return false;
    }

    bool ok;
    if (bloque.n < datos_factor_bloque()) {
        // TODO (integración): insertar `nuevo` en bloque.regs manteniendo
        // el orden por clave, desplazando los mayores una posición,
        // incrementar bloque.n y llamar datos_escribir_bloque().
        ok = false;
    } else {
        int nueva_pos = overflow_insertar(nuevo, bloque.ptr_overflow);
        if (nueva_pos == -1) {
            ok = false;
        } else {
            bloque.ptr_overflow = nueva_pos;
            ok = datos_escribir_bloque(num_bloque, &bloque);
        }
    }

    datos_liberar_bloque(&bloque);
    return ok;
}

// Eliminación: intentar en el bloque, si no está buscar/eliminar en overflow.
bool isam_eliminar(int clave) {
    int num_bloque = indice_buscar_bloque(clave);
    if (num_bloque < 0) return false;

    Bloque bloque;
    if (!datos_reservar_bloque(&bloque)) return false;
    if (!datos_leer_bloque(num_bloque, &bloque)) {
        datos_liberar_bloque(&bloque);
        return false;
    }

    // TODO (integración): si la clave está en bloque.regs, eliminarla ahí
    // (desplazar registros, decrementar bloque.n, datos_escribir_bloque()).
    // Si no está, y bloque.ptr_overflow != -1, usar overflow_eliminar()
    // (ya lo hace el bloque de abajo).

    bool ok = false;
    if (bloque.ptr_overflow != -1) {
        ok = overflow_eliminar(bloque.ptr_overflow, clave);
    }
    datos_liberar_bloque(&bloque);
    return ok;
}

// Búsqueda por RANGO [clave_min, clave_max].
// Aprovecha que el área de datos está físicamente ordenada por clave:
// se ubica el bloque de arranque con el índice y se escanean bloques
// consecutivos, cortando en cuanto un bloque empiece más allá de clave_max.
// (El overflow de cada bloque hay que recorrerlo completo, sin poder
// cortar antes, porque no está ordenado — ver overflow.h).
int isam_buscar_rango(int clave_min, int clave_max, Registro *out, int capacidad) {
    int num_bloque = indice_buscar_bloque(clave_min);
    if (num_bloque < 0) num_bloque = 0; // clave_min menor que todas: arrancar desde el inicio

    int encontrados = 0;
    int total_bloques = datos_num_bloques();

    Bloque bloque;
    if (!datos_reservar_bloque(&bloque)) return 0;

    for (int b = num_bloque; b < total_bloques; b++) {
        if (!datos_leer_bloque(b, &bloque)) break;

        // El área de datos sí está ordenada: si el primer registro del
        // bloque ya supera clave_max, todo lo que sigue también, se corta.
        if (bloque.n > 0 && bloque.regs[0].clave > clave_max) break;

        for (int i = 0; i < bloque.n; i++) {
            if (bloque.regs[i].clave >= clave_min && bloque.regs[i].clave <= clave_max) {
                if (encontrados < capacidad) out[encontrados] = bloque.regs[i];
                encontrados++;
            }
        }

        if (bloque.ptr_overflow != -1) {
            encontrados += overflow_buscar_rango(bloque.ptr_overflow, clave_min, clave_max,
                                                  out + encontrados,
                                                  capacidad - encontrados);
        }
    }

    datos_liberar_bloque(&bloque);
    return encontrados;
}

int main(void) {
    // 1) Decirle al motor cuánto pesa la tabla que van a usar (esto es lo
    //    único que cambia si usan otra tabla en vez de `coordenadas`).
    registro_configurar_tam(sizeof(Coordenada));

    // 2) Cada tabla vive en su propia carpeta: data/<tabla>/{datos,indice,overflow}.dat
    //    (ISAM es una estructura por tabla, no un archivo con varias mezcladas).
    const char *carpeta_tabla = "data/coordenadas";
    mkdir("data", 0755);
    mkdir(carpeta_tabla, 0755);

    char ruta_datos[256], ruta_indice[256], ruta_overflow[256];
    snprintf(ruta_datos,    sizeof(ruta_datos),    "%s/%s", carpeta_tabla, ARCHIVO_DATOS);
    snprintf(ruta_indice,   sizeof(ruta_indice),   "%s/%s", carpeta_tabla, ARCHIVO_INDICE);
    snprintf(ruta_overflow, sizeof(ruta_overflow), "%s/%s", carpeta_tabla, ARCHIVO_OVERFLOW);

    // TODO (integración): cargar filas reales de `coordenadas` (ordenadas
    // por id_coordenada), convertirlas con coordenada_a_registro() a un
    // arreglo de Registro, y pasarlo a datos_crear(ruta_datos, regs, n).
    // Luego: datos_abrir(ruta_datos); indice_construir();
    // indice_guardar(ruta_indice); overflow_crear(ruta_overflow);
    // y probar isam_buscar_rango(...) con un arreglo reservado por el
    // llamador.

    printf("Esqueleto ISAM - integrar datos.c, indice.c y overflow.c\n");
    return 0;
}
