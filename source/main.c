#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_EXPR 48
#define MAX_RESULT 64

char expresion[MAX_EXPR];
char resultado[MAX_RESULT];

int cursor = 0;

/* Ultimo resultado */
double ultimo_resultado = 0.0;

/* Consolas */
PrintConsole topScreen;
PrintConsole bottomScreen;


/* =========================================================
   UTILIDADES
   ========================================================= */

void actualizar(void) {
    consoleSelect(&topScreen);
    consoleClear();

    printf("\n");
    printf("  DS SCIENTIFIC CALCULATOR\n");
    printf("  ========================\n\n");

    printf("  EXPRESION\n");
    printf("  %s\n", expresion[0] ? expresion : "0");

    printf("\n");
    printf("  RESULTADO\n");
    printf("  %s\n", resultado);

    printf("\n");
    printf("  DEG | A=CALC B=DEL X=AC\n");
}


/* =========================================================
   AGREGAR
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

double deg2rad(double x) {
    return x * M_PI / 180.0;
}


/* =========================================================
   PARSER MATEMATICO
   ========================================================= */

/*
   Esta parte reemplaza al evaluador antiguo.

   Orden:

   expresion
      |
      v
   suma/resta
      |
      v
   multiplicacion/division
      |
      v
   potencia
      |
      v
   unario
      |
      v
   factorial
      |
      v
   funciones
      |
      v
   parentesis/numeros
*/


const char *parser_ptr;
int parser_error = 0;


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
   Funciones auxiliares
   --------------------------------------------------------- */

int coincide(const char *texto) {
    int len = strlen(texto);

    if (strncmp(parser_ptr, texto, len) == 0) {
        parser_ptr += len;
        return 1;
    }

    return 0;
}


/* ---------------------------------------------------------
   Declaraciones
   --------------------------------------------------------- */

double parse_suma(void);
double parse_multiplicacion(void);
double parse_potencia(void);
double parse_unario(void);
double parse_postfijo(void);
double parse_principal(void);


/* ---------------------------------------------------------
   Factorial
   --------------------------------------------------------- */

double factorial(double x) {

    if (x < 0.0) {
        parser_error = 1;
        return 0.0;
    }

    /*
       Permitimos factorial de enteros.
    */

    double redondeado = floor(x + 0.0000001);

    if (fabs(x - redondeado) > 0.000001) {
        parser_error = 1;
        return 0.0;
    }

    if (redondeado > 170.0) {
        parser_error = 1;
        return 0.0;
    }

    double r = 1.0;

    for (int i = 2; i <= (int)redondeado; i++) {
        r *= i;
    }

    return r;
}


/* ---------------------------------------------------------
   NUMERO / PARENTESIS / FUNCIONES
   --------------------------------------------------------- */

