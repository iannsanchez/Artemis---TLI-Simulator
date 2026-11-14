#include "artemis.h"
#include "validaciones.h"

#include <stdio.h>

static void mostrarMenu(void)
{
    printf("\n===============================================\n");
    printf("             ARTEMIS - TLI SIMULATOR\n");
    printf("===============================================\n");
    printf("1. Configurar parametros de Orion\n");
    printf("2. Ejecutar maniobra TLI\n");
    printf("3. Desacoplar etapa ICPS\n");
    printf("4. Desafio de navegacion: Primera Ley de Newton\n");
    printf("5. Analizar influencia gravitatoria\n");
    printf("6. Analizar energia y punto L1\n");
    printf("7. Consultar historial de eventos\n");
    printf("8. Generar informe final\n");
    printf("0. Salir\n");
    printf("-----------------------------------------------\n");
}

int main(void)
{
    Mision mision;
    int opcion;

    inicializarMision(&mision);
    printf("Bienvenido al simulador de la mision Artemis: Inyeccion Translunar.\n");
    printf("Modelo didactico: eventos de mision y analisis gravitatorio Tierra-Luna.\n");

    do {
        printf("\nEstado actual: %d - ", (int)mision.estado);
        switch (mision.estado) {
            case ESTADO_PREPARACION: printf("Preparacion LEO\n"); break;
            case ESTADO_TLI_REALIZADA: printf("TLI ejecutada\n"); break;
            case ESTADO_ICPS_SEPARADA: printf("ICPS desacoplada\n"); break;
            case ESTADO_ANALISIS_COMPLETO: printf("Analisis completo\n"); break;
            default: printf("Desconocido\n"); break;
        }

        mostrarMenu();
        opcion = leerEntero("Seleccione una opcion: ", 0, 8);

        switch (opcion) {
            case 1: configurarMision(&mision); break;
            case 2: ejecutarTLI(&mision); break;
            case 3: separarICPS(&mision); break;
            case 4: realizarDesafio(&mision); break;
            case 5: analizarGravedad(&mision); break;
            case 6: analizarEnergia(&mision); break;
            case 7: mostrarHistorial(&mision); break;
            case 8: generarInforme(&mision); break;
            case 0: printf("Cierre del simulador. Mision finalizada por el operador.\n"); break;
            default: printf("Opcion no reconocida.\n"); break;
        }

    } while (opcion != 0);

    return 0;
}