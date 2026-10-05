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
   EDICION DE EXPRESION
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
   UTILIDADES MATEMATICAS
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

/* ---------------------------------------------------------
   Saltar espacios
   --------------------------------------------------------- */

void saltar_espacios(void) {
    while (*parser_ptr == ' ' ||
           *parser_ptr == '\t') {
        parser_ptr++;
    }
}

/* ---------------------------------------------------------
   Numero
   --------------------------------------------------------- */

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

/* ---------------------------------------------------------
   Primario
   --------------------------------------------------------- */

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

/* ---------------------------------------------------------
   Funciones cientificas
   --------------------------------------------------------- */

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

/* ---------------------------------------------------------
   Factorial y porcentaje
   --------------------------------------------------------- */

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

/* ---------------------------------------------------------
   Signos + y -
   --------------------------------------------------------- */

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

/* ---------------------------------------------------------
   Potencias
   --------------------------------------------------------- */

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

/* ---------------------------------------------------------
   Multiplicacion / division
   --------------------------------------------------------- */

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

/* ---------------------------------------------------------
   Suma / resta
   --------------------------------------------------------- */

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

    /* Si quedaron caracteres sin interpretar */
    if (*parser_ptr != '\0')
        error_calculo = 1;

    /* Comprobar resultados invalidos */
    if (isnan(r) || isinf(r))
        error_calculo = 1;

    if (error_calculo) {
        strcpy(resultado, "ERROR");
    } else {
        ans = r;
        snprintf(resultado, sizeof(resultado), "%.10g", r);
    }

    /* La calculadora normal conserva el resultado */
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
   tipo:
   0 = normal
   1 = AC
   2 = DEL
   3 = =
*/

Boton botones[] = {

    /* Fila 1 */
    {  4,   4, 46, 26, "sin",  "sin(",  0 },
    { 54,   4, 46, 26, "cos",  "cos(",  0 },
    {104,   4, 46, 26, "tan",  "tan(",  0 },
    {154,   4, 46, 26, "log",  "log(",  0 },
    {204,   4, 48, 26, "ln",   "ln(",   0 },

    /* Fila 2 */
    {  4,  34, 46, 26, "x^2",  "^2",    0 },
    { 54,  34, 46, 26, "sqrt", "sqrt(", 0 },
    {104,  34, 46, 26, "^",    "^",     0 },
    {154,  34, 46, 26, "(",    "(",     0 },
    {204,  34, 48, 26, ")",    ")",     0 },

    /* Fila 3 */
    {  4,  64, 46, 26, "7",    "7",     0 },
    { 54,  64, 46, 26, "8",    "8",     0 },
    {104,  64, 46, 26, "9",    "9",     0 },
    {154,  64, 46, 26, "DEL",  "",      2 },
    {204,  64, 48, 26, "AC",   "",      1 },

    /* Fila 4 */
    {  4,  94, 46, 26, "4",    "4",     0 },
    { 54,  94, 46, 26, "5",    "5",     0 },
    {104,  94, 46, 26, "6",    "6",     0 },
    {154,  94, 46, 26, "x",    "*",     0 },
    {204,  94, 48, 26, "/",    "/",     0 },

    /* Fila 5 */
    {  4, 124, 46, 26, "1",    "1",     0 },
    { 54, 124, 46, 26, "2",    "2",     0 },
    {104, 124, 46, 26, "3",    "3",     0 },
    {154, 124, 46, 26, "+",    "+",     0 },
    {204, 124, 48, 26, "-",    "-",     0 },

    /* Fila 6 */
    {  4, 154, 46, 26, "0",    "0",     0 },
    { 54, 154, 46, 26, ".",    ".",     0 },
    {104, 154, 46, 26, "pi",   "pi",    0 },
    {154, 154, 46, 26, "!",    "!",     0 },
    {204, 154, 48, 26, "=",    "",      3 }
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
    printf("        CALCULADORA\n");
    printf("  ---------------------------\n\n");

    for (int i = 0; i < NUM_BOTONES; i++) {

        Boton *b = &botones[i];

        /*
           Convertimos las coordenadas de los botones
           a posiciones de la consola.
        */

        int fila = 5 + (b->y / 30);
        int col  = 1 + (b->x / 8);

        printf("\x1b[%d;%dH%s",
               fila,
               col,
               b->label);
    }

    printf("\n\n");
    printf(" A = CALCULAR\n");
    printf(" B = BORRAR\n");
    printf(" X = AC\n");
    printf(" START = SALIR");
}


/* =========================================================
   DETECTAR BOTON
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
            /* AC */
            limpiar();
            break;

        case 2:
            /* DEL */
            borrar();
            break;

        case 3:
            /* = */
            calcular();
            break;

        default:
            /* Boton normal */
            if (b->insert != NULL &&
                b->insert[0] != '\0') {

                agregar(b->insert);
            }
            break;
    }
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
       CONSOLAS
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

    strcpy(resultado, "0");


    /* -----------------------------------------------------
       MOSTRAR INTERFAZ
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


        /* =================================================
           PANTALLA TACTIL
           ================================================= */

        if (keys & KEY_TOUCH) {

            touchRead(&touch);

            int idx =
                detectar_boton(
                    touch.px,
                    touch.py
                );

            if (idx >= 0) {

                procesar_boton(idx);

                /*
                   Redibujar teclado por si la pantalla
                   fue limpiada al actualizar.
                */

                dibujar_teclado();
            }
        }


        /* =================================================
           BOTONES FISICOS
           ================================================= */

        if (keys & KEY_A) {

            calcular();
        }


        if (keys & KEY_B) {

            borrar();
        }


        if (keys & KEY_X) {

            limpiar();
        }


        if (keys & KEY_START) {

            break;
        }
    }


    /* -----------------------------------------------------
       SALIR
       ----------------------------------------------------- */

    return 0;
}
