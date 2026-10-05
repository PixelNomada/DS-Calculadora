#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_EXPR 96

char expresion[MAX_EXPR];
char resultado[64];

int cursor = 0;
double ans = 0.0;
int error_calculo = 0;

PrintConsole topScreen;
PrintConsole bottomScreen;

/* Boton actualmente seleccionado con la cruceta */
int boton_seleccionado = 10;


/* =========================================================
   PANTALLA SUPERIOR
   ========================================================= */

void actualizar(void) {
    consoleSelect(&topScreen);
    consoleClear();

    printf("\n");
    printf("   DS SCIENTIFIC CALCULATOR\n");
    printf("   ========================\n\n");

    printf("   EXPRESION:\n");
    printf("   %s\n\n", expresion[0] ? expresion : "0");

    printf("   RESULTADO:\n");
    printf("   = %s\n\n", resultado);

    printf("   ANS = %.10g\n", ans);
}


/* =========================================================
   EDICION
   ========================================================= */

void agregar(const char *s) {
    int len = strlen(s);

    if (cursor + len < MAX_EXPR - 1) {
        strcat(expresion, s);
        cursor += len;
        actualizar();
    }
}


void borrar(void) {
    if (cursor > 0) {
        cursor--;
        expresion[cursor] = '\0';
        actualizar();
    }
}


void limpiar(void) {
    expresion[0] = '\0';
    cursor = 0;

    strcpy(resultado, "0");

    error_calculo = 0;

    actualizar();
}


/* =========================================================
   MATEMATICAS
   ========================================================= */

double deg2rad(double x) {
    return x * M_PI / 180.0;
}


double factorial(double x) {

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


void saltar_espacios(void) {

    while (*parser_ptr == ' ' ||
           *parser_ptr == '\t') {

        parser_ptr++;
    }
}


/* =========================================================
   NUMEROS
   ========================================================= */

double parse_numero(void) {

    char *fin;
    double valor;

    saltar_espacios();

    if (strncmp(parser_ptr, "pi", 2) == 0) {

        parser_ptr += 2;

        return M_PI;
    }

    if (strncmp(parser_ptr, "ans", 3) == 0) {

        parser_ptr += 3;

        return ans;
    }

    valor = strtod(parser_ptr, &fin);

    if (fin == parser_ptr) {

        error_calculo = 1;

        return 0.0;
    }

    parser_ptr = fin;

    return valor;
}


/* =========================================================
   PARENTESIS
   ========================================================= */

double parse_primario(void) {

    double valor;

    saltar_espacios();

    if (*parser_ptr == '(') {

        parser_ptr++;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')') {

            error_calculo = 1;

            return 0.0;
        }

        parser_ptr++;

        return valor;
    }

    return parse_numero();
}


/* =========================================================
   FUNCIONES CIENTIFICAS
   ========================================================= */

double parse_funcion(void) {

    double valor;

    saltar_espacios();


    if (strncmp(parser_ptr, "sqrt(", 5) == 0) {

        parser_ptr += 5;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')') {

            error_calculo = 1;

            return 0.0;
        }

        parser_ptr++;

        if (valor < 0.0) {

            error_calculo = 1;

            return 0.0;
        }

        return sqrt(valor);
    }


    if (strncmp(parser_ptr, "sin(", 4) == 0) {

        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')') {

            error_calculo = 1;

            return 0.0;
        }

        parser_ptr++;

        return sin(deg2rad(valor));
    }


    if (strncmp(parser_ptr, "cos(", 4) == 0) {

        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')') {

            error_calculo = 1;

            return 0.0;
        }

        parser_ptr++;

        return cos(deg2rad(valor));
    }


    if (strncmp(parser_ptr, "tan(", 4) == 0) {

        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')') {

            error_calculo = 1;

            return 0.0;
        }

        parser_ptr++;

        return tan(deg2rad(valor));
    }


    if (strncmp(parser_ptr, "log(", 4) == 0) {

        parser_ptr += 4;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')') {

            error_calculo = 1;

            return 0.0;
        }

        parser_ptr++;

        if (valor <= 0.0) {

            error_calculo = 1;

            return 0.0;
        }

        return log10(valor);
    }


    if (strncmp(parser_ptr, "ln(", 3) == 0) {

        parser_ptr += 3;

        valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr != ')') {

            error_calculo = 1;

            return 0.0;
        }

        parser_ptr++;

        if (valor <= 0.0) {

            error_calculo = 1;

            return 0.0;
        }

        return log(valor);
    }


    return parse_primario();
}


