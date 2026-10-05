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
int shift = 0;
int modo_bot = 0;

PrintConsole topScreen;
PrintConsole bottomScreen;

void actualizar(void) {
    consoleSelect(&topScreen);
    consoleClear();

    printf("\n  DS Scientific Calculator\n");
    printf("  --------------------------\n\n");

    if (modo_bot) {
        printf("  [MODO BOT EDUCATIVO]\n\n");
    } else if (shift) {
        printf("  [SHIFT]\n\n");
    } else {
        printf("\n");
    }

    printf("  %s\n\n", expresion[0] ? expresion : "0");
    printf("  = %s\n", resultado);
}

void agregar(const char *s) {
    int len = strlen(s);
    if (cursor + len < MAX_EXPR - 1) {
        strcat(expresion, s);
        cursor += len;
        strcpy(resultado, expresion);
        actualizar();
    }
}

void borrar(void) {
    if (cursor > 0) {
        expresion[--cursor] = 0;
        if (cursor == 0) strcpy(resultado, "0");
        else strcpy(resultado, expresion);
        actualizar();
    }
}

void limpiar(void) {
    expresion[0] = 0;
    cursor = 0;
    strcpy(resultado, "0");
    shift = 0;
    actualizar();
}

double deg2rad(double x) {
    return x * M_PI / 180.0;
}

double evaluar(const char *input) {
    char expr[128];
    strncpy(expr, input, 127);
    expr[127] = 0;

    for (int i = 0; expr[i]; i++) {
        expr[i] = tolower((unsigned char)expr[i]);
    }

    // Funciones cientificas
    if (strncmp(expr, "sin", 3) == 0)  return sin(deg2rad(atof(expr + 3)));
    if (strncmp(expr, "cos", 3) == 0)  return cos(deg2rad(atof(expr + 3)));
    if (strncmp(expr, "tan", 3) == 0)  return tan(deg2rad(atof(expr + 3)));
    if (strncmp(expr, "log", 3) == 0)  return log10(atof(expr + 3));
    if (strncmp(expr, "ln", 2) == 0)   return log(atof(expr + 2));
    if (strncmp(expr, "sqrt", 4) == 0) return sqrt(atof(expr + 4));

    // Potencia 2^8
    char *pot = strchr(expr, '^');
    if (pot != NULL) {
        *pot = 0;
        return pow(atof(expr), atof(pot + 1));
    }

    // Operaciones basicas
    double res = 0;
    char op = '+';
    char num[32];
    int np = 0;
    num[0] = 0;

    for (int i = 0; ; i++) {
        char c = expr[i];

        if ((c >= '0' && c <= '9') || c == '.' || (c == '-' && np == 0)) {
            num[np++] = c;
            num[np] = 0;
        } else {
            if (np > 0) {
                double v = atof(num);
                if (op == '+') res += v;
                else if (op == '-') res -= v;
                else if (op == '*') res *= v;
                else if (op == '/') {
                    if (v != 0.0) res /= v;
                }
                np = 0;
                num[0] = 0;
            }
            if (c == '+' || c == '-' || c == '*' || c == '/') op = c;
            if (c == 0) break;
        }
    }
    return res;
}

void calcular(void) {
    // Codigo secreto 2026
    if (strcmp(expresion, "2026") == 0) {
        modo_bot = !modo_bot;
        if (modo_bot) strcpy(resultado, "BOT ACTIVADO");
        else strcpy(resultado, "BOT DESACTIVADO");
        expresion[0] = 0;
        cursor = 0;
        actualizar();
        return;
    }

    if (modo_bot) {
        char t[64];
        strncpy(t, expresion, 63);
        t[63] = 0;
        for (int i = 0; t[i]; i++) t[i] = tolower((unsigned char)t[i]);

        if (strstr(t, "quimica")) strcpy(resultado, "Ciencia de la materia");
        else if (strstr(t, "atomo")) strcpy(resultado, "Unidad basica");
        else if (strstr(t, "verbo")) strcpy(resultado, "Expresa accion");
        else if (strstr(t, "fraccion")) strcpy(resultado, "Parte de un todo");
        else if (strstr(t, "seno") || strstr(t, "sin")) strcpy(resultado, "Funcion trigonometrica");
        else if (strstr(t, "log")) strcpy(resultado, "Logaritmo base 10");
        else strcpy(resultado, "Tema no encontrado");

        expresion[0] = 0;
        cursor = 0;
        actualizar();
        return;
    }

    // Calculadora normal
    double r = evaluar(expresion);
    snprintf(resultado, 63, "%.8g", r);
    expresion[0] = 0;
    cursor = 0;
    actualizar();
}

