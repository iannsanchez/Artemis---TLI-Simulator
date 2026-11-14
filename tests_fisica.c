#include "artemis.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EXIGIR(condicion, mensaje) do { \
    if (!(condicion)) { \
        fprintf(stderr, "FALLO: %s\n", mensaje); \
        return EXIT_FAILURE; \
    } \
} while (0)

int main(void)
{
    DatosMision datos = {
        MASA_DEFAULT,
        VELOCIDAD_DEFAULT,
        ALTURA_DEFAULT,
        DISTANCIA_DEFAULT
    };
    ResultadosFisicos resultados;
    char error[160];
    double l1Tierra;
    double l1Luna;
    double mu;
    double n;

    EXIGIR(fabs(calcularEnergiaCinetica(26000.0, 10900.0) - 1.54453e12) < 1.0,
           "energia cinetica inicial");
    EXIGIR(calcularPuntoL1(DISTANCIA_DEFAULT, &l1Tierra, &l1Luna, &mu, &n),
           "calculo de L1");
    EXIGIR(fabs(l1Tierra / 1000.0 - 326041.235) < 1.0,
           "distancia Tierra-L1 de referencia");
    EXIGIR(fabs((l1Tierra + l1Luna) - DISTANCIA_DEFAULT) < 1.0e-5,
           "suma de distancias a L1");
    EXIGIR(calcularResultadosFisicos(&datos, &resultados, error, sizeof(error)),
           "calculo de resultados fisicos");
    EXIGIR(resultados.energiaCineticaJ > 1.5e12,
           "energia cinetica positiva");
    EXIGIR(resultados.velocidadCircularMps > 7700.0 &&
           resultados.velocidadCircularMps < 7900.0,
           "velocidad circular LEO razonable");

    datos.masaKg = -1.0;
    EXIGIR(!calcularResultadosFisicos(&datos, &resultados, error, sizeof(error)),
           "rechazo de masa negativa");

    printf("Todas las pruebas fisicas finalizaron correctamente.\n");
    return EXIT_SUCCESS;
}
