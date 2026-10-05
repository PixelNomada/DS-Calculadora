#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_EXPR 96

char expresion[MAX_EXPR];
char resultado[64];

int cursor_expr = 0;
double ans = 0.0;
int error_calculo = 0;

PrintConsole topScreen;
PrintConsole bottomScreen;


/* =========================================================
   TECLADO
   ========================================================= */

typedef struct {
    int x;
    int y;
    int w;
    int h;

    const char *label;
    const char *insert;

    int tipo;
} Boton;


/*
   NUEVO DISEÑO:

   5 columnas
   6 filas

   Los botones tienen espacio real entre ellos.

   COLUMNAS:
   0      1      2      3      4

   FILAS:
   0  cientifica
   1  funciones
   2  7 8 9 DEL AC
   3  4 5 6 x  /
   4  1 2 3 +  -
   5  0 . pi ! =

   Esto hace que la cruceta tenga una rejilla
   perfectamente predecible.
*/


#define BTN_W 43
#define BTN_H 25

#define COL0 2
#define COL1 52
#define COL2 102
#define COL3 152
#define COL4 202

#define ROW0 2
#define ROW1 33
#define ROW2 64
#define ROW3 95
#define ROW4 126
#define ROW5 157


Boton botones[] = {

    /* =====================================================
       FILA 0
       ===================================================== */

    {COL0, ROW0, BTN_W, BTN_H, "SIN",  "sin(",  0},
    {COL1, ROW0, BTN_W, BTN_H, "COS",  "cos(",  0},
    {COL2, ROW0, BTN_W, BTN_H, "TAN",  "tan(",  0},
    {COL3, ROW0, BTN_W, BTN_H, "LOG",  "log(",  0},
    {COL4, ROW0, BTN_W, BTN_H, "LN",   "ln(",   0},


    /* =====================================================
       FILA 1
       ===================================================== */

    {COL0, ROW1, BTN_W, BTN_H, "SQRT", "sqrt(", 0},
    {COL1, ROW1, BTN_W, BTN_H, "X^2",  "^2",    0},
    {COL2, ROW1, BTN_W, BTN_H, "^",    "^",     0},
    {COL3, ROW1, BTN_W, BTN_H, "(",    "(",     0},
    {COL4, ROW1, BTN_W, BTN_H, ")",    ")",     0},


    /* =====================================================
       FILA 2
       ===================================================== */

    {COL0, ROW2, BTN_W, BTN_H, "7",    "7",     0},
    {COL1, ROW2, BTN_W, BTN_H, "8",    "8",     0},
    {COL2, ROW2, BTN_W, BTN_H, "9",    "9",     0},
    {COL3, ROW2, BTN_W, BTN_H, "DEL",  "",      2},
    {COL4, ROW2, BTN_W, BTN_H, "AC",   "",      1},


    /* =====================================================
       FILA 3
       ===================================================== */

    {COL0, ROW3, BTN_W, BTN_H, "4",    "4",     0},
    {COL1, ROW3, BTN_W, BTN_H, "5",    "5",     0},
    {COL2, ROW3, BTN_W, BTN_H, "6",    "6",     0},
    {COL3, ROW3, BTN_W, BTN_H, "X",    "*",     0},
    {COL4, ROW3, BTN_W, BTN_H, "/",    "/",     0},


    /* =====================================================
       FILA 4
       ===================================================== */

    {COL0, ROW4, BTN_W, BTN_H, "1",    "1",     0},
    {COL1, ROW4, BTN_W, BTN_H, "2",    "2",     0},
    {COL2, ROW4, BTN_W, BTN_H, "3",    "3",     0},
    {COL3, ROW4, BTN_W, BTN_H, "+",    "+",     0},
    {COL4, ROW4, BTN_W, BTN_H, "-",    "-",     0},


    /* =====================================================
       FILA 5
       ===================================================== */

    {COL0, ROW5, BTN_W, BTN_H, "0",    "0",     0},
    {COL1, ROW5, BTN_W, BTN_H, ".",    ".",     0},
    {COL2, ROW5, BTN_W, BTN_H, "PI",   "pi",    0},
    {COL3, ROW5, BTN_W, BTN_H, "!",    "!",     0},
    {COL4, ROW5, BTN_W, BTN_H, "=",    "",      3}
};


