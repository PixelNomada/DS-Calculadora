#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_EXPR 48

char expresion[MAX_EXPR];
char resultado[64];

int cursor = 0;
double ultimo_resultado = 0.0;

PrintConsole topScreen;
PrintConsole bottomScreen;


/* =========================================================
   PANTALLA SUPERIOR
   ========================================================= */

void actualizar(void) {

    consoleSelect(&topScreen);
    consoleClear();

    printf("\n");
    printf("  DS CALCULADORA CIENTIFICA\n");
    printf("  -------------------------\n\n");

    printf("  EXPRESION:\n");
    printf("  %s\n\n", expresion[0] ? expresion : "0");

    printf("  RESULTADO:\n");
    printf("  = %s\n\n", resultado);

    printf("  ANGULO: GRADOS\n");
}


/* =========================================================
   AGREGAR
   ========================================================= */

void agregar(const char *texto) {

    int len = strlen(texto);

    if (cursor + len < MAX_EXPR) {

        strcat(expresion, texto);
        cursor += len;

        strcpy(resultado, expresion);

        actualizar();
    }
}


/* =========================================================
   BORRAR
   ========================================================= */

void borrar(void) {

    if (cursor > 0) {

        cursor--;

        expresion[cursor] = '\0';

        if (cursor == 0)
            strcpy(resultado, "0");
        else
            strcpy(resultado, expresion);

        actualizar();
    }
}


/* =========================================================
   LIMPIAR
   ========================================================= */

void limpiar(void) {

    expresion[0] = '\0';

    cursor = 0;

    ultimo_resultado = 0.0;

    strcpy(resultado, "0");

    actualizar();
}


/* =========================================================
   GRADOS A RADIANES
   ========================================================= */

double deg2rad(double grados) {

    return grados * M_PI / 180.0;
}


/* =========================================================
   PARSER
   ========================================================= */

const char *posicion;
int error_calculo;


/* ---------------------------------------------------------
   Saltar espacios
   --------------------------------------------------------- */

void espacios(void) {

    while (*posicion == ' ' ||
           *posicion == '\t') {

        posicion++;
    }
}


/* ---------------------------------------------------------
   Declaraciones
   --------------------------------------------------------- */

double parse_suma(void);
double parse_producto(void);
double parse_potencia(void);
double parse_unario(void);
double parse_postfijo(void);
double parse_primario(void);


/* =========================================================
   PRIMARIO
   ========================================================= */

double parse_primario(void) {

    espacios();


    /* -----------------------------------------
       Parentesis
       ----------------------------------------- */

    if (*posicion == '(') {

        double valor;

        posicion++;

        valor = parse_suma();

        espacios();

        if (*posicion == ')') {

            posicion++;

        } else {

            error_calculo = 1;
        }

        return valor;
    }


    /* -----------------------------------------
       Numero
       ----------------------------------------- */

    if ((*posicion >= '0' &&
         *posicion <= '9') ||
        *posicion == '.') {

        char *fin;

        double valor = strtod(posicion, &fin);

        if (fin == posicion) {

            error_calculo = 1;

            return 0.0;
        }

        posicion = fin;

        return valor;
    }


    /* -----------------------------------------
       PI
       ----------------------------------------- */

    if (strncmp(posicion, "pi", 2) == 0) {

        posicion += 2;

        return M_PI;
    }


    /* -----------------------------------------
       ANS
       ----------------------------------------- */

    if (strncmp(posicion, "ans", 3) == 0) {

        posicion += 3;

        return ultimo_resultado;
    }


    /* -----------------------------------------
       SIN
       ----------------------------------------- */

    if (strncmp(posicion, "sin", 3) == 0) {

        double valor;

        posicion += 3;

        espacios();

        if (*posicion == '(') {

            posicion++;

            valor = parse_suma();

            espacios();

            if (*posicion == ')')
                posicion++;
            else
                error_calculo = 1;

        } else {

            valor = parse_unario();
        }

        return sin(deg2rad(valor));
    }


    /* -----------------------------------------
       COS
       ----------------------------------------- */

    if (strncmp(posicion, "cos", 3) == 0) {

        double valor;

        posicion += 3;

        espacios();

        if (*posicion == '(') {

            posicion++;

            valor = parse_suma();

            espacios();

            if (*posicion == ')')
                posicion++;
            else
                error_calculo = 1;

        } else {

            valor = parse_unario();
        }

        return cos(deg2rad(valor));
    }


    /* -----------------------------------------
       TAN
       ----------------------------------------- */

    if (strncmp(posicion, "tan", 3) == 0) {

        double valor;

        posicion += 3;

        espacios();

        if (*posicion == '(') {

            posicion++;

            valor = parse_suma();

            espacios();

            if (*posicion == ')')
                posicion++;
            else
                error_calculo = 1;

        } else {

            valor = parse_unario();
        }

        return tan(deg2rad(valor));
    }


    /* -----------------------------------------
       SQRT
       ----------------------------------------- */

    if (strncmp(posicion, "sqrt", 4) == 0) {

        double valor;

        posicion += 4;

        espacios();

        if (*posicion == '(') {

            posicion++;

            valor = parse_suma();

            espacios();

            if (*posicion == ')')
                posicion++;
            else
                error_calculo = 1;

        } else {

            valor = parse_unario();
        }

        if (valor < 0.0) {

            error_calculo = 1;

            return 0.0;
        }

        return sqrt(valor);
    }


    /* -----------------------------------------
       LOG
       ----------------------------------------- */

    if (strncmp(posicion, "log", 3) == 0) {

        double valor;

        posicion += 3;

        espacios();

        if (*posicion == '(') {

            posicion++;

            valor = parse_suma();

            espacios();

            if (*posicion == ')')
                posicion++;
            else
                error_calculo = 1;

        } else {

            valor = parse_unario();
        }

        if (valor <= 0.0) {

            error_calculo = 1;

            return 0.0;
        }

        return log10(valor);
    }


    /* -----------------------------------------
       LN
       ----------------------------------------- */

    if (strncmp(posicion, "ln", 2) == 0) {

        double valor;

        posicion += 2;

        espacios();

        if (*posicion == '(') {

            posicion++;

            valor = parse_suma();

            espacios();

            if (*posicion == ')')
                posicion++;
            else
                error_calculo = 1;

        } else {

            valor = parse_unario();
        }

        if (valor <= 0.0) {

            error_calculo = 1;

            return 0.0;
        }

        return log(valor);
    }


    error_calculo = 1;

    return 0.0;
}


/* =========================================================
   POSTFIJO
   ========================================================= */

double parse_postfijo(void) {

    double valor = parse_primario();

    while (!error_calculo) {

        espacios();

        /* Factorial */

        if (*posicion == '!') {

            posicion++;

            if (valor < 0.0 ||
                valor > 170.0 ||
                floor(valor) != valor) {

                error_calculo = 1;

                return 0.0;
            }

            double factorial = 1.0;

            for (int i = 2; i <= (int)valor; i++)
                factorial *= i;

            valor = factorial;
        }

        /* Porcentaje */

        else if (*posicion == '%') {

            posicion++;

            valor /= 100.0;
        }

        else {

            break;
        }
    }

    return valor;
}


/* =========================================================
   UNARIO
   ========================================================= */

double parse_unario(void) {

    espacios();

    if (*posicion == '+') {

        posicion++;

        return parse_unario();
    }

    if (*posicion == '-') {

        posicion++;

        return -parse_unario();
    }

    return parse_postfijo();
}


/* =========================================================
   POTENCIAS
   ========================================================= */

double parse_potencia(void) {

    double izquierda;

    izquierda = parse_unario();

    espacios();

    if (*posicion == '^') {

        double derecha;

        posicion++;

        derecha = parse_potencia();

        izquierda = pow(izquierda, derecha);
    }

    return izquierda;
}


/* =========================================================
   MULTIPLICACION Y DIVISION
   ========================================================= */

double parse_producto(void) {

    double valor;

    valor = parse_potencia();

    while (!error_calculo) {

        espacios();

        if (*posicion == '*') {

            posicion++;

            valor *= parse_potencia();
        }

        else if (*posicion == '/') {

            double divisor;

            posicion++;

            divisor = parse_potencia();

            if (fabs(divisor) < 0.000000000001) {

                error_calculo = 1;

                return 0.0;
            }

            valor /= divisor;
        }

        else {

            break;
        }
    }

    return valor;
}


/* =========================================================
   SUMA Y RESTA
   ========================================================= */

double parse_suma(void) {

    double valor;

    valor = parse_producto();

    while (!error_calculo) {

        espacios();

        if (*posicion == '+') {

            posicion++;

            valor += parse_producto();
        }

        else if (*posicion == '-') {

            posicion++;

            valor -= parse_producto();
        }

        else {

            break;
        }
    }

    return valor;
}


/* =========================================================
   EVALUAR
   ========================================================= */

double evaluar(const char *texto) {

    double valor;

    posicion = texto;

    error_calculo = 0;

    valor = parse_suma();

    espacios();

    if (*posicion != '\0')
        error_calculo = 1;

    if (isnan(valor) ||
        isinf(valor))
        error_calculo = 1;

    return valor;
}


/* =========================================================
   CALCULAR
   ========================================================= */

