#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_EXPR 96
#define NUM_BOTONES 30

#define PI_VAL 3.14159265358979323846

/* =========================================================
   ESTADO
   ========================================================= */

char expresion[MAX_EXPR];
char resultado[64];

int cursor_expr = 0;
double ans = 0.0;
int error_calculo = 0;

PrintConsole topScreen;
PrintConsole bottomScreen;

/* =========================================================
   BOTONES
   ========================================================= */

typedef struct
{
    int cx;
    int cy;

    const char *label;
    const char *insert;

    int tipo;
    int radio;

} Boton;

/*
    tipo:

    0 = normal
    1 = AC
    2 = DEL
    3 = =
*/

/*
    Pantalla inferior:

       SIN  COS  TAN  LOG  LN
       SQR  X2   ^    (    )
        7    8    9   DEL  AC
        4    5    6    X   /
        1    2    3    +   -
        0    .    PI   !   =
*/

#define C0 24
#define C1 76
#define C2 128
#define C3 180
#define C4 232

#define Y0 16
#define Y1 44
#define Y2 80
#define Y3 108
#define Y4 136
#define Y5 164

Boton botones[NUM_BOTONES] =
{
    /* fila 0 */

    {C0, Y0, "SIN", "sin(", 0, 15},
    {C1, Y0, "COS", "cos(", 0, 15},
    {C2, Y0, "TAN", "tan(", 0, 15},
    {C3, Y0, "LOG", "log(", 0, 15},
    {C4, Y0, "LN",  "ln(",  0, 15},

    /* fila 1 */

    {C0, Y1, "SQR", "sqrt(", 0, 15},
    {C1, Y1, "X2",  "^2",    0, 15},
    {C2, Y1, "^",   "^",     0, 15},
    {C3, Y1, "(",   "(",     0, 15},
    {C4, Y1, ")",   ")",     0, 15},

    /* fila 2 */

    {C0, Y2, "7",   "7",  0, 15},
    {C1, Y2, "8",   "8",  0, 15},
    {C2, Y2, "9",   "9",  0, 15},
    {C3, Y2, "DEL", "",   2, 15},
    {C4, Y2, "AC",  "",   1, 15},

    /* fila 3 */

    {C0, Y3, "4", "4", 0, 15},
    {C1, Y3, "5", "5", 0, 15},
    {C2, Y3, "6", "6", 0, 15},
    {C3, Y3, "X", "*", 0, 15},
    {C4, Y3, "/", "/", 0, 15},

    /* fila 4 */

    {C0, Y4, "1", "1", 0, 15},
    {C1, Y4, "2", "2", 0, 15},
    {C2, Y4, "3", "3", 0, 15},
    {C3, Y4, "+", "+", 0, 15},
    {C4, Y4, "-", "-", 0, 15},

    /* fila 5 */

    {C0, Y5, "0",  "0",  0, 15},
    {C1, Y5, ".",  ".",  0, 15},
    {C2, Y5, "PI", "pi", 0, 15},
    {C3, Y5, "!",  "!",  0, 15},
    {C4, Y5, "=",  "",   3, 15}
};

int boton_seleccionado = 10;

/* =========================================================
   SPRITES
   ========================================================= */

OamState oamSub;

u16 *graficos_botones[NUM_BOTONES];

/* =========================================================
   COLORES
   ========================================================= */

#define COLOR_TRANSPARENTE 0

#define COLOR_BORDE \
    (RGB15(31, 31, 31) | BIT(15))

#define COLOR_SELECCION \
    (RGB15(31, 25, 0) | BIT(15))

#define COLOR_NUMERO \
    (RGB15(4, 18, 10) | BIT(15))

#define COLOR_FUNCION \
    (RGB15(8, 8, 20) | BIT(15))

#define COLOR_OPERADOR \
    (RGB15(20, 10, 3) | BIT(15))

#define COLOR_ESPECIAL \
    (RGB15(20, 3, 3) | BIT(15))

/* =========================================================
   MATEMATICAS
   ========================================================= */

double deg2rad(double x)
{
    return x * PI_VAL / 180.0;
}

