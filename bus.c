#include <string.h>
#include "bus.h"

Registro bus_a_registro(Bus b) {
    Registro r;
    memset(&r, 0, sizeof(Registro));
    r.clave = b.id_bus;
    // Copiamos la fila real tal cual, byte a byte, en el área de datos.
    // (memcpy simple porque Bus ya es un struct "plano"; si su tabla tiene
    // strings de largo variable tendrían que serializarlos aparte, pero
    // para tablas de columnas fijas esto alcanza).
    memcpy(r.datos, &b, sizeof(Bus));
    return r;
}

Bus registro_a_bus(Registro r) {
    Bus b;
    memcpy(&b, r.datos, sizeof(Bus));
    return b;
}
