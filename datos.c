#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datos.h"

struct DataArea {
    FILE *fp;
    long  accesos;
};

size_t da_bloque_bytes(void) {
    return sizeof(int32_t) + sizeof(int64_t) + ISAM_REGS_POR_BLOQUE * registro_bytes();
}

DataArea *da_abrir(const char *ruta) {
    DataArea *da = calloc(1, sizeof(DataArea));
    if (!da) return NULL;

    da->fp = fopen(ruta, "r+b");
    if (!da->fp) da->fp = fopen(ruta, "w+b");
    if (!da->fp) { free(da); return NULL; }

    return da;
}

void da_cerrar(DataArea *da) {
    if (!da) return;
    if (da->fp) fclose(da->fp);
    free(da);
}

bool da_truncar(DataArea *da) {
    FILE *nuevo = freopen(NULL, "w+b", da->fp);
    if (!nuevo) return false;
    da->fp = nuevo;
    return true;
}

long da_contar_bloques(DataArea *da) {
    fseek(da->fp, 0, SEEK_END);
    long tam = ftell(da->fp);
    return tam / (long)da_bloque_bytes();
}

bool da_leer_bloque(DataArea *da, long numero_bloque,
                     IsamRegistro *regs_out, int *cantidad_out,
                     int64_t *puntero_overflow_out) {
    size_t tam_bloque = da_bloque_bytes();
    uint8_t *buf = malloc(tam_bloque);
    if (!buf) return false;

    fseek(da->fp, numero_bloque * (long)tam_bloque, SEEK_SET);
    if (fread(buf, 1, tam_bloque, da->fp) != tam_bloque) { free(buf); return false; }
    da->accesos++;

    int32_t cnt;
    memcpy(&cnt, buf, sizeof(int32_t));
    memcpy(puntero_overflow_out, buf + sizeof(int32_t), sizeof(int64_t));
    *cantidad_out = cnt;

    size_t base = sizeof(int32_t) + sizeof(int64_t);
    for (int i = 0; i < cnt; i++) {
        int64_t reservado_dummy; /* el area de datos no usa este campo */
        registro_deserializar(buf + base + (size_t)i * registro_bytes(), &regs_out[i], &reservado_dummy);
    }
    free(buf);
    return true;
}

bool da_escribir_bloque(DataArea *da, long numero_bloque,
                         const IsamRegistro *regs, int cantidad,
                         int64_t puntero_overflow) {
    size_t tam_bloque = da_bloque_bytes();
    uint8_t *buf = calloc(1, tam_bloque);
    if (!buf) return false;

    int32_t cnt = cantidad;
    memcpy(buf, &cnt, sizeof(int32_t));
    memcpy(buf + sizeof(int32_t), &puntero_overflow, sizeof(int64_t));

    size_t base = sizeof(int32_t) + sizeof(int64_t);
    for (int i = 0; i < cantidad; i++) {
        /* ISAM_PUNTERO_NULO: el "siguiente" del registro no se usa aca,
         * solo lo usan los registros que viven en el area de overflow. */
        registro_serializar(&regs[i], ISAM_PUNTERO_NULO, buf + base + (size_t)i * registro_bytes());
    }

    fseek(da->fp, numero_bloque * (long)tam_bloque, SEEK_SET);
    bool ok = fwrite(buf, 1, tam_bloque, da->fp) == tam_bloque;
    fflush(da->fp);
    da->accesos++;
    free(buf);
    return ok;
}

bool da_construir(DataArea *da, const IsamRegistro *registros_ordenados, size_t n) {
    if (!da_truncar(da)) return false;

    long bloque_actual = 0;
    for (size_t i = 0; i < n; i += ISAM_FACTOR_CARGA) {
        size_t fin = (i + ISAM_FACTOR_CARGA < n) ? i + ISAM_FACTOR_CARGA : n;
        int cantidad = (int)(fin - i);
        if (!da_escribir_bloque(da, bloque_actual, &registros_ordenados[i], cantidad, ISAM_PUNTERO_NULO))
            return false;
        bloque_actual++;
    }
    return true;
}

long da_accesos(DataArea *da) { return da->accesos; }
void da_reiniciar_accesos(DataArea *da) { da->accesos = 0; }