double factorial(double x)
{
    int n;
    double r;

    if (x < 0.0 || x > 12.0)
        return NAN;

    n = (int)x;

    if (fabs(x - (double)n) > 0.0000001)
        return NAN;

    r = 1.0;

    for (int i = 2; i <= n; i++)
        r *= i;

    return r;
}

/* =========================================================
   PARSER
   ========================================================= */

const char *parser_ptr;

double parse_suma(void);
double parse_producto(void);
double parse_potencia(void);
double parse_unario(void);
double parse_postfijo(void);
double parse_funcion(void);
double parse_primario(void);

void saltar_espacios(void)
{
    while (*parser_ptr == ' ' ||
           *parser_ptr == '\t')
    {
        parser_ptr++;
    }
}

double parse_numero(void)
{
    char *fin;
    double valor;

    saltar_espacios();

    if (strncmp(parser_ptr, "pi", 2) == 0)
    {
        parser_ptr += 2;
        return PI_VAL;
    }

    if (strncmp(parser_ptr, "ans", 3) == 0)
    {
        parser_ptr += 3;
        return ans;
    }

    valor = strtod(parser_ptr, &fin);

    if (fin == parser_ptr)
    {
        error_calculo = 1;
        return 0.0;
    }

    parser_ptr = fin;

    return valor;
}

double parse_primario(void)
{
    double valor;

    saltar_espacios();

    if (*parser_ptr == '(')
    {
        parser_ptr++;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')')
        {
            error_calculo = 1;
            return 0.0;
        }

        parser_ptr++;

        return valor;
    }

    return parse_numero();
}

double parse_funcion(void)
{
    double valor;

    saltar_espacios();

    if (strncmp(parser_ptr, "sqrt(", 5) == 0)
    {
        parser_ptr += 5;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')')
        {
            error_calculo = 1;
            return 0.0;
        }

        parser_ptr++;

        if (valor < 0.0)
        {
            error_calculo = 1;
            return 0.0;
        }

        return sqrt(valor);
    }

    if (strncmp(parser_ptr, "sin(", 4) == 0)
    {
        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')')
        {
            error_calculo = 1;
            return 0.0;
        }

        parser_ptr++;

        return sin(deg2rad(valor));
    }

    if (strncmp(parser_ptr, "cos(", 4) == 0)
    {
        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')')
        {
            error_calculo = 1;
            return 0.0;
        }

        parser_ptr++;

        return cos(deg2rad(valor));
    }

    if (strncmp(parser_ptr, "tan(", 4) == 0)
    {
        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')')
        {
            error_calculo = 1;
            return 0.0;
        }

        parser_ptr++;

        return tan(deg2rad(valor));
    }

    if (strncmp(parser_ptr, "log(", 4) == 0)
    {
        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')')
        {
            error_calculo = 1;
            return 0.0;
        }

        parser_ptr++;

        if (valor <= 0.0)
        {
            error_calculo = 1;
            return 0.0;
        }

        return log10(valor);
    }

    if (strncmp(parser_ptr, "ln(", 3) == 0)
    {
        parser_ptr += 3;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')')
        {
            error_calculo = 1;
            return 0.0;
        }

        parser_ptr++;

        if (valor <= 0.0)
        {
            error_calculo = 1;
            return 0.0;
        }

        return log(valor);
    }

    return parse_primario();
}

double parse_postfijo(void)
{
    double valor;

    valor = parse_funcion();

    while (!error_calculo)
    {
        saltar_espacios();

        if (*parser_ptr == '!')
        {
            parser_ptr++;

            valor = factorial(valor);

            if (isnan(valor))
            {
                error_calculo = 1;
                return 0.0;
            }
        }
        else if (*parser_ptr == '%')
        {
            parser_ptr++;

            valor /= 100.0;
        }
        else
        {
            break;
        }
    }

    return valor;
}

double parse_unario(void)
{
    saltar_espacios();

    if (*parser_ptr == '+')
    {
        parser_ptr++;
        return parse_unario();
    }

    if (*parser_ptr == '-')
    {
        parser_ptr++;
        return -parse_unario();
    }

    return parse_postfijo();
}

double parse_potencia(void)
{
    double izquierda;
    double derecha;

    izquierda = parse_unario();

    if (error_calculo)
        return 0.0;

    saltar_espacios();

    if (*parser_ptr == '^')
    {
        parser_ptr++;

        derecha = parse_potencia();

        if (error_calculo)
            return 0.0;

        izquierda = pow(
            izquierda,
            derecha
        );

        if (isnan(izquierda) ||
            isinf(izquierda))
        {
            error_calculo = 1;
            return 0.0;
        }
    }

    return izquierda;
}
double parse_producto(void)
{
    double resultado_local;
    double valor;

    resultado_local =
        parse_potencia();

    if (error_calculo)
        return 0.0;

    while (!error_calculo)
    {
        saltar_espacios();

        if (*parser_ptr == '*')
        {
            parser_ptr++;

            valor =
                parse_potencia();

            if (error_calculo)
                return 0.0;

            resultado_local *= valor;
        }
        else if (*parser_ptr == '/')
        {
            parser_ptr++;

            valor =
                parse_potencia();

            if (error_calculo)
                return 0.0;

            if (fabs(valor) < 0.000000000001)
            {
                error_calculo = 1;
                return 0.0;
            }

            resultado_local /= valor;
        }
        else
        {
            break;
        }
    }

    return resultado_local;
}

double parse_suma(void)
{
    double resultado_local;
    double valor;

    resultado_local =
        parse_producto();

    if (error_calculo)
        return 0.0;

    while (!error_calculo)
    {
        saltar_espacios();

        if (*parser_ptr == '+')
        {
            parser_ptr++;

            valor =
                parse_producto();

            if (error_calculo)
                return 0.0;

            resultado_local += valor;
        }
        else if (*parser_ptr == '-')
        {
            parser_ptr++;

            valor =
                parse_producto();

            if (error_calculo)
                return 0.0;

            resultado_local -= valor;
        }
        else
        {
            break;
        }
    }

    return resultado_local;
}

/* =========================================================
   VALIDACION
   ========================================================= */

int expresion_balanceada(const char *texto)
{
    int nivel = 0;

    for (int i = 0;
         texto[i] != '\0';
         i++)
    {
        if (texto[i] == '(')
        {
            nivel++;
        }
        else if (texto[i] == ')')
        {
            nivel--;

            if (nivel < 0)
                return 0;
        }
    }

    return nivel == 0;
}

int sintaxis_basica_valida(const char *texto)
{
    int len;
    char ultimo;

    len = strlen(texto);

    if (len <= 0)
        return 0;

    ultimo = texto[len - 1];

    if (ultimo == '+' ||
        ultimo == '-' ||
        ultimo == '*' ||
        ultimo == '/' ||
        ultimo == '^' ||
        ultimo == '(')
    {
        return 0;
    }

    if (ultimo == '.')
        return 0;

    if (!expresion_balanceada(texto))
        return 0;

    return 1;
}

/* =========================================================
   PANTALLA SUPERIOR
   ========================================================= */

void actualizar(void)
{
    consoleSelect(&topScreen);

    consoleClear();

    printf("\n");
    printf(" DS SCIENTIFIC CALCULATOR\n");
    printf("-------------------------\n\n");

    printf("EXPRESION:\n");

    if (expresion[0] != '\0')
        printf("%s\n", expresion);
    else
        printf("0\n");

    printf("\n");
    printf("RESULTADO:\n");
    printf("%s\n", resultado);

    printf("\n");
    printf("ANS = %.10g\n", ans);

    if (error_calculo)
    {
        printf("\n");
        printf("ERROR: EXPRESION INVALIDA\n");
    }
}

/* =========================================================
   EDITAR EXPRESION
   ========================================================= */

void agregar(const char *s)
{
    int len;

    len = strlen(s);

    if (cursor_expr + len >= MAX_EXPR)
        return;

    strcat(
        expresion,
        s
    );

    cursor_expr += len;

    error_calculo = 0;

    actualizar();
}

void borrar(void)
{
    if (cursor_expr <= 0)
        return;

    cursor_expr--;

    expresion[cursor_expr] =
        '\0';

    error_calculo = 0;

    actualizar();
}

void limpiar(void)
{
    expresion[0] =
        '\0';

    cursor_expr = 0;

    strcpy(
        resultado,
        "0"
    );

    error_calculo = 0;

    actualizar();
}

/* =========================================================
   CALCULAR
   ========================================================= */

void calcular(void)
{
    double r;

    if (expresion[0] == '\0')
    {
        strcpy(
            resultado,
            "ERROR"
        );

        error_calculo = 1;

        actualizar();

        return;
    }

    if (!sintaxis_basica_valida(expresion))
    {
        strcpy(
            resultado,
            "ERROR"
        );

        error_calculo = 1;

        actualizar();

        return;
    }

    error_calculo = 0;

    parser_ptr = expresion;

    r = parse_suma();

    saltar_espacios();

    if (*parser_ptr != '\0')
        error_calculo = 1;

    if (isnan(r) ||
        isinf(r))
    {
        error_calculo = 1;
    }

    if (error_calculo)
    {
        strcpy(
            resultado,
            "ERROR"
        );
    }
    else
    {
        ans = r;

        snprintf(
            resultado,
            sizeof(resultado),
            "%.10g",
            r
        );
    }

    expresion[0] =
        '\0';

    cursor_expr = 0;

    actualizar();
}

/* =========================================================
   GRAFICOS
   ========================================================= */

void limpiar_sprite(u16 *gfx)
{
    for (int y = 0;
         y < 32;
         y++)
    {
        for (int x = 0;
             x < 32;
             x++)
        {
            gfx[y * 32 + x] =
                COLOR_TRANSPARENTE;
        }
    }
}

void dibujar_circulo(
    u16 *gfx,
    u16 color,
    int radio
)
{
    int centro = 15;

    for (int y = -radio;
         y <= radio;
         y++)
    {
        for (int x = -radio;
             x <= radio;
             x++)
        {
            int d =
                x * x +
                y * y;

            if (d <= radio * radio)
            {
                int px =
                    centro + x;

                int py =
                    centro + y;

                if (px >= 0 &&
                    px < 32 &&
                    py >= 0 &&
                    py < 32)
                {
                    gfx[
                        py * 32 + px
                    ] = color;
                }
            }
        }
    }
}

void dibujar_anillo(
    u16 *gfx,
    u16 color,
    int radio_exterior,
    int radio_interior
)
{
    int centro = 15;

    for (int y = -radio_exterior;
         y <= radio_exterior;
         y++)
    {
        for (int x = -radio_exterior;
             x <= radio_exterior;
             x++)
        {
            int d =
                x * x +
                y * y;

            if (d <=
                    radio_exterior *
                    radio_exterior &&
                d >=
                    radio_interior *
                    radio_interior)
            {
                int px =
                    centro + x;

                int py =
                    centro + y;

                if (px >= 0 &&
                    px < 32 &&
                    py >= 0 &&
                    py < 32)
                {
                    gfx[
                        py * 32 + px
                    ] = color;
                }
            }
        }
    }
}

u16 obtener_color_boton(int indice)
{
    int fila =
        indice / 5;

    int col =
        indice % 5;

    if (fila < 2)
        return COLOR_FUNCION;

    if (indice == 13 ||
        indice == 14)
    {
        return COLOR_ESPECIAL;
    }

    if (col >= 3)
        return COLOR_OPERADOR;

    return COLOR_NUMERO;
}

/* =========================================================
   FUENTE 5x7
   ========================================================= */

