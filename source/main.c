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
   TIPOS DE BOTON
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
    0 = normal
    1 = AC
    2 = DEL
    3 = =
*/


/* =========================================================
   MATRIZ DEL TECLADO
   =========================================================

       SIN COS TAN LOG LN

       SQR X2  ^   (   )

        7   8   9  DEL AC

        4   5   6   X  /

        1   2   3   +  -

        0   .  PI   !  =
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
    /* -----------------------------------------------------
       FILA 0
       ----------------------------------------------------- */

    {C0, Y0, "SIN", "sin(", 0, 15},
    {C1, Y0, "COS", "cos(", 0, 15},
    {C2, Y0, "TAN", "tan(", 0, 15},
    {C3, Y0, "LOG", "log(", 0, 15},
    {C4, Y0, "LN",  "ln(",  0, 15},


    /* -----------------------------------------------------
       FILA 1
       ----------------------------------------------------- */

    {C0, Y1, "SQR", "sqrt(", 0, 15},
    {C1, Y1, "X2",  "^2",    0, 15},
    {C2, Y1, "^",   "^",     0, 15},
    {C3, Y1, "(",   "(",     0, 15},
    {C4, Y1, ")",   ")",     0, 15},


    /* -----------------------------------------------------
       FILA 2
       ----------------------------------------------------- */

    {C0, Y2, "7",   "7",  0, 15},
    {C1, Y2, "8",   "8",  0, 15},
    {C2, Y2, "9",   "9",  0, 15},
    {C3, Y2, "DEL", "",   2, 15},
    {C4, Y2, "AC",  "",   1, 15},


    /* -----------------------------------------------------
       FILA 3
       ----------------------------------------------------- */

    {C0, Y3, "4", "4", 0, 15},
    {C1, Y3, "5", "5", 0, 15},
    {C2, Y3, "6", "6", 0, 15},
    {C3, Y3, "X", "*", 0, 15},
    {C4, Y3, "/", "/", 0, 15},


    /* -----------------------------------------------------
       FILA 4
       ----------------------------------------------------- */

    {C0, Y4, "1", "1", 0, 15},
    {C1, Y4, "2", "2", 0, 15},
    {C2, Y4, "3", "3", 0, 15},
    {C3, Y4, "+", "+", 0, 15},
    {C4, Y4, "-", "-", 0, 15},


    /* -----------------------------------------------------
       FILA 5
       ----------------------------------------------------- */

    {C0, Y5, "0",  "0",  0, 15},
    {C1, Y5, ".",  ".",  0, 15},
    {C2, Y5, "PI", "pi", 0, 15},
    {C3, Y5, "!",  "!",  0, 15},
    {C4, Y5, "=",  "",   3, 15}
};


/* =========================================================
   SELECCION
   ========================================================= */

int boton_seleccionado = 10;


/* =========================================================
   SPRITES
   ========================================================= */

OamState oamSub;

u16 *graficos_botones[NUM_BOTONES];


/* =========================================================
   COLORES
   ========================================================= */

#define COLOR_NORMAL   (RGB15(8, 12, 20)  | BIT(15))
#define COLOR_NUMERO   (RGB15(5, 20, 12)  | BIT(15))
#define COLOR_FUNCION  (RGB15(10, 8, 24)  | BIT(15))
#define COLOR_OPERADOR (RGB15(22, 12, 4)  | BIT(15))
#define COLOR_ESPECIAL (RGB15(20, 4, 4)   | BIT(15))

#define COLOR_BORDE    (RGB15(31, 31, 31) | BIT(15))
#define COLOR_SEL      (RGB15(31, 25, 4)  | BIT(15))

#define COLOR_TEXTO    1


/* =========================================================
   UTILIDADES
   ========================================================= */

void limpiar_sprite(u16 *gfx)
{
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            gfx[y * 32 + x] = 0;
        }
    }
}


void pixel_sprite(
    u16 *gfx,
    int x,
    int y,
    u16 color
)
{
    if (x < 0 || x >= 32 ||
        y < 0 || y >= 32)
        return;

    gfx[y * 32 + x] = color;
}