void calcular(void) {

    double valor;

    if (expresion[0] == '\0') {

        strcpy(resultado, "0");

        actualizar();

        return;
    }


    valor = evaluar(expresion);


    if (error_calculo) {

        strcpy(resultado, "ERROR");

        actualizar();

        return;
    }


    ultimo_resultado = valor;


    snprintf(
        resultado,
        63,
        "%.10g",
        valor
    );


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
    tipo 0 = normal
    tipo 1 = AC
    tipo 2 = DEL
    tipo 3 = =
*/


Boton botones[] = {

    /* FUNCIONES */

    {   4,   6, 46, 24, "sin",  "sin(",   0 },
    {  54,   6, 46, 24, "cos",  "cos(",   0 },
    { 104,   6, 46, 24, "tan",  "tan(",   0 },
    { 154,   6, 46, 24, "log",  "log(",   0 },
    { 204,   6, 48, 24, "ln",   "ln(",    0 },


    /* CIENTIFICAS */

    {   4,  34, 46, 24, "x^2",  "^2",     0 },
    {  54,  34, 46, 24, "sqrt", "sqrt(",  0 },
    { 104,  34, 46, 24, "^",    "^",      0 },
    { 154,  34, 46, 24, "(",    "(",      0 },
    { 204,  34, 48, 24, ")",    ")",      0 },


    /* 7 8 9 */

    {   4,  62, 46, 24, "[7]",  "7",      0 },
    {  54,  62, 46, 24, "[8]",  "8",      0 },
    { 104,  62, 46, 24, "[9]",  "9",      0 },

    { 154,  62, 46, 24, "DEL",  "",       2 },
    { 204,  62, 48, 24, "AC",   "",       1 },


    /* 4 5 6 */

    {   4,  90, 46, 24, "[4]",  "4",      0 },
    {  54,  90, 46, 24, "[5]",  "5",      0 },
    { 104,  90, 46, 24, "[6]",  "6",      0 },

    { 154,  90, 46, 24, "x",    "*",      0 },
    { 204,  90, 48, 24, "/",    "/",      0 },


    /* 1 2 3 */

    {   4, 118, 46, 24, "[1]",  "1",      0 },
    {  54, 118, 46, 24, "[2]",  "2",      0 },
    { 104, 118, 46, 24, "[3]",  "3",      0 },

    { 154, 118, 46, 24, "+",    "+",      0 },
    { 204, 118, 48, 24, "-",    "-",      0 },


    /* 0 . PI = */

    {   4, 146, 46, 24, "[0]",  "0",      0 },
    {  54, 146, 46, 24, ".",    ".",      0 },
    { 104, 146, 46, 24, "pi",   "pi",     0 },

    { 154, 146, 98, 24, "=",    "",       3 }
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
    printf("  CALCULADORA\n");
    printf("  -----------\n");


    for (int i = 0; i < NUM_BOTONES; i++) {

        Boton *b = &botones[i];

        int fila;
        int columna;

        fila = (b->y / 14) + 1;
        columna = (b->x / 8) + 1;

        printf(
            "\x1b[%d;%dH%s",
            fila,
            columna,
            b->label
        );
    }


    printf("\x1b[20;1H");

    printf("A=CALC B=DEL X=AC");
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

void procesar_boton(int indice) {

    Boton *b;


    if (indice < 0)
        return;


    b = &botones[indice];


    if (b->tipo == 1) {

        limpiar();

        return;
    }


    if (b->tipo == 2) {

        borrar();

        return;
    }


    if (b->tipo == 3) {

        calcular();

        return;
    }


    if (b->insert != NULL &&
        b->insert[0] != '\0') {

        agregar(b->insert);
    }
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void) {

    videoSetMode(MODE_0_2D);

    videoSetModeSub(MODE_0_2D);


    vramSetBankA(VRAM_A_MAIN_BG);

    vramSetBankC(VRAM_C_SUB_BG);


    /* PANTALLA SUPERIOR */

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


    /* PANTALLA INFERIOR */

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


    /* ESTADO INICIAL */

    expresion[0] = '\0';

    cursor = 0;

    ultimo_resultado = 0.0;

    strcpy(resultado, "0");


    actualizar();

    dibujar_teclado();


    touchPosition touch;


    /* =====================================================
       BUCLE PRINCIPAL
       ===================================================== */

    while (1) {

        swiWaitForVBlank();

        scanKeys();

        u32 keys = keysDown();


        /* TOUCH */

        if (keys & KEY_TOUCH) {

            touchRead(&touch);


            int boton;

            boton = detectar_boton(
                touch.px,
                touch.py
            );


            if (boton >= 0) {

                procesar_boton(boton);

                dibujar_teclado();
            }
        }


        /* A = CALCULAR */

        if (keys & KEY_A) {

            calcular();

            dibujar_teclado();
        }


        /* B = BORRAR */

        if (keys & KEY_B) {

            borrar();

            dibujar_teclado();
        }


        /* X = LIMPIAR */

        if (keys & KEY_X) {

            limpiar();

            dibujar_teclado();
        }


        /* START = SALIR */

        if (keys & KEY_START) {

            break;
        }
    }


    return 0;
}