void obtener_glyph(
    char c,
    u8 g[7]
)
{
    for (int i = 0; i < 7; i++)
        g[i] = 0;

    switch (c)
    {
        case '0':
            g[0]=0x0E; g[1]=0x11; g[2]=0x13;
            g[3]=0x15; g[4]=0x19; g[5]=0x11;
            g[6]=0x0E;
            break;

        case '1':
            g[0]=0x04; g[1]=0x0C; g[2]=0x04;
            g[3]=0x04; g[4]=0x04; g[5]=0x04;
            g[6]=0x0E;
            break;

        case '2':
            g[0]=0x0E; g[1]=0x11; g[2]=0x01;
            g[3]=0x02; g[4]=0x04; g[5]=0x08;
            g[6]=0x1F;
            break;

        case '3':
            g[0]=0x1E; g[1]=0x01; g[2]=0x01;
            g[3]=0x0E; g[4]=0x01; g[5]=0x01;
            g[6]=0x1E;
            break;

        case '4':
            g[0]=0x02; g[1]=0x06; g[2]=0x0A;
            g[3]=0x12; g[4]=0x1F; g[5]=0x02;
            g[6]=0x02;
            break;

        case '5':
            g[0]=0x1F; g[1]=0x10; g[2]=0x10;
            g[3]=0x1E; g[4]=0x01; g[5]=0x01;
            g[6]=0x1E;
            break;

        case '6':
            g[0]=0x0E; g[1]=0x10; g[2]=0x10;
            g[3]=0x1E; g[4]=0x11; g[5]=0x11;
            g[6]=0x0E;
            break;

        case '7':
            g[0]=0x1F; g[1]=0x01; g[2]=0x02;
            g[3]=0x04; g[4]=0x08; g[5]=0x08;
            g[6]=0x08;
            break;

        case '8':
            g[0]=0x0E; g[1]=0x11; g[2]=0x11;
            g[3]=0x0E; g[4]=0x11; g[5]=0x11;
            g[6]=0x0E;
            break;

        case '9':
            g[0]=0x0E; g[1]=0x11; g[2]=0x11;
            g[3]=0x0F; g[4]=0x01; g[5]=0x01;
            g[6]=0x0E;
            break;

        case 'A':
            g[0]=0x0E; g[1]=0x11; g[2]=0x11;
            g[3]=0x1F; g[4]=0x11; g[5]=0x11;
            g[6]=0x11;
            break;

        case 'C':
            g[0]=0x0E; g[1]=0x11; g[2]=0x10;
            g[3]=0x10; g[4]=0x10; g[5]=0x11;
            g[6]=0x0E;
            break;

        case 'D':
            g[0]=0x1E; g[1]=0x11; g[2]=0x11;
            g[3]=0x11; g[4]=0x11; g[5]=0x11;
            g[6]=0x1E;
            break;

        case 'E':
            g[0]=0x1F; g[1]=0x10; g[2]=0x10;
            g[3]=0x1E; g[4]=0x10; g[5]=0x10;
            g[6]=0x1F;
            break;

        case 'G':
            g[0]=0x0E; g[1]=0x11; g[2]=0x10;
            g[3]=0x17; g[4]=0x11; g[5]=0x11;
            g[6]=0x0E;
            break;

        case 'I':
            g[0]=0x1F; g[1]=0x04; g[2]=0x04;
            g[3]=0x04; g[4]=0x04; g[5]=0x04;
            g[6]=0x1F;
            break;

        case 'L':
            g[0]=0x10; g[1]=0x10; g[2]=0x10;
            g[3]=0x10; g[4]=0x10; g[5]=0x10;
            g[6]=0x1F;
            break;

        case 'N':
            g[0]=0x11; g[1]=0x19; g[2]=0x15;
            g[3]=0x13; g[4]=0x11; g[5]=0x11;
            g[6]=0x11;
            break;

        case 'O':
            g[0]=0x0E; g[1]=0x11; g[2]=0x11;
            g[3]=0x11; g[4]=0x11; g[5]=0x11;
            g[6]=0x0E;
            break;

        case 'P':
            g[0]=0x1E; g[1]=0x11; g[2]=0x11;
            g[3]=0x1E; g[4]=0x10; g[5]=0x10;
            g[6]=0x10;
            break;

        case 'R':
            g[0]=0x1E; g[1]=0x11; g[2]=0x11;
            g[3]=0x1E; g[4]=0x14; g[5]=0x12;
            g[6]=0x11;
            break;

        case 'S':
            g[0]=0x0F; g[1]=0x10; g[2]=0x10;
            g[3]=0x0E; g[4]=0x01; g[5]=0x01;
            g[6]=0x1E;
            break;

        case 'T':
            g[0]=0x1F; g[1]=0x04; g[2]=0x04;
            g[3]=0x04; g[4]=0x04; g[5]=0x04;
            g[6]=0x04;
            break;

        case 'X':
            g[0]=0x11; g[1]=0x0A; g[2]=0x04;
            g[3]=0x04; g[4]=0x04; g[5]=0x0A;
            g[6]=0x11;
            break;

        case '+':
            g[0]=0x00; g[1]=0x04; g[2]=0x04;
            g[3]=0x1F; g[4]=0x04; g[5]=0x04;
            g[6]=0x00;
            break;

        case '-':
            g[0]=0x00; g[1]=0x00; g[2]=0x00;
            g[3]=0x1F; g[4]=0x00; g[5]=0x00;
            g[6]=0x00;
            break;

        case '/':
            g[0]=0x01; g[1]=0x02; g[2]=0x04;
            g[3]=0x08; g[4]=0x10; g[5]=0x00;
            g[6]=0x00;
            break;

        case '^':
            g[0]=0x04; g[1]=0x0A; g[2]=0x11;
            g[3]=0x00; g[4]=0x00; g[5]=0x00;
            g[6]=0x00;
            break;

        case '(':
            g[0]=0x02; g[1]=0x04; g[2]=0x08;
            g[3]=0x08; g[4]=0x08; g[5]=0x04;
            g[6]=0x02;
            break;

        case ')':
            g[0]=0x08; g[1]=0x04; g[2]=0x02;
            g[3]=0x02; g[4]=0x02; g[5]=0x04;
            g[6]=0x08;
            break;

        case '!':
            g[0]=0x04; g[1]=0x04; g[2]=0x04;
            g[3]=0x04; g[4]=0x04; g[5]=0x00;
            g[6]=0x04;
            break;

        case '.':
            g[0]=0x00; g[1]=0x00; g[2]=0x00;
            g[3]=0x00; g[4]=0x00; g[5]=0x00;
            g[6]=0x04;
            break;

        case '%':
            g[0]=0x19; g[1]=0x19; g[2]=0x02;
            g[3]=0x04; g[4]=0x08; g[5]=0x13;
            g[6]=0x13;
            break;

        case '=':
            g[0]=0x00; g[1]=0x1F; g[2]=0x00;
            g[3]=0x1F; g[4]=0x00; g[5]=0x00;
            g[6]=0x00;
            break;
    }
}

