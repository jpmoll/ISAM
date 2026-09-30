#include "registro.h"

static int tam_actual = 0;

void registro_configurar_tam(int tam_bytes_datos) {
    tam_actual = tam_bytes_datos;
}

int registro_tam_actual(void) {
    return tam_actual;
}