#define NUM_BOTONES \
    (sizeof(botones) / sizeof(Boton))


/*
   Este índice es independiente de la expresión.

   Sirve para saber qué botón está seleccionado
   con la cruceta.
*/
int boton_seleccionado = 10;


/* =========================================================
   PANTALLA SUPERIOR
   ========================================================= */

void actualizar(void)
{
    consoleSelect(&topScreen);
    consoleClear();

    printf("\n");
    printf(" DS SCIENTIFIC CALCULATOR\n");
    printf(" ------------------------\n\n");

    printf(" EXPRESION:\n");

    if (expresion[0] != '\0')
        printf(" %s\n", expresion);
    else
        printf(" 0\n");

    printf("\n");

    printf(" RESULTADO:\n");
    printf(" = %s\n", resultado);

    printf("\n");

    printf(" ANS = %.10g\n", ans);

    printf("\n");

    if (error_calculo)
        printf(" ERROR DE CALCULO");
}


/* =========================================================
   AGREGAR
   ========================================================= */

void agregar(const char *s)
{
    int len = strlen(s);

    if (cursor_expr + len < MAX_EXPR - 1)
    {
        strcat(expresion, s);

        cursor_expr += len;

        error_calculo = 0;

        actualizar();
    }
}


/* =========================================================
   BORRAR
   ========================================================= */

void borrar(void)
{
    if (cursor_expr > 0)
    {
        cursor_expr--;

        expresion[cursor_expr] = '\0';

        error_calculo = 0;

        actualizar();
    }
}


/* =========================================================
   AC
   ========================================================= */

void limpiar(void)
{
    expresion[0] = '\0';

    cursor_expr = 0;

    strcpy(resultado, "0");

    error_calculo = 0;

    actualizar();
}


/* =========================================================
   MATEMATICAS
   ========================================================= */

double deg2rad(double x)
{
    return x * M_PI / 180.0;
}


