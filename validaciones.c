#include "validaciones.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINEA 256

static int leerLinea(char *destino, size_t capacidad)
{
    size_t longitud;

    if (fgets(destino, (int)capacidad, stdin) == NULL)
    {
        printf("\nFin de entrada. Se cierra el simulador.\n");
        exit(EXIT_SUCCESS);
    }

    longitud = strlen(destino);
    if (longitud > 0 && destino[longitud - 1] == '\n')
    {
        destino[longitud - 1] = '\0';
        return 1;
    }

    /* Si no entró toda la línea, se descarta el resto para evitar
       que quede como entrada pendiente para la próxima lectura. */
    if (!feof(stdin))
    {
        int caracter;
        while ((caracter = getchar()) != '\n' && caracter != EOF)
        {
            /* Descartar caracteres sobrantes. */
        }
        return 0;
    }

    return 1;
}

static char *quitarEspacios(char *texto)
{
    char *inicio = texto;
    char *fin;

    while (isspace((unsigned char)*inicio))
    {
        inicio++;
    }

    if (*inicio == '\0')
    {
        return inicio;
    }

    fin = inicio + strlen(inicio) - 1;
    while (fin > inicio && isspace((unsigned char)*fin))
    {
        *fin = '\0';
        fin--;
    }

    return inicio;
}

int leerEntero(const char *mensaje, int minimo, int maximo)
{
    char linea[TAM_LINEA];
    char *fin;
    long valor;

    for (;;)
    {
        printf("%s", mensaje);
        if (!leerLinea(linea, sizeof(linea)))
        {
            printf("Entrada demasiado larga. Intente nuevamente.\n");
            continue;
        }

        errno = 0;
        valor = strtol(quitarEspacios(linea), &fin, 10);
        while (isspace((unsigned char)*fin))
        {
            fin++;
        }

        if (linea[0] == '\0' || *quitarEspacios(linea) == '\0' ||
            errno == ERANGE || fin == quitarEspacios(linea) || *fin != '\0' ||
            valor < minimo || valor > maximo)
        {
            printf("Valor invalido. Ingrese un entero entre %d y %d.\n",
                   minimo, maximo);
            continue;
        }

        return (int)valor;
    }
}

double leerReal(const char *mensaje, double minimo, double maximo)
{
    char linea[TAM_LINEA];
    char normalizada[TAM_LINEA];
    char *inicio;
    char *fin;
    char *coma;
    char *punto;
    double valor;
    size_t i;

    for (;;)
    {
        printf("%s", mensaje);
        if (!leerLinea(linea, sizeof(linea)))
        {
            printf("Entrada demasiado larga. Intente nuevamente.\n");
            continue;
        }

        inicio = quitarEspacios(linea);
        if (*inicio == '\0')
        {
            printf("El valor no puede estar vacio.\n");
            continue;
        }

        if (strlen(inicio) >= sizeof(normalizada))
        {
            printf("Entrada demasiado larga.\n");
            continue;
        }
        strcpy(normalizada, inicio);

        coma = strchr(normalizada, ',');
        punto = strchr(normalizada, '.');
        if (coma != NULL && punto != NULL)
        {
            printf("Use punto o coma decimal, pero no ambos separadores.\n");
            continue;
        }
        if (coma != NULL)
        {
            *coma = '.';
        }

        /* Rechaza separadores de miles y caracteres no numericos. */
        for (i = 0; normalizada[i] != '\0'; i++)
        {
            if (!(isdigit((unsigned char)normalizada[i]) ||
                  normalizada[i] == '.' || normalizada[i] == '+' ||
                  normalizada[i] == '-' || normalizada[i] == 'e' ||
                  normalizada[i] == 'E'))
            {
                break;
            }
        }
        if (normalizada[i] != '\0')
        {
            printf("Formato numerico incorrecto. Use un unico separador decimal y no separe miles.\n");
            continue;
        }

        errno = 0;
        valor = strtod(normalizada, &fin);
        while (isspace((unsigned char)*fin))
        {
            fin++;
        }

        if (errno == ERANGE || fin == normalizada || *fin != '\0' ||
            !isfinite(valor) || valor < minimo || valor > maximo)
        {
            printf("Valor invalido. Rango permitido:  %.2f a  %.2f\n",
                   minimo, maximo);
            continue;
        }

        return valor;
    }
}