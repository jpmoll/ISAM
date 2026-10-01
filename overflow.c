#include <stdio.h>
#include "overflow.h"
#include "config.h"

/*
 * Módulo OVERFLOW: archivo de nodos de tamaño fijo (sizeof(NodoOverflow)).
 * La "posición" de un nodo es su índice: offset en bytes = pos * sizeof(NodoOverflow).
 * Cada bloque de datos guarda la posición de la CABEZA de su cadena;
 * insertar agrega un nodo al final del archivo y lo pone como nueva cabeza
 * (inserción al frente, O(1)).
 */

static FILE *fp = NULL;

/* ---------- helpers internos ---------- */

static bool leer_nodo(int pos, NodoOverflow *nodo) {
    if (!fp || pos < 0) return false;
    long offset = (long)pos * (long)sizeof(NodoOverflow);
    if (fseek(fp, offset, SEEK_SET) != 0) return false;
    return fread(nodo, sizeof(NodoOverflow), 1, fp) == 1;
}

static bool escribir_nodo(int pos, const NodoOverflow *nodo) {
    if (!fp || pos < 0) return false;
    long offset = (long)pos * (long)sizeof(NodoOverflow);
    if (fseek(fp, offset, SEEK_SET) != 0) return false;
    if (fwrite(nodo, sizeof(NodoOverflow), 1, fp) != 1) return false;
    return fflush(fp) == 0;
}

/* ---------- API pública ---------- */

bool overflow_crear(const char *ruta_archivo) {
    if (!ruta_archivo) return false;
    FILE *f = fopen(ruta_archivo, "wb");   // trunca: archivo vacío
    if (!f) return false;
    fclose(f);
    return true;
}

bool overflow_abrir(const char *ruta_archivo) {
    if (!ruta_archivo) return false;
    if (fp) overflow_cerrar();

    fp = fopen(ruta_archivo, "r+b");
    if (!fp) {                              // no existe: crearlo y reabrir
        if (!overflow_crear(ruta_archivo)) return false;
        fp = fopen(ruta_archivo, "r+b");
    }
    return fp != NULL;
}

void overflow_cerrar(void) {
    if (fp) {
        fclose(fp);
        fp = NULL;
    }
}

int overflow_insertar(Registro reg, int ptr_cabeza) {
    if (!fp) return -1;
    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    long fin = ftell(fp);
    if (fin < 0) return -1;

    int pos = (int)(fin / (long)sizeof(NodoOverflow));

    NodoOverflow nodo;
    nodo.reg = reg;
    nodo.siguiente = ptr_cabeza;   // el nodo nuevo apunta a la antigua cabeza
    nodo.activo = true;

    if (!escribir_nodo(pos, &nodo)) return -1;
    return pos;                    // nueva cabeza para el bloque ancla
}

bool overflow_buscar(int ptr_cabeza, int clave, Registro *resultado) {
    NodoOverflow nodo;
    int pos = ptr_cabeza;
    while (pos != -1) {
        if (!leer_nodo(pos, &nodo)) return false;
        if (nodo.activo && nodo.reg.clave == clave) {
            if (resultado) *resultado = nodo.reg;
            return true;
        }
        pos = nodo.siguiente;
    }
    return false;
}

bool overflow_eliminar(int ptr_cabeza, int clave) {
    NodoOverflow nodo;
    int pos = ptr_cabeza;
    while (pos != -1) {
        if (!leer_nodo(pos, &nodo)) return false;
        if (nodo.activo && nodo.reg.clave == clave) {
            nodo.activo = false;               // eliminación lógica
            return escribir_nodo(pos, &nodo);
        }
        pos = nodo.siguiente;
    }
    return false;
}

int overflow_buscar_rango(int ptr_cabeza, int clave_min, int clave_max,
                           Registro *out, int capacidad) {
    NodoOverflow nodo;
    int pos = ptr_cabeza;
    int total = 0;      // coincidencias encontradas (aunque no quepan)
    int copiados = 0;   // las que sí se copiaron a out

    while (pos != -1) {
        if (!leer_nodo(pos, &nodo)) break;
        if (nodo.activo && nodo.reg.clave >= clave_min && nodo.reg.clave <= clave_max) {
            if (out && copiados < capacidad) out[copiados++] = nodo.reg;
            total++;
        }
        pos = nodo.siguiente;
    }
    return total;
}