double factorial(double x)
{
    int n;

    if (x < 0.0 || x > 12.0)
        return NAN;

    n = (int)x;

    if (fabs(x - (double)n) > 0.0000001)
        return NAN;

    double r = 1.0;

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


/* =========================================================
   NUMERO
   ========================================================= */

double parse_numero(void)
{
    char *fin;
    double valor;

    saltar_espacios();

    if (strncmp(parser_ptr, "pi", 2) == 0)
    {
        parser_ptr += 2;
        return M_PI;
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


/* =========================================================
   PARENTESIS
   ========================================================= */

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


/* =========================================================
   FUNCIONES
   ========================================================= */

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


/* =========================================================
   POSTFIJO
   ========================================================= */

double parse_postfijo(void)
{
    double valor = parse_funcion();

    while (1)
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
/* =========================================================
   UNARIO
   ========================================================= */

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


/* =========================================================
   POTENCIA
   ========================================================= */

double parse_potencia(void)
{
    double izquierda;
    double derecha;

    izquierda = parse_unario();

    saltar_espacios();

    if (*parser_ptr == '^')
    {
        parser_ptr++;

        derecha = parse_potencia();

        izquierda = pow(izquierda, derecha);
    }

    return izquierda;
}


/* =========================================================
   MULTIPLICACION / DIVISION
   ========================================================= */

double parse_producto(void)
{
    double resultado_local;
    double valor;

    resultado_local = parse_potencia();

    while (!error_calculo)
    {
        saltar_espacios();

        if (*parser_ptr == '*')
        {
            parser_ptr++;

            valor = parse_potencia();

            resultado_local *= valor;
        }
        else if (*parser_ptr == '/')
        {
            parser_ptr++;

            valor = parse_potencia();

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


/* =========================================================
   SUMA / RESTA
   ========================================================= */

double parse_suma(void)
{
    double resultado_local;
    double valor;

    resultado_local = parse_producto();

    while (!error_calculo)
    {
        saltar_espacios();

        if (*parser_ptr == '+')
        {
            parser_ptr++;

            valor = parse_producto();

            resultado_local += valor;
        }
        else if (*parser_ptr == '-')
        {
            parser_ptr++;

            valor = parse_producto();

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
   CALCULAR
   ========================================================= */

void calcular(void)
{
    double r;

    if (expresion[0] == '\0')
    {
        strcpy(resultado, "0");

        actualizar();

        return;
    }

    error_calculo = 0;

    parser_ptr = expresion;

    r = parse_suma();

    saltar_espacios();

    if (*parser_ptr != '\0')
        error_calculo = 1;

    if (isnan(r) || isinf(r))
        error_calculo = 1;


    if (error_calculo)
    {
        strcpy(resultado, "ERROR");
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


    expresion[0] = '\0';

    cursor_expr = 0;

    actualizar();
}


/* =========================================================
   DIBUJAR BOTONES
   ========================================================= */

void dibujar_boton(int indice)
{
    Boton *b = &botones[indice];

    int col;
    int fila;

    /*
       Convertimos las coordenadas en caracteres.

       Cada botón ocupa aproximadamente 6 caracteres.
       Así quedan separados visualmente.
    */

    col = 1 + (b->x / 8);

    fila = 2 + (b->y / 8);


    /*
       Botón seleccionado:
       << NUM >>
    */

    if (indice == boton_seleccionado)
    {
        printf(
            "\x1b[%d;%dH<%s>",
            fila,
            col,
            b->label
        );
    }
    else
    {
        printf(
            "\x1b[%d;%dH %s ",
            fila,
            col,
            b->label
        );
    }
}


/* =========================================================
   DIBUJAR TECLADO COMPLETO
   ========================================================= */

void dibujar_teclado(void)
{
    consoleSelect(&bottomScreen);

    consoleClear();


    printf("\n");

    printf("   CALCULADORA CIENTIFICA");

    printf("\n\n");


    /*
       Dibujar los 30 botones.
    */

    for (int i = 0; i < NUM_BOTONES; i++)
    {
        dibujar_boton(i);
    }


    /*
       Información inferior.
    */

    printf("\x1b[22;1H");

    printf("CRUCETA: MOVER");

    printf("\x1b[23;1H");

    printf("START: PULSAR");

    printf("\x1b[24;1H");

    printf("A=CALC  B=DEL  X=AC");

    printf("\x1b[25;1H");

    printf("SELECT=SALIR");
}


/* =========================================================
   DETECTAR TOUCH
   ========================================================= */

int detectar_boton(int tx, int ty)
{
    for (int i = 0; i < NUM_BOTONES; i++)
    {
        Boton *b = &botones[i];

        /*
           La zona tactil es exactamente la zona
           del botón más un pequeño margen.
        */

        int margen = 2;

        if (tx >= b->x - margen &&
            tx < b->x + b->w + margen &&
            ty >= b->y - margen &&
            ty < b->y + b->h + margen)
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
    if (indice < 0 ||
        indice >= NUM_BOTONES)
        return;


    Boton *b = &botones[indice];


    if (b->tipo == 1)
    {
        limpiar();
    }
    else if (b->tipo == 2)
    {
        borrar();
    }
    else if (b->tipo == 3)
    {
        calcular();
    }
    else
    {
        if (b->insert != NULL &&
            b->insert[0] != '\0')
        {
            agregar(b->insert);
        }
    }
}


/* =========================================================
   MOVER SELECCION
   ========================================================= */

void mover_arriba(void)
{
    int fila = boton_seleccionado / 5;
    int col  = boton_seleccionado % 5;

    if (fila > 0)
        fila--;
    else
        fila = 5;

    boton_seleccionado =
        fila * 5 + col;
}


void mover_abajo(void)
{
    int fila = boton_seleccionado / 5;
    int col  = boton_seleccionado % 5;

    if (fila < 5)
        fila++;
    else
        fila = 0;

    boton_seleccionado =
        fila * 5 + col;
}


void mover_izquierda(void)
{
    int fila = boton_seleccionado / 5;
    int col  = boton_seleccionado % 5;

    if (col > 0)
        col--;
    else
        col = 4;

    boton_seleccionado =
        fila * 5 + col;
}


void mover_derecha(void)
{
    int fila = boton_seleccionado / 5;
    int col  = boton_seleccionado % 5;

    if (col < 4)
        col++;
    else
        col = 0;

    boton_seleccionado =
        fila * 5 + col;
}
/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    /*
       -----------------------------------------------------
       VIDEO
       -----------------------------------------------------
    */

    videoSetMode(MODE_0_2D);

    videoSetModeSub(MODE_0_2D);


    vramSetBankA(VRAM_A_MAIN_BG);

    vramSetBankC(VRAM_C_SUB_BG);


    /*
       -----------------------------------------------------
       PANTALLA SUPERIOR
       -----------------------------------------------------
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
       -----------------------------------------------------
       PANTALLA INFERIOR
       -----------------------------------------------------
    */

    consoleInit(
        &bottomScreen,
        3,
        BgType_Text4bpp,
        BgSize_T_256x256,
        31,
        0,
        false,
        true
    );


    /*
       -----------------------------------------------------
       ESTADO INICIAL
       -----------------------------------------------------
    */

    expresion[0] = '\0';

    cursor_expr = 0;

    ans = 0.0;

    error_calculo = 0;

    /*
       Empieza seleccionado el 7.
    */

    boton_seleccionado = 10;

    strcpy(resultado, "0");


    actualizar();

    dibujar_teclado();


    /*
       -----------------------------------------------------
       TOUCH
       -----------------------------------------------------
    */

    touchPosition touch;


    /*
       -----------------------------------------------------
       BUCLE PRINCIPAL
       -----------------------------------------------------
    */

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        u32 keys = keysDown();


        /* ================================================
           CRUCETA ARRIBA
           ================================================ */

        if (keys & KEY_UP)
        {
            mover_arriba();

            dibujar_teclado();
        }


        /* ================================================
           CRUCETA ABAJO
           ================================================ */

        if (keys & KEY_DOWN)
        {
            mover_abajo();

            dibujar_teclado();
        }


        /* ================================================
           CRUCETA IZQUIERDA
           ================================================ */

        if (keys & KEY_LEFT)
        {
            mover_izquierda();

            dibujar_teclado();
        }


        /* ================================================
           CRUCETA DERECHA
           ================================================ */

        if (keys & KEY_RIGHT)
        {
            mover_derecha();

            dibujar_teclado();
        }


        /* ================================================
           START = PULSAR BOTON SELECCIONADO
           ================================================ */

        if (keys & KEY_START)
        {
            procesar_boton(
                boton_seleccionado
            );

            dibujar_teclado();
        }


        /* ================================================
           A = CALCULAR
           ================================================ */

        if (keys & KEY_A)
        {
            calcular();

            dibujar_teclado();
        }


        /* ================================================
           B = BORRAR
           ================================================ */

        if (keys & KEY_B)
        {
            borrar();

            dibujar_teclado();
        }


        /* ================================================
           X = AC
           ================================================ */

        if (keys & KEY_X)
        {
            limpiar();

            dibujar_teclado();
        }


        /* ================================================
           SELECT = SALIR
           ================================================ */

        if (keys & KEY_SELECT)
        {
            break;
        }


        /* ================================================
           TOUCH
           ================================================ */

        if (keys & KEY_TOUCH)
        {
            touchRead(&touch);


            int indice =
                detectar_boton(
                    touch.px,
                    touch.py
                );


            if (indice >= 0)
            {
                /*
                   Al tocar un botón también se mueve
                   el selector a ese botón.
                */

                boton_seleccionado = indice;


                procesar_boton(indice);


                dibujar_teclado();
            }
        }
    }


    return 0;
}