void dibujar_caracter(
    u16 *gfx,
    char c,
    int x,
    int y,
    int escala,
    u16 color
)
{
    u8 g[7];

    obtener_glyph(c, g);

    for (int fila = 0; fila < 7; fila++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (g[fila] & (1 << (4 - col)))
            {
                for (int sy = 0;
                     sy < escala;
                     sy++)
                {
                    for (int sx = 0;
                         sx < escala;
                         sx++)
                    {
                        int px =
                            x +
                            col * escala +
                            sx;

                        int py =
                            y +
                            fila * escala +
                            sy;

                        if (px >= 0 &&
                            px < 32 &&
                            py >= 0 &&
                            py < 32)
                        {
                            gfx[
                                py * 32 + px
                            ] = color;
                        }
                    }
                }
            }
        }
    }
}

void dibujar_texto_boton(
    u16 *gfx,
    const char *texto
)
{
    int largo =
        strlen(texto);

    int escala;

    int ancho;

    int x;

    int y;


    if (largo <= 2)
        escala = 2;
    else
        escala = 1;


    ancho =
        largo * (5 * escala + escala)
        - escala;


    x =
        (32 - ancho) / 2;


    y =
        (32 - 7 * escala) / 2;


    for (int i = 0;
         i < largo;
         i++)
    {
        dibujar_caracter(
            gfx,
            texto[i],
            x,
            y,
            escala,
            COLOR_BORDE
        );

        x +=
            5 * escala +
            escala;
    }
}
void crear_grafico_boton(int indice)
{
    u16 *gfx;

    gfx =
        graficos_botones[indice];

    if (gfx == NULL)
        return;

    limpiar_sprite(gfx);

    /*
       Circulo exterior.
    */

    dibujar_circulo(
        gfx,
        COLOR_BORDE,
        15
    );

    /*
       Interior.
    */

    dibujar_circulo(
        gfx,
        obtener_color_boton(indice),
        12
    );

    /*
       Anillo amarillo del seleccionado.
    */

    if (indice ==
        boton_seleccionado)
    {
        dibujar_anillo(
            gfx,
            COLOR_SELECCION,
            15,
            12
        );
    }

    /*
       Texto directamente dentro
       del sprite.

       Así el texto y la zona táctil
       pertenecen al mismo botón.
    */

    dibujar_texto_boton(
        gfx,
        botones[indice].label
    );
}