/* =========================================================
   CIRCULO GRAFICO REAL
   ========================================================= */

void dibujar_circulo_sprite(
    u16 *gfx,
    u16 color,
    int radio
)
{
    int centro = 15;

    for (int y = -radio; y <= radio; y++)
    {
        for (int x = -radio; x <= radio; x++)
        {
            int distancia =
                x * x + y * y;

            if (distancia <= radio * radio)
            {
                pixel_sprite(
                    gfx,
                    centro + x,
                    centro + y,
                    color
                );
            }
        }
    }
}


/* =========================================================
   ANILLO
   ========================================================= */

void dibujar_anillo_sprite(
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
                x * x + y * y;

            if (d <= radio_exterior * radio_exterior &&
                d >= radio_interior * radio_interior)
            {
                pixel_sprite(
                    gfx,
                    centro + x,
                    centro + y,
                    color
                );
            }
        }
    }
}


/* =========================================================
   COLOR DEL BOTON
   ========================================================= */

u16 color_boton(int indice)
{
    int fila = indice / 5;
    int col  = indice % 5;

    if (fila < 2)
        return COLOR_FUNCION;

    if (indice == 13 || indice == 14)
        return COLOR_ESPECIAL;

    if (indice == 18 ||
        indice == 19 ||
        indice == 23 ||
        indice == 24 ||
        indice == 28 ||
        indice == 29)
    {
        return COLOR_OPERADOR;
    }

    if (fila >= 2 && col <= 2)
        return COLOR_NUMERO;

    return COLOR_NORMAL;
}


/* =========================================================
   CREAR GRAFICO DE BOTON
   ========================================================= */

void crear_grafico_boton(int indice)
{
    u16 *gfx =
        graficos_botones[indice];

    limpiar_sprite(gfx);

    /*
       Primero dibujamos el círculo exterior.
    */

    dibujar_circulo_sprite(
        gfx,
        COLOR_BORDE,
        15
    );


    /*
       Después el círculo interior.
    */

    dibujar_circulo_sprite(
        gfx,
        color_boton(indice),
        12
    );


    /*
       Si está seleccionado,
       hacemos un anillo amarillo.
    */

    if (indice == boton_seleccionado)
    {
        dibujar_anillo_sprite(
            gfx,
            COLOR_SEL,
            15,
            12
        );
    }
}


/* =========================================================
   CREAR TODOS LOS SPRITES
   ========================================================= */

void crear_botones_graficos(void)
{
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

        if (graficos_botones[i] != NULL)
        {
            crear_grafico_boton(i);
        }
    }
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
            false,
            false
        );
    }

    oamUpdate(&oamSub);
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

    printf("\n");

    if (error_calculo)
    {
        printf("ERROR: EXPRESION INVALIDA\n");
    }
}


/* =========================================================
   AGREGAR TEXTO
   ========================================================= */

void agregar(const char *s)
{
    int len = strlen(s);

    if (cursor_expr + len >= MAX_EXPR)
        return;

    strcat(expresion, s);

    cursor_expr += len;

    error_calculo = 0;

    actualizar();
}


/* =========================================================
   BORRAR
   ========================================================= */

void borrar(void)
{
    if (cursor_expr <= 0)
        return;

    cursor_expr--;

    expresion[cursor_expr] = '\0';

    error_calculo = 0;

    actualizar();
}


/* =========================================================
   LIMPIAR
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
   FACTORIAL
   ========================================================= */

double factorial(double x)
{
    if (x < 0.0 ||
        x > 12.0)
    {
        return NAN;
    }

    int n = (int)x;

    if (fabs(x - n) > 0.0000001)
        return NAN;

    double r = 1.0;

    for (int i = 2;
         i <= n;
         i++)
    {
        r *= i;
    }

    return r;
}


/* =========================================================
   GRADOS -> RADIANES
   ========================================================= */