double parse_principal(void) {

    saltar_espacios();


    /* ---------------------------------------------
       Parentesis
       --------------------------------------------- */

    if (*parser_ptr == '(') {

        parser_ptr++;

        double valor = parse_suma();

        saltar_espacios();

        if (*parser_ptr == ')') {
            parser_ptr++;
        } else {
            parser_error = 1;
        }

        return valor;
    }


    /* ---------------------------------------------
       Numero
       --------------------------------------------- */

    if ((*parser_ptr >= '0' && *parser_ptr <= '9') ||
        *parser_ptr == '.') {

        char *fin;

        double valor = strtod(parser_ptr, &fin);

        if (fin == parser_ptr) {
            parser_error = 1;
            return 0.0;
        }

        parser_ptr = fin;

        return valor;
    }


    /* ---------------------------------------------
       PI
       --------------------------------------------- */

    if (strncmp(parser_ptr, "pi", 2) == 0) {

        parser_ptr += 2;

        return M_PI;
    }


    /* ---------------------------------------------
       ANS
       --------------------------------------------- */

    if (strncmp(parser_ptr, "ans", 3) == 0) {

        parser_ptr += 3;

        return ultimo_resultado;
    }


    /* ---------------------------------------------
       E
       --------------------------------------------- */

    if (*parser_ptr == 'e') {

        parser_ptr++;

        return M_E;
    }


    /* ---------------------------------------------
       SIN
       --------------------------------------------- */

    if (strncmp(parser_ptr, "sin", 3) == 0) {

        parser_ptr += 3;

        saltar_espacios();

        double valor;

        if (*parser_ptr == '(') {

            parser_ptr++;

            valor = parse_suma();

            saltar_espacios();

            if (*parser_ptr == ')')
                parser_ptr++;
            else
                parser_error = 1;

        } else {

            valor = parse_unario();
        }

        return sin(deg2rad(valor));
    }


    /* ---------------------------------------------
       COS
       --------------------------------------------- */

    if (strncmp(parser_ptr, "cos", 3) == 0) {

        parser_ptr += 3;

        saltar_espacios();

        double valor;

        if (*parser_ptr == '(') {

            parser_ptr++;

            valor = parse_suma();

            saltar_espacios();

            if (*parser_ptr == ')')
                parser_ptr++;
            else
                parser_error = 1;

        } else {

            valor = parse_unario();
        }

        return cos(deg2rad(valor));
    }


    /* ---------------------------------------------
       TAN
       --------------------------------------------- */

    if (strncmp(parser_ptr, "tan", 3) == 0) {

        parser_ptr += 3;

        saltar_espacios();

        double valor;

        if (*parser_ptr == '(') {

            parser_ptr++;

            valor = parse_suma();

            saltar_espacios();

            if (*parser_ptr == ')')
                parser_ptr++;
            else
                parser_error = 1;

        } else {

            valor = parse_unario();
        }

        return tan(deg2rad(valor));
    }


    /* ---------------------------------------------
       SQRT
       --------------------------------------------- */

    if (strncmp(parser_ptr, "sqrt", 4) == 0) {

        parser_ptr += 4;

        saltar_espacios();

        double valor;

        if (*parser_ptr == '(') {

            parser_ptr++;

            valor = parse_suma();

            saltar_espacios();

            if (*parser_ptr == ')')
                parser_ptr++;
            else
                parser_error = 1;

        } else {

            valor = parse_unario();
        }

        if (valor < 0.0) {
            parser_error = 1;
            return 0.0;
        }

        return sqrt(valor);
    }


    /* ---------------------------------------------
       LOG
       --------------------------------------------- */

    if (strncmp(parser_ptr, "log", 3) == 0) {

        parser_ptr += 3;

        saltar_espacios();

        double valor;

        if (*parser_ptr == '(') {

            parser_ptr++;

            valor = parse_suma();

            saltar_espacios();

            if (*parser_ptr == ')')
                parser_ptr++;
            else
                parser_error = 1;

        } else {

            valor = parse_unario();
        }

        if (valor <= 0.0) {
            parser_error = 1;
            return 0.0;
        }

        return log10(valor);
    }


    /* ---------------------------------------------
       LN
       --------------------------------------------- */

    if (strncmp(parser_ptr, "ln", 2) == 0) {

        parser_ptr += 2;

        saltar_espacios();

        double valor;

        if (*parser_ptr == '(') {

            parser_ptr++;

            valor = parse_suma();

            saltar_espacios();

            if (*parser_ptr == ')')
                parser_ptr++;
            else
                parser_error = 1;

        } else {

            valor = parse_unario();
        }

        if (valor <= 0.0) {
            parser_error = 1;
            return 0.0;
        }

        return log(valor);
    }


    parser_error = 1;

    return 0.0;
}


/* ---------------------------------------------------------
   POSTFIJO
   --------------------------------------------------------- */

double parse_postfijo(void) {

    double valor = parse_principal();

    while (!parser_error) {

        saltar_espacios();

        if (*parser_ptr == '!') {

            parser_ptr++;

            valor = factorial(valor);

        }

        else if (*parser_ptr == '%') {

            parser_ptr++;

            valor = valor / 100.0;

        }

        else {

            break;
        }
    }

    return valor;
}


/* ---------------------------------------------------------
   UNARIO
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
   POTENCIA
   --------------------------------------------------------- */

double parse_potencia(void) {

    double izquierda = parse_unario();

    saltar_espacios();

    if (*parser_ptr == '^') {

        parser_ptr++;

        double derecha = parse_potencia();

        izquierda = pow(izquierda, derecha);
    }

    return izquierda;
}


/* ---------------------------------------------------------
   MULTIPLICACION / DIVISION
   --------------------------------------------------------- */