/* =========================================================
   ACTUALIZAR SPRITES
   ========================================================= */

void actualizar_sprites(void)
{
    for (int i = 0;
         i < NUM_BOTONES;
         i++)
    {
        if (graficos_botones[i] == NULL)
            continue;

        crear_grafico_boton(i);

        /*
           IMPORTANTE:

           La versión de BlocksDS del
           usuario utiliza esta firma
           de 15 argumentos.
        */

        oamSet(
            &oamSub,
            i,
            botones[i].cx - 16,
            botones[i].cy - 16,
            0,
            0,
            SpriteSize_32x32,
            SpriteColorFormat_Bmp,
            graficos_botones[i],
            -1,
            false,
            false,
            false,
            false,
            false
        );
    }

    oamUpdate(&oamSub);
}

/* =========================================================
   TOUCH
   ========================================================= */

int detectar_boton(
    int tx,
    int ty
)
{
    for (int i = 0;
         i < NUM_BOTONES;
         i++)
    {
        int dx;
        int dy;
        int radio;

        dx =
            tx - botones[i].cx;

        dy =
            ty - botones[i].cy;

        radio =
            botones[i].radio;

        /*
           El área táctil es un círculo.

           No hay rectángulo gigante alrededor
           del botón.
        */

        if (
            dx * dx +
            dy * dy
            <=
            radio * radio
        )
        {
            return i;
        }
    }

    return -1;
}

/* =========================================================
   PROCESAR BOTON
   ========================================================= */

void procesar_boton(int indice)
{
    Boton *b;

    if (indice < 0 ||
        indice >= NUM_BOTONES)
    {
        return;
    }

    b =
        &botones[indice];

    /*
       AC
    */

    if (b->tipo == 1)
    {
        limpiar();
        return;
    }

    /*
       DEL
    */

    if (b->tipo == 2)
    {
        borrar();
        return;
    }

    /*
       =
    */

    if (b->tipo == 3)
    {
        calcular();
        return;
    }

    /*
       Boton normal.
    */

    if (b->insert != NULL &&
        b->insert[0] != '\0')
    {
        agregar(
            b->insert
        );
    }
}

/* =========================================================
   NAVEGACION
   ========================================================= */

void mover_arriba(void)
{
    int fila;
    int col;

    fila =
        boton_seleccionado / 5;

    col =
        boton_seleccionado % 5;

    if (fila > 0)
        fila--;
    else
        fila = 5;

    boton_seleccionado =
        fila * 5 + col;

    actualizar_sprites();
}

void mover_abajo(void)
{
    int fila;
    int col;

    fila =
        boton_seleccionado / 5;

    col =
        boton_seleccionado % 5;

    if (fila < 5)
        fila++;
    else
        fila = 0;

    boton_seleccionado =
        fila * 5 + col;

    actualizar_sprites();
}

void mover_izquierda(void)
{
    int fila;
    int col;

    fila =
        boton_seleccionado / 5;

    col =
        boton_seleccionado % 5;

    if (col > 0)
        col--;
    else
        col = 4;

    boton_seleccionado =
        fila * 5 + col;

    actualizar_sprites();
}

void mover_derecha(void)
{
    int fila;
    int col;

    fila =
        boton_seleccionado / 5;

    col =
        boton_seleccionado % 5;

    if (col < 4)
        col++;
    else
        col = 0;

    boton_seleccionado =
        fila * 5 + col;

    actualizar_sprites();
}