double deg2rad(double x)
{
    return x * PI_VAL / 180.0;
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

        return PI_VAL;
    }


    if (strncmp(parser_ptr, "ans", 3) == 0)
    {
        parser_ptr += 3;

        return ans;
    }


    valor =
        strtod(
            parser_ptr,
            &fin
        );


    if (fin == parser_ptr)
    {
        error_calculo = 1;

        return 0.0;
    }


    parser_ptr = fin;

    return valor;
}


/* =========================================================
   PRIMARIO
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
    double valor =
        parse_funcion();


    while (!error_calculo)
    {
        saltar_espacios();


        if (*parser_ptr == '!')
        {
            parser_ptr++;

            valor =
                factorial(valor);

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

    if (error_calculo)
        return 0.0;

    saltar_espacios();

    if (*parser_ptr == '^')
    {
        parser_ptr++;

        derecha = parse_potencia();

        if (error_calculo)
            return 0.0;

        izquierda = pow(izquierda, derecha);

        if (isnan(izquierda) ||
            isinf(izquierda))
        {
            error_calculo = 1;
            return 0.0;
        }
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

    if (error_calculo)
        return 0.0;


    while (!error_calculo)
    {
        saltar_espacios();


        if (*parser_ptr == '*')
        {
            parser_ptr++;

            valor = parse_potencia();

            if (error_calculo)
                return 0.0;

            resultado_local *= valor;
        }


        else if (*parser_ptr == '/')
        {
            parser_ptr++;

            valor = parse_potencia();

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


/* =========================================================
   SUMA / RESTA
   ========================================================= */

