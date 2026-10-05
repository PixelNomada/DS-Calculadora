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

PrintConsole topScreen;
PrintConsole bottomScreen;


/* =========================================================
   PANTALLA SUPERIOR
   ========================================================= */

void actualizar(void) {
    consoleSelect(&topScreen);
    consoleClear();

    printf("\n  DS SCIENTIFIC CALCULATOR\n");
    printf("  ------------------------\n\n");

    printf("  EXPRESION:\n");
    printf("  %s\n\n", expresion[0] ? expresion : "0");

    printf("  RESULTADO:\n");
    printf("  = %s\n", resultado);
}


/* =========================================================
   AGREGAR TEXTO
   ========================================================= */

void agregar(const char *s) {
    int len = strlen(s);

    if (cursor + len < MAX_EXPR) {
        strcat(expresion, s);
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

        expresion[cursor] = 0;

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
    expresion[0] = 0;
    cursor = 0;

    strcpy(resultado, "0");

    actualizar();
}


/* =========================================================
   GRADOS A RADIANES
   ========================================================= */

double deg2rad(double x) {
    return x * M_PI / 180.0;
}


/* =========================================================
   EVALUADOR DE EXPRESIONES
   ========================================================= */

double evaluar(const char *input) {

    char expr[128];

    strncpy(expr, input, 127);
    expr[127] = 0;


    /* Convertir a minusculas */

    for (int i = 0; expr[i]; i++) {
        expr[i] = tolower((unsigned char)expr[i]);
    }


    /* =====================================================
       CONSTANTE PI
       ===================================================== */

    if (strcmp(expr, "pi") == 0) {
        return M_PI;
    }


    /* =====================================================
       FUNCIONES CIENTIFICAS
       ===================================================== */

    if (strncmp(expr, "sin", 3) == 0) {
        return sin(deg2rad(atof(expr + 3)));
    }

    if (strncmp(expr, "cos", 3) == 0) {
        return cos(deg2rad(atof(expr + 3)));
    }

    if (strncmp(expr, "tan", 3) == 0) {
        return tan(deg2rad(atof(expr + 3)));
    }

    if (strncmp(expr, "log", 3) == 0) {
        return log10(atof(expr + 3));
    }

    if (strncmp(expr, "ln", 2) == 0) {
        return log(atof(expr + 2));
    }

    if (strncmp(expr, "sqrt", 4) == 0) {
        return sqrt(atof(expr + 4));
    }


    /* =====================================================
       POTENCIA
       ===================================================== */

    char *pot = strchr(expr, '^');

    if (pot != NULL) {

        *pot = 0;

        double base = atof(expr);
        double exponente = atof(pot + 1);

        return pow(base, exponente);
    }


    /* =====================================================
       OPERACIONES BASICAS
       ===================================================== */

    double numeros[32];
    char operaciones[32];

    int cantidadNumeros = 0;
    int cantidadOperaciones = 0;

    char num[32];
    int np = 0;

    num[0] = 0;


    for (int i = 0; ; i++) {

        char c = expr[i];


        /* Numero */

        if ((c >= '0' && c <= '9') ||
            c == '.' ||
            (c == '-' && np == 0 &&
             (i == 0 ||
              expr[i - 1] == '+' ||
              expr[i - 1] == '-' ||
              expr[i - 1] == '*' ||
              expr[i - 1] == '/'))) {

            if (np < 30) {
                num[np++] = c;
                num[np] = 0;
            }

        } else {

            /* Guardar numero */

            if (np > 0) {

                numeros[cantidadNumeros++] = atof(num);

                np = 0;
                num[0] = 0;
            }


            /* Guardar operador */

            if (c == '+' ||
                c == '-' ||
                c == '*' ||
                c == '/') {

                operaciones[cantidadOperaciones++] = c;
            }


            if (c == 0)
                break;
        }
    }


    /* No hay numeros */

    if (cantidadNumeros == 0)
        return 0;


    /* =====================================================
       PRIMERO MULTIPLICACION Y DIVISION
       ===================================================== */

    for (int i = 0; i < cantidadOperaciones; ) {

        if (operaciones[i] == '*' ||
            operaciones[i] == '/') {

            double a = numeros[i];
            double b = numeros[i + 1];

            double r = 0;


            if (operaciones[i] == '*') {
                r = a * b;
            }

            else {

                if (b != 0.0)
                    r = a / b;
                else
                    r = 0;
            }


            numeros[i] = r;


            /* Mover numeros */

            for (int j = i + 1;
                 j < cantidadNumeros - 1;
                 j++) {

                numeros[j] = numeros[j + 1];
            }


            /* Mover operadores */

            for (int j = i;
                 j < cantidadOperaciones - 1;
                 j++) {

                operaciones[j] = operaciones[j + 1];
            }


            cantidadNumeros--;
            cantidadOperaciones--;

        } else {

            i++;
        }
    }


    /* =====================================================
       SUMA Y RESTA
       ===================================================== */

    double res = numeros[0];

    for (int i = 0;
         i < cantidadOperaciones;
         i++) {

        if (operaciones[i] == '+') {

            res += numeros[i + 1];

        } else if (operaciones[i] == '-') {

            res -= numeros[i + 1];
        }
    }


    return res;
}


/* =========================================================
   CALCULAR
   ========================================================= */

void calcular(void) {

    if (expresion[0] == 0) {
        strcpy(resultado, "0");
        actualizar();
        return;
    }


    double r = evaluar(expresion);


    /* Mostrar resultado */

    snprintf(resultado, 63, "%.10g", r);


    /* Preparar nueva operacion */

    expresion[0] = 0;
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

    /* FUNCIONES */

    {   4,   6, 46, 24, "sin",  "sin",   0 },
    {  54,   6, 46, 24, "cos",  "cos",   0 },
    { 104,   6, 46, 24, "tan",  "tan",   0 },
    { 154,   6, 46, 24, "log",  "log",   0 },
    { 204,   6, 48, 24, "ln",   "ln",    0 },


    /* CIENTIFICAS */

    {   4,  34, 46, 24, "x^2",  "^2",    0 },
    {  54,  34, 46, 24, "sqrt", "sqrt",  0 },
    { 104,  34, 46, 24, "^",    "^",     0 },
    { 154,  34, 46, 24, "(",    "(",     0 },
    { 204,  34, 48, 24, ")",    ")",     0 },


    /* 7 8 9 */

    {   4,  62, 46, 24, "(7)", "7",      0 },
    {  54,  62, 46, 24, "(8)", "8",      0 },
    { 104,  62, 46, 24, "(9)", "9",      0 },

    { 154,  62, 46, 24, "DEL",  "",       2 },
    { 204,  62, 48, 24, "AC",   "",       1 },


    /* 4 5 6 */

    {   4,  90, 46, 24, "(4)", "4",      0 },
    {  54,  90, 46, 24, "(5)", "5",      0 },
    { 104,  90, 46, 24, "(6)", "6",      0 },

    { 154,  90, 46, 24, "x",   "*",      0 },
    { 204,  90, 48, 24, "/",   "/",      0 },


    /* 1 2 3 */

    {   4, 118, 46, 24, "(1)", "1",      0 },
    {  54, 118, 46, 24, "(2)", "2",      0 },
    { 104, 118, 46, 24, "(3)", "3",      0 },

    { 154, 118, 46, 24, "+",   "+",      0 },
    { 204, 118, 48, 24, "-",   "-",      0 },


    /* 0 . PI = */

    {   4, 146, 46, 24, "(0)", "0",      0 },
    {  54, 146, 46, 24, ".",   ".",      0 },
    { 104, 146, 46, 24, "pi",  "3.1415926535", 0 },

    { 154, 146, 98, 24, "=",   "",       3 }
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
    printf("       CALCULADORA\n\n");


    for (int i = 0; i < NUM_BOTONES; i++) {

        Boton *b = &botones[i];


        /*
           Convertimos coordenadas de pixeles
           a posiciones de texto.
        */

        int fila = (b->y / 14) + 1;
        int col  = (b->x / 8) + 1;


        printf("\x1b[%d;%dH%s",
               fila,
               col,
               b->label);
    }


    /*
       Separacion visual inferior
    */

    printf("\x1b[20;1H");
    printf(" Toca una tecla");
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


    /* AC */

    if (b->tipo == 1) {

        limpiar();
    }


    /* DEL */

    else if (b->tipo == 2) {

        borrar();
    }


    /* IGUAL */

    else if (b->tipo == 3) {

        calcular();
    }


    /* NORMAL */

    else {

        if (b->insert != NULL &&
            b->insert[0] != 0) {

            agregar(b->insert);
        }
    }
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void) {

    /* Inicializar video */

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);


    /* Memoria de video */

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);


    /* Consola superior */

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


    /* Consola inferior */

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


    /* Estado inicial */

    expresion[0] = 0;
    cursor = 0;

    strcpy(resultado, "0");


    /* Dibujar */

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

                procesar_boton(idx);

                /*
                   Redibujar teclado después
                   de cada pulsación.
                */

                dibujar_teclado();
            }
        }


        /* ================================================
           BOTONES FISICOS
           ================================================ */

        if (keys & KEY_A) {

            calcular();

            dibujar_teclado();
        }


        if (keys & KEY_B) {

            borrar();

            dibujar_teclado();
        }


        if (keys & KEY_X) {

            limpiar();

            dibujar_teclado();
        }


        /* START = salir */

        if (keys & KEY_START) {

            break;
        }
    }


    return 0;
}