void pulsar_seleccionado(void)
{
    procesar_boton(
        boton_seleccionado
    );

    actualizar_sprites();
}

/* =========================================================
   INICIALIZAR SPRITES
   ========================================================= */

int inicializar_sprites(void)
{
    oamInit(
        &oamSub,
        SpriteMapping_Bmp_1D_128,
        false
    );

    for (int i = 0;
         i < NUM_BOTONES;
         i++)
    {
        graficos_botones[i] =
            oamAllocateGfx(
                &oamSub,
                SpriteSize_32x32,
                SpriteColorFormat_Bmp
            );

        if (graficos_botones[i] == NULL)
        {
            return 0;
        }
    }

    return 1;
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    touchPosition touch;

    u32 keys;

    int indice;


    /*
       VIDEO PRINCIPAL
    */

    videoSetMode(
        MODE_0_2D
    );

    /*
       VIDEO INFERIOR
    */

    videoSetModeSub(
        MODE_0_2D
    );


    /*
       VRAM
    */

    vramSetBankA(
        VRAM_A_MAIN_BG
    );

    vramSetBankC(
        VRAM_C_SUB_BG
    );

    vramSetBankD(
        VRAM_D_SUB_SPRITE
    );


    /*
       CONSOLA SUPERIOR
    */

    consoleInit(
        &topScreen,
        3,
        BgType_Text4bpp,
        BgSize_T_256x256,
        31,
        0,
        true,
        true
    );


    /*
       CONSOLA INFERIOR.

       Se usa solamente como fondo de la
       pantalla inferior; los botones son
       sprites gráficos.
    */

    consoleInit(
        &bottomScreen,
        0,
        BgType_Text4bpp,
        BgSize_T_256x256,
        31,
        0,
        false,
        true
    );


    /*
       ESTADO INICIAL
    */

    expresion[0] =
        '\0';

    strcpy(
        resultado,
        "0"
    );

    cursor_expr = 0;

    ans = 0.0;

    error_calculo = 0;


    /*
       Seleccion inicial: 7
       indice 10.
    */

    boton_seleccionado = 10;


    /*
       Crear sprites.
    */

    if (!inicializar_sprites())
    {
        consoleSelect(
            &topScreen
        );

        consoleClear();

        printf("\n");
        printf("ERROR DE MEMORIA\n\n");
        printf("No se pudieron crear\n");
        printf("los botones.\n");

        while (1)
        {
            swiWaitForVBlank();
        }
    }


    /*
       Primera imagen.
    */

    actualizar();

    actualizar_sprites();


    /*
       BUCLE PRINCIPAL
    */

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        keys =
            keysDown();


        /*
           SELECT = salir
        */

        if (keys & KEY_SELECT)
        {
            break;
        }


        /*
           CRUCETA
        */

        if (keys & KEY_UP)
        {
            mover_arriba();
        }

        if (keys & KEY_DOWN)
        {
            mover_abajo();
        }

        if (keys & KEY_LEFT)
        {
            mover_izquierda();
        }

        if (keys & KEY_RIGHT)
        {
            mover_derecha();
        }


        /*
           START = pulsar
        */

        if (keys & KEY_START)
        {
            pulsar_seleccionado();
        }


        /*
           A = pulsar
        */

        if (keys & KEY_A)
        {
            pulsar_seleccionado();
        }


        /*
           B = DEL
        */

        if (keys & KEY_B)
        {
            borrar();

            actualizar_sprites();
        }


        /*
           X = AC
        */

        if (keys & KEY_X)
        {
            limpiar();

            actualizar_sprites();
        }


        /*
           TOUCH
        */

        if (keys & KEY_TOUCH)
        {
            touchRead(
                &touch
            );

            indice =
                detectar_boton(
                    touch.px,
                    touch.py
                );

            if (indice >= 0)
            {
                /*
                   El botón tocado se
                   convierte en seleccionado.
                */

                boton_seleccionado =
                    indice;

                actualizar_sprites();


                /*
                   Y además se ejecuta
                   inmediatamente.
                */

                procesar_boton(
                    indice
                );

                actualizar_sprites();
            }
        }
    }

    return 0;
}