double parse_suma(void)
{
    double resultado_local;
    double valor;

    resultado_local = parse_producto();

    if (error_calculo)
        return 0.0;


    while (!error_calculo)
    {
        saltar_espacios();


        if (*parser_ptr == '+')
        {
            parser_ptr++;

            valor = parse_producto();

            if (error_calculo)
                return 0.0;

            resultado_local += valor;
        }


        else if (*parser_ptr == '-')
        {
            parser_ptr++;

            valor = parse_producto();

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
   VALIDAR PARENTESIS
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


/* =========================================================
   VALIDAR SINTAXIS BASICA
   ========================================================= */

int sintaxis_basica_valida(const char *texto)
{
    int len = strlen(texto);


    if (len <= 0)
        return 0;


    /*
       Nunca debe terminar en un operador.
    */

    char ultimo = texto[len - 1];

    if (ultimo == '+' ||
        ultimo == '-' ||
        ultimo == '*' ||
        ultimo == '/' ||
        ultimo == '^' ||
        ultimo == '(')
    {
        return 0;
    }


    /*
       Nunca debe terminar en una función incompleta.
    */

    if (strcmp(texto, "sin") == 0 ||
        strcmp(texto, "cos") == 0 ||
        strcmp(texto, "tan") == 0 ||
        strcmp(texto, "log") == 0 ||
        strcmp(texto, "ln") == 0 ||
        strcmp(texto, "sqrt") == 0)
    {
        return 0;
    }


    /*
       Paréntesis balanceados.
    */

    if (!expresion_balanceada(texto))
        return 0;


    return 1;
}


/* =========================================================
   CALCULAR
   ========================================================= */

void calcular(void)
{
    double r;


    /*
       Nada escrito.
    */

    if (expresion[0] == '\0')
    {
        strcpy(resultado, "ERROR");

        error_calculo = 1;

        actualizar();

        return;
    }


    /*
       Comprobación previa.
    */

    if (!sintaxis_basica_valida(expresion))
    {
        strcpy(resultado, "ERROR");

        error_calculo = 1;

        actualizar();

        return;
    }


    error_calculo = 0;

    parser_ptr = expresion;


    r = parse_suma();


    /*
       Si el parser dejó caracteres
       sin procesar, la expresión es inválida.
    */

    saltar_espacios();


    if (*parser_ptr != '\0')
    {
        error_calculo = 1;
    }


    /*
       Comprobar resultados matemáticamente inválidos.
    */

    if (isnan(r) ||
        isinf(r))
    {
        error_calculo = 1;
    }


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


    /*
       La expresión queda vacía después
       de pulsar igual.
    */

    expresion[0] = '\0';

    cursor_expr = 0;


    actualizar();
}


/* =========================================================
   DETECTAR BOTON POR CIRCULO
   =========================================================

   IMPORTANTE:

   Ya NO usamos x/y/w/h.

   El botón visible es un círculo.

   Por lo tanto el touch también se comprueba
   contra ese mismo círculo.
*/

int detectar_boton(int tx, int ty)
{
    for (int i = 0;
         i < NUM_BOTONES;
         i++)
    {
        int dx =
            tx - botones[i].cx;

        int dy =
            ty - botones[i].cy;

        int radio =
            botones[i].radio;


        /*
           Distancia al centro.

           Si está dentro del círculo,
           es ese botón.
        */

        if ((dx * dx) +
            (dy * dy)
            <=
            (radio * radio))
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
    {
        return;
    }


    Boton *b =
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
       IGUAL
    */

    if (b->tipo == 3)
    {
        calcular();

        return;
    }


    /*
       Boton normal.
    */

    if (b->
       /* =========================================================
   PARTE 3/3
   MAIN + OAM + TOUCH + CRUCETA
   ========================================================= */


/* =========================================================
   DIBUJAR ETIQUETAS
   ========================================================= */

void dibujar_etiquetas(void)
{
    consoleSelect(&bottomScreen);

    consoleClear();

    /*
       Las etiquetas están colocadas sobre los
       círculos mediante la consola de texto.

       La consola está solamente para las letras.
       Los círculos son sprites independientes.
    */

    printf("\x1b[1;2HSIN");
    printf("\x1b[1;8HCOS");
    printf("\x1b[1;14HTAN");
    printf("\x1b[1;20HLOG");
    printf("\x1b[1;26HLN");

    printf("\x1b[4;2HSQR");
    printf("\x1b[4;8HX2");
    printf("\x1b[4;14H^");
    printf("\x1b[4;20H(");
    printf("\x1b[4;26H)");

    printf("\x1b[7;2H7");
    printf("\x1b[7;8H8");
    printf("\x1b[7;14H9");
    printf("\x1b[7;20HDEL");
    printf("\x1b[7;26HAC");

    printf("\x1b[10;2H4");
    printf("\x1b[10;8H5");
    printf("\x1b[10;14H6");
    printf("\x1b[10;20HX");
    printf("\x1b[10;26H/");

    printf("\x1b[13;2H1");
    printf("\x1b[13;8H2");
    printf("\x1b[13;14H3");
    printf("\x1b[13;20H+");
    printf("\x1b[13;26H-");

    printf("\x1b[16;2H0");
    printf("\x1b[16;8H.");
    printf("\x1b[16;14HPI");
    printf("\x1b[16;20H!");
    printf("\x1b[16;26H=");

    printf("\x1b[23;1HD-PAD: MOVER");
    printf("\x1b[24;1HSTART: USAR");
}


/* =========================================================
   ACTUALIZAR INTERFAZ COMPLETA
   ========================================================= */

void actualizar_interfaz(void)
{
    actualizar();

    dibujar_etiquetas();

    actualizar_sprites();
}


/* =========================================================
   INICIALIZAR SPRITES
   ========================================================= */

int inicializar_sprites(void)
{
    /*
       OAM de la pantalla inferior.
    */

    oamInit(
        &oamSub,
        SpriteMapping_Bmp_1D_128,
        false
    );


    /*
       Reservar memoria para cada botón.
    */

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


    /*
       Crear los círculos.
    */

    actualizar_sprites();


    return 1;
}


/* =========================================================
   LIBERAR SPRITES
   ========================================================= */

void liberar_sprites(void)
{
    for (int i = 0;
         i < NUM_BOTONES;
         i++)
    {
        if (graficos_botones[i] != NULL)
        {
            oamFreeGfx(
                &oamSub,
                graficos_botones[i]
            );

            graficos_botones[i] = NULL;
        }
    }
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    touchPosition touch;

    u32 keys;


    /* =====================================================
       VIDEO PRINCIPAL
       ===================================================== */

    videoSetMode(MODE_0_2D);

    videoSetModeSub(MODE_0_2D);


    /* =====================================================
       VRAM
       ===================================================== */

    /*
       Pantalla superior.
    */

    vramSetBankA(
        VRAM_A_MAIN_BG
    );


    /*
       Pantalla inferior:
       C = fondos de pantalla.
       D = sprites.
    */

    vramSetBankC(
        VRAM_C_SUB_BG
    );

    vramSetBankD(
        VRAM_D_SUB_SPRITE
    );


    /* =====================================================
       CONSOLA SUPERIOR
       ===================================================== */

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


    /* =====================================================
       CONSOLA INFERIOR
       ===================================================== */

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


    /* =====================================================
       ESTADO INICIAL
       ===================================================== */

    expresion[0] = '\0';

    resultado[0] = '\0';

    strcpy(
        resultado,
        "0"
    );

    cursor_expr = 0;

    ans = 0.0;

    error_calculo = 0;

    /*
       Seleccionamos el 7.
       Índice 10:
       fila 2, columna 0.
    */

    boton_seleccionado = 10;


    /* =====================================================
       SPRITES
       ===================================================== */

    if (!inicializar_sprites())
    {
        consoleSelect(&topScreen);

        consoleClear();

        printf("\n");
        printf(" ERROR DE MEMORIA\n");
        printf("\n");
        printf(" No se pudieron crear\n");
        printf(" los botones graficos.\n");

        while (1)
        {
            swiWaitForVBlank();
        }
    }


    /* =====================================================
       PRIMER DIBUJO
       ===================================================== */

    actualizar_interfaz();


    /* =====================================================
       BUCLE PRINCIPAL
       ===================================================== */

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        keys = keysDown();


        /* =================================================
           SALIR
           ================================================= */

        if (keys & KEY_SELECT)
        {
            break;
        }


        /* =================================================
           CRUCETA ARRIBA
           ================================================= */

        if (keys & KEY_UP)
        {
            mover_arriba();
        }


        /* =================================================
           CRUCETA ABAJO
           ================================================= */

        if (keys & KEY_DOWN)
        {
            mover_abajo();
        }


        /* =================================================
           CRUCETA IZQUIERDA
           ================================================= */

        if (keys & KEY_LEFT)
        {
            mover_izquierda();
        }


        /* =================================================
           CRUCETA DERECHA
           ================================================= */

        if (keys & KEY_RIGHT)
        {
            mover_derecha();
        }


        /* =================================================
           START
           ================================================= */

        if (keys & KEY_START)
        {
            pulsar_seleccionado();
        }


        /* =================================================
           A
           ================================================= */

        if (keys & KEY_A)
        {
            pulsar_seleccionado();
        }


        /* =================================================
           B = BORRAR
           ================================================= */

        if (keys & KEY_B)
        {
            borrar();

            actualizar_sprites();
        }


        /* =================================================
           X = AC
           ================================================= */

        if (keys & KEY_X)
        {
            limpiar();

            actualizar_sprites();
        }


        /* =================================================
           TOUCH
           ================================================= */

        if (keys & KEY_TOUCH)
        {
            touchRead(&touch);


            /*
               AQUÍ está la diferencia importante:

               detectar_boton() utiliza el centro y radio
               del círculo REAL.

               Ya no utiliza la posición del texto.
            */

            int indice =
                detectar_boton(
                    touch.px,
                    touch.py
                );


            if (indice >= 0)
            {
                /*
                   Al tocar un círculo también lo
                   seleccionamos con la cruceta.
                */

                boton_seleccionado =
                    indice;


                actualizar_sprites();


                /*
                   Después ejecutamos el botón.
                */

                procesar_boton(indice);


                actualizar_sprites();
            }
        }
    }


    /* =====================================================
       LIMPIEZA
       ===================================================== */

    liberar_sprites();

    oamClear(
        &oamSub,
        0,
        128
    );


    return 0;
}