/* =========================================================
   FACTORIAL / PORCENTAJE
   ========================================================= */

double parse_postfijo(void) {

    double valor = parse_funcion();

    while (1) {

        saltar_espacios();

        if (*parser_ptr == '!') {

            parser_ptr++;

            valor = factorial(valor);

            if (isnan(valor)) {

                error_calculo = 1;

                return 0.0;
            }
        }

        else if (*parser_ptr == '%') {

            parser_ptr++;

            valor /= 100.0;
        }

        else {

            break;
        }
    }

    return valor;
}


/* =========================================================
   SIGNOS
   ========================================================= */

double parse_unario(void) {

    saltar_espacios();

    if (*parser_ptr == '+') {

        parser_ptr++;

        return parse_unario();
    }

    if (*parser_ptr == '-') {

        parser_ptr++;

        return -parse_unario();
    }

    return parse_postfijo();
}


/* =========================================================
   POTENCIA
   ========================================================= */

double parse_potencia(void) {

    double izquierda;
    double derecha;

    izquierda = parse_unario();

    saltar_espacios();

    if (*parser_ptr == '^') {

        parser_ptr++;

        derecha = parse_potencia();

        izquierda = pow(izquierda, derecha);
    }

    return izquierda;
}


/* =========================================================
   MULTIPLICACION / DIVISION
   ========================================================= */

double parse_producto(void) {

    double resultado_local;
    double valor;

    resultado_local = parse_potencia();

    while (!error_calculo) {

        saltar_espacios();

        if (*parser_ptr == '*') {

            parser_ptr++;

            valor = parse_potencia();

            resultado_local *= valor;
        }

        else if (*parser_ptr == '/') {

            parser_ptr++;

            valor = parse_potencia();

            if (fabs(valor) < 0.000000000001) {

                error_calculo = 1;

                return 0.0;
            }

            resultado_local /= valor;
        }

        else {

            break;
        }
    }

    return resultado_local;
}


/* =========================================================
   SUMA / RESTA
   ========================================================= */

double parse_suma(void) {

    double resultado_local;
    double valor;

    resultado_local = parse_producto();

    while (!error_calculo) {

        saltar_espacios();

        if (*parser_ptr == '+') {

            parser_ptr++;

            valor = parse_producto();

            resultado_local += valor;
        }

        else if (*parser_ptr == '-') {

            parser_ptr++;

            valor = parse_producto();

            resultado_local -= valor;
        }

        else {

            break;
        }
    }

    return resultado_local;
}
/* =========================================================
   CALCULAR
   ========================================================= */