double parse_multiplicacion(void) {

    double valor = parse_potencia();

    while (!parser_error) {

        saltar_espacios();

        if (*parser_ptr == '*') {

            parser_ptr++;

            valor *= parse_potencia();
        }

        else if (*parser_ptr == '/') {

            parser_ptr++;

            double divisor = parse_potencia();

            if (fabs(divisor) < 0.000000000001) {

                parser_error = 1;

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


/* ---------------------------------------------------------
   SUMA / RESTA
   --------------------------------------------------------- */

double parse_suma(void) {

    double valor = parse_multiplicacion();

    while (!parser_error) {

        saltar_espacios();

        if (*parser_ptr == '+') {

            parser_ptr++;

            valor += parse_multiplicacion();
        }

        else if (*parser_ptr == '-') {

            parser_ptr++;

            valor -= parse_multiplicacion();
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

double evaluar(const char *input) {

    parser_ptr = input;

    parser_error = 0;


    double resultado_parser = parse_suma();


    saltar_espacios();


    /*
       Si todavía queda algo sin interpretar,
       la expresión es incorrecta.
    */

    if (*parser_ptr != '\0') {
        parser_error = 1;
    }


    if (parser_error)
        return 0.0;


    return resultado_parser;
}


/* =========================================================
   CALCULAR
   ========================================================= */

void calcular(void) {

    if (expresion[0] == '\0') {

        strcpy(resultado, "0");

        actualizar();

        return;
    }


    double r = evaluar(expresion);


    if (parser_error) {

        strcpy(resultado, "ERROR");

        actualizar();

        return;
    }


    /*
       Evitar resultados infinitos o NaN.
    */

    if (isnan(r) || isinf(r)) {

        strcpy(resultado, "ERROR");

        actualizar();

        return;
    }


    ultimo_resultado = r;


    snprintf(
        resultado,
        MAX_RESULT,
        "%.10g",
        r
    );


    /*
       La calculadora queda lista
       para una nueva operacion.
    */

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
   TIPOS:

   0 = normal
   1 = AC
   2 = DEL
   3 = =
*/


Boton botones[] = {

    /* -----------------------------------------------------
       FUNCIONES
       ----------------------------------------------------- */

    {   4,   6, 46, 24, "sin(",  "sin(",   0 },
    {  54,   6, 46, 24, "cos(",  "cos(",   0 },
    { 104,   6, 46, 24, "tan(",  "tan(",   0 },
    { 154,   6, 46, 24, "log(",  "log(",   0 },
    { 204,   6, 48, 24, "ln(",   "ln(",    0 },


    /* -----------------------------------------------------
       CIENTIFICAS
       ----------------------------------------------------- */

    {   4,  34, 46, 24, "x^2",   "^2",     0 },
    {  54,  34, 46, 24, "sqrt(", "sqrt(",  0 },
    { 104,  34, 46, 24, "^",     "^",      0 },
    { 154,  34, 46, 24, "(",     "(",      0 },
    { 204,  34, 48, 24, ")",     ")",      0 },


    /* -----------------------------------------------------
       7 8 9
       ----------------------------------------------------- */

    {   4,  62, 46, 24, "[ 7 ]", "7",      0 },
    {  54,  62, 46, 24, "[ 8 ]", "8",      0 },
    { 104,  62, 46, 24, "[ 9 ]", "9",      0 },

    { 154,  62, 46, 24, "DEL",   "",       2 },
    { 204,  62, 48, 24, "AC",    "",       1 },


    /* -----------------------------------------------------
       4 5 6
       ----------------------------------------------------- */

    {   4,  90, 46, 24, "[ 4 ]", "4",      0 },
    {  54,  90, 46, 24, "[ 5 ]", "5",      0 },
    { 104,  90, 46, 24, "[ 6 ]", "6",      0 },

    { 154,  90, 46, 24, "x",     "*",      0 },
    { 204,  90, 48, 24, "/",     "/",      0 },


    /* -----------------------------------------------------
       1 2 3
       ----------------------------------------------------- */

    {   4, 118, 46, 24, "[ 1 ]", "1",      0 },
    {  54, 118, 46, 24, "[ 2 ]", "2",      0 },
    { 104, 118, 46, 24, "[ 3 ]", "3",      0 },

    { 154, 118, 46, 24, "+",     "+",      0 },
    { 204, 118, 48, 24, "-",     "-",      0 },


    /* -----------------------------------------------------
       0 . PI =
       ----------------------------------------------------- */

    {   4, 146, 46, 24, "[ 0 ]", "0",      0 },
    {  54, 146, 46, 24, ".",     ".",      0 },
    { 104, 146, 46, 24, "pi",    "pi",     0 },

    { 154, 146, 98, 24, "=",     "",       3 }
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
    printf("  CALCULADORA CIENTIFICA\n");
    printf("  ----------------------\n");


    for (int i = 0; i < NUM_BOTONES; i++) {

        Boton *b = &botones[i];


        /*
           El teclado visual utiliza exactamente
           las mismas coordenadas que el sistema tactil.
        */

        int fila = (b->y / 14) + 1;
        int col  = (b->x / 8) + 1;


        printf(
            "\x1b[%d;%dH%s",
            fila,
            col,
            b->label
        );
    }


    printf("\x1b[20;1H");
    printf(" A=CALC  B=DEL  X=AC");
}


/* =========================================================
   DETECTAR BOTON
   ========================================================= */

int detectar_boton(int tx, int ty) {

    /*
       Primero comprobamos cada boton individualmente.

       No usamos una rejilla aproximada.
       Esto evita que 2 pueda caer accidentalmente
       en el area de 8.
    */

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

        return;
    }


    /* DEL */

    if (b->tipo == 2) {

        borrar();

        return;
    }


    /* IGUAL */

    if (b->tipo == 3) {

        calcular();

        return;
    }


    /* NORMAL */

    if (b->insert != NULL &&
        b->insert[0] != '\0') {

        agregar(b->insert);
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


    /* -----------------------------------------------------
       VRAM
       ----------------------------------------------------- */

    vramSetBankA(VRAM_A_MAIN_BG);

    vramSetBankC(VRAM_C_SUB_BG);


    /* -----------------------------------------------------
       PANTALLA SUPERIOR
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
       PANTALLA INFERIOR
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
