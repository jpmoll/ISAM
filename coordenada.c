#include <string.h>
#include "coordenada.h"

Registro coordenada_a_registro(Coordenada c) {
    Registro r;
    memset(&r, 0, sizeof(Registro));
    r.clave = c.id_coordenada;
    memcpy(r.datos, &c, sizeof(Coordenada));
    return r;
}

Coordenada registro_a_coordenada(Registro r) {
    Coordenada c;
    memcpy(&c, r.datos, sizeof(Coordenada));
    return c;
}