void calcular(void) {

    double r;

    if (expresion[0] == '\0') {

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


    if (error_calculo) {

        strcpy(resultado, "ERROR");
    }

    else {

        ans = r;

        snprintf(
            resultado,
            sizeof(resultado),
            "%.10g",
            r
        );
    }

    expresion[0] = '\0';

    cursor = 0;

    actualizar();
}


/* =========================================================
   BOTONES
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
   IMPORTANTE:

   Los botones estan organizados en 6 filas
   y 5 columnas.

   Esto permite que la cruceta pueda navegar
   perfectamente entre ellos.
*/

Boton botones[] = {

    /* FILA 0 */

    {  2,   4, 45, 26, "sin",  "sin(",  0 },
    { 53,   4, 45, 26, "cos",  "cos(",  0 },
    {104,   4, 45, 26, "tan",  "tan(",  0 },
    {155,   4, 45, 26, "log",  "log(",  0 },
    {206,   4, 45, 26, "ln",   "ln(",   0 },


    /* FILA 1 */

    {  2,  34, 45, 26, "x^2",  "^2",    0 },
    { 53,  34, 45, 26, "sqrt", "sqrt(", 0 },
    {104,  34, 45, 26, "^",    "^",     0 },
    {155,  34, 45, 26, "(",    "(",     0 },
    {206,  34, 45, 26, ")",    ")",     0 },


    /* FILA 2 */

    {  2,  64, 45, 28, "7",   "7",   0 },
    { 53,  64, 45, 28, "8",   "8",   0 },
    {104,  64, 45, 28, "9",   "9",   0 },
    {155,  64, 45, 28, "DEL", "",    2 },
    {206,  64, 45, 28, "AC",  "",    1 },


    /* FILA 3 */

    {  2,  96, 45, 28, "4", "4", 0 },
    { 53,  96, 45, 28, "5", "5", 0 },
    {104,  96, 45, 28, "6", "6", 0 },
    {155,  96, 45, 28, "x", "*", 0 },
    {206,  96, 45, 28, "/", "/", 0 },


    /* FILA 4 */

    {  2, 128, 45, 28, "1", "1", 0 },
    { 53, 128, 45, 28, "2", "2", 0 },
    {104, 128, 45, 28, "3", "3", 0 },
    {155, 128, 45, 28, "+", "+", 0 },
    {206, 128, 45, 28, "-", "-", 0 },


    /* FILA 5 */

    {  2, 160, 45, 28, "0",  "0",  0 },
    { 53, 160, 45, 28, ".",  ".",  0 },
    {104, 160, 45, 28, "pi", "pi", 0 },
    {155, 160, 45, 28, "!",  "!",  0 },
    {206, 160, 45, 28, "=",  "",   3 }
};


#define NUM_BOTONES \
    (sizeof(botones) / sizeof(Boton))


/* =========================================================
   DIBUJAR TECLADO
   ========================================================= */

void dibujar_teclado(void) {

    consoleSelect(&bottomScreen);

    consoleClear();

    printf("\n");

    printf("       CALCULADORA\n");

    printf("  ---------------------------\n\n");


    for (int i = 0; i < NUM_BOTONES; i++) {

        Boton *b = &botones[i];

        int fila = 5 + (b->y / 30);

        int col = 1 + (b->x / 8);


        /*
           El boton seleccionado por la cruceta
           aparece con una flecha.
        */

        if (i == boton_seleccionado) {

            printf(
                "\x1b[%d;%dH>%s",
                fila,
                col,
                b->label
            );
        }

        else {

            printf(
                "\x1b[%d;%dH %s",
                fila,
                col,
                b->label
            );
        }
    }


    printf("\n\n");

    printf(" CRUCETA = MOVER\n");
    printf(" START = SELECCIONAR\n");
    printf(" A = CALCULAR\n");
    printf(" B = BORRAR\n");
    printf(" X = AC\n");
    printf(" SELECT = SALIR");
}


/* =========================================================
   DETECTAR BOTON TACTIL
   ========================================================= */

int detectar_boton(int tx, int ty) {

    for (int i = 0; i < NUM_BOTONES; i++) {

        Boton *b = &botones[i];

        if (tx >= b->x &&
            tx < b->x + b->w &&
            ty >= b->y &&
            ty < b->y + b->h) {

            return i;
        }
    }

    return -1;
}


/* =========================================================
   PROCESAR BOTON
   ========================================================= */

void procesar_boton(int idx) {

    if (idx < 0)
        return;


    Boton *b = &botones[idx];


    switch (b->tipo) {

        case 1:

            limpiar();

            break;


        case 2:

            borrar();

            break;


        case 3:

            calcular();

            break;


        default:

            if (b->insert != NULL &&
                b->insert[0] != '\0') {

                agregar(b->insert);
            }

            break;
    }
}


/* =========================================================
   NAVEGACION CON CRUCETA
   ========================================================= */

void mover_arriba(void) {

    int fila = boton_seleccionado / 5;
    int col  = boton_seleccionado % 5;


    if (fila > 0)
        fila--;
    else
        fila = 5;


    boton_seleccionado =
        fila * 5 + col;
}


void mover_abajo(void) {

    int fila = boton_seleccionado / 5;
    int col  = boton_seleccionado % 5;


    if (fila < 5)
        fila++;
    else
        fila = 0;


    boton_seleccionado =
        fila * 5 + col;
}


void mover_izquierda(void) {

    int fila = boton_seleccionado / 5;
    int col  = boton_seleccionado % 5;


    if (col > 0)
        col--;
    else
        col = 4;


    boton_seleccionado =
        fila * 5 + col;
}


void mover_derecha(void) {

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

int main(void) {

    /* -----------------------------------------------------
       VIDEO
       ----------------------------------------------------- */

    videoSetMode(MODE_0_2D);

    videoSetModeSub(MODE_0_2D);


    vramSetBankA(VRAM_A_MAIN_BG);

    vramSetBankC(VRAM_C_SUB_BG);


    /* -----------------------------------------------------
       CONSOLA SUPERIOR
       ----------------------------------------------------- */

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


    /* -----------------------------------------------------
       CONSOLA INFERIOR
       ----------------------------------------------------- */

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


    /* -----------------------------------------------------
       ESTADO INICIAL
       ----------------------------------------------------- */

    expresion[0] = '\0';

    cursor = 0;

    ans = 0.0;

    error_calculo = 0;

    boton_seleccionado = 10;

    strcpy(resultado, "0");


    /* -----------------------------------------------------
       MOSTRAR
       ----------------------------------------------------- */

    actualizar();

    dibujar_teclado();


    /* -----------------------------------------------------
       TOUCH
       ----------------------------------------------------- */

    touchPosition touch;


    /* -----------------------------------------------------
       BUCLE PRINCIPAL
       ----------------------------------------------------- */

    while (1) {

        swiWaitForVBlank();

        scanKeys();

        u32 keys = keysDown();


        /* ================================================
           CRUCETA ARRIBA
           ================================================ */

        if (keys & KEY_UP) {

            mover_arriba();

            dibujar_teclado();
        }


        /* ================================================
           CRUCETA ABAJO
           ================================================ */

        if (keys & KEY_DOWN) {

            mover_abajo();

            dibujar_teclado();
        }


        /* ================================================
           CRUCETA IZQUIERDA
           ================================================ */

        if (keys & KEY_LEFT) {

            mover_izquierda();

            dibujar_teclado();
        }


        /* ================================================
           CRUCETA DERECHA
           ================================================ */

        if (keys & KEY_RIGHT) {

            mover_derecha();

            dibujar_teclado();
        }


        /* ================================================
           START = SELECCIONAR BOTON
           ================================================ */

        if (keys & KEY_START) {

            procesar_boton(
                boton_seleccionado
            );

            dibujar_teclado();
        }


        /* ================================================
           A = CALCULAR
           ================================================ */

        if (keys & KEY_A) {

            calcular();

            dibujar_teclado();
        }


        /* ================================================
           B = BORRAR
           ================================================ */

        if (keys & KEY_B) {

            borrar();

            dibujar_teclado();
        }


        /* ================================================
           X = AC
           ================================================ */

        if (keys & KEY_X) {

            limpiar();

            dibujar_teclado();
        }


        /* ================================================
           SELECT = SALIR
           ================================================ */

        if (keys & KEY_SELECT) {

            break;
        }


        /* ================================================
           PANTALLA TACTIL
           ================================================ */

        if (keys & KEY_TOUCH) {

            touchRead(&touch);


            int idx =
                detectar_boton(
                    touch.px,
                    touch.py
                );


            if (idx >= 0) {

                /*
                   Si tocas un boton, también hacemos
                   que ese sea el seleccionado.
                */

                boton_seleccionado = idx;


                procesar_boton(idx);


                dibujar_teclado();
            }
        }
    }


    /* -----------------------------------------------------
       SALIR
       ----------------------------------------------------- */

    return 0;
}