// ===================== BOTONES =====================

typedef struct {
    int x, y, w, h;
    const char *label;
    const char *insert;
    int tipo; // 0=normal, 1=AC, 2=DEL, 3==, 4=SHIFT
} Boton;

Boton botones[] = {
    {  4,  6, 46, 24, "sin",  "sin",  0 },
    { 54,  6, 46, 24, "cos",  "cos",  0 },
    {104,  6, 46, 24, "tan",  "tan",  0 },
    {154,  6, 46, 24, "log",  "log",  0 },
    {204,  6, 48, 24, "ln",   "ln",   0 },

    {  4, 34, 46, 24, "x^2",  "^2",   0 },
    { 54, 34, 46, 24, "sqrt", "sqrt", 0 },
    {104, 34, 46, 24, "^",    "^",    0 },
    {154, 34, 46, 24, "(",    "(",    0 },
    {204, 34, 48, 24, ")",    ")",    0 },

    {  4, 62, 46, 24, "7", "7", 0 },
    { 54, 62, 46, 24, "8", "8", 0 },
    {104, 62, 46, 24, "9", "9", 0 },
    {154, 62, 46, 24, "DEL","", 2 },
    {204, 62, 48, 24, "AC", "", 1 },

    {  4, 90, 46, 24, "4", "4", 0 },
    { 54, 90, 46, 24, "5", "5", 0 },
    {104, 90, 46, 24, "6", "6", 0 },
    {154, 90, 46, 24, "x", "*", 0 },
    {204, 90, 48, 24, "/", "/", 0 },

    {  4,118, 46, 24, "1", "1", 0 },
    { 54,118, 46, 24, "2", "2", 0 },
    {104,118, 46, 24, "3", "3", 0 },
    {154,118, 46, 24, "+", "+", 0 },
    {204,118, 48, 24, "-", "-", 0 },

    {  4,146, 46, 24, "0", "0", 0 },
    { 54,146, 46, 24, ".", ".", 0 },
    {104,146, 46, 24, "pi","3.1416", 0 },
    {154,146, 98, 24, "=", "", 3 },
};

#define NUM_BOTONES (sizeof(botones) / sizeof(Boton))

void dibujar_teclado(void) {
    consoleSelect(&bottomScreen);
    consoleClear();

    for (int i = 0; i < NUM_BOTONES; i++) {
        Boton *b = &botones[i];
        int fila = (b->y / 14) + 1;
        int col  = (b->x / 8) + 1;
        printf("\x1b[%d;%dH%s", fila, col, b->label);
    }
}

int detectar_boton(int tx, int ty) {
    for (int i = 0; i < NUM_BOTONES; i++) {
        Boton *b = &botones[i];
        if (tx >= b->x && tx < b->x + b->w &&
            ty >= b->y && ty < b->y + b->h) {
            return i;
        }
    }
    return -1;
}

void procesar_boton(int idx) {
    if (idx < 0) return;

    Boton *b = &botones[idx];

    if (b->tipo == 1) {
        limpiar();
    } else if (b->tipo == 2) {
        borrar();
    } else if (b->tipo == 3) {
        calcular();
    } else {
        if (b->insert[0] != 0) {
            agregar(b->insert);
        }
    }
}

// ===================== MAIN =====================

int main(void) {
    // Inicializar pantallas
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    // Estado inicial
    expresion[0] = 0;
    strcpy(resultado, "0");

    actualizar();
    dibujar_teclado();

    touchPosition touch;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        u32 keys = keysDown();

        // Pantalla tactil
        if (keys & KEY_TOUCH) {
            touchRead(&touch);
            int idx = detectar_boton(touch.px, touch.py);
            procesar_boton(idx);
            dibujar_teclado();
        }

        // Botones fisicos
        if (keys & KEY_A) calcular();
        if (keys & KEY_B) borrar();
        if (keys & KEY_X) limpiar();
        if (keys & KEY_START) break;
    }

    return 0;
}
