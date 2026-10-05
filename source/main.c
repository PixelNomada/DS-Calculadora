#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_EXPR 64

char expresion[MAX_EXPR] = "";
char resultado[64] = "0";
int cursor = 0;
int shift = 0;
int modo_bot = 0;

PrintConsole top, bottom;

// ===================== DISPLAY =====================

void actualizar() {
    consoleSelect(&top);
    consoleClear();

    printf("\n  DS Scientific Calculator\n");
    printf("  ----------------------------\n\n");

    if (modo_bot) {
        printf("  ** MODO BOT EDUCATIVO **\n\n");
    } else if (shift) {
        printf("  [SHIFT]\n\n");
    } else {
        printf("\n\n");
    }

    printf("  %s\n\n", expresion[0] ? expresion : "0");
    printf("  = %s\n", resultado);
}

// ===================== ENTRADA =====================

void agregar(const char *s) {
    int len = strlen(s);
    if (cursor + len < MAX_EXPR - 1) {
        strcat(expresion, s);
        cursor += len;
        strcpy(resultado, expresion);
        actualizar();
    }
}

void borrar() {
    if (cursor > 0) {
        expresion[--cursor] = 0;
        strcpy(resultado, cursor ? expresion : "0");
        actualizar();
    }
}

void limpiar() {
    expresion[0] = 0;
    cursor = 0;
    strcpy(resultado, "0");
    shift = 0;
    actualizar();
}

// ===================== EVALUADOR CIENTÍFICO =====================

double deg2rad(double x) {
    return x * M_PI / 180.0;
}

double evaluar(const char *in) {
    char expr[128];
    strncpy(expr, in, 127);
    expr[127] = 0;

    for (int i = 0; expr[i]; i++) expr[i] = tolower(expr[i]);

    if (strncmp(expr, "sin", 3) == 0)
        return sin(deg2rad(atof(expr+3)));

    if (strncmp(expr, "cos", 3) == 0)
        return cos(deg2rad(atof(expr+3)));

    if (strncmp(expr, "tan", 3) == 0)
        return tan(deg2rad(atof(expr+3)));

    if (strncmp(expr, "asin", 4) == 0)
        return asin(atof(expr+4)) * 180.0 / M_PI;

    if (strncmp(expr, "acos", 4) == 0)
        return acos(atof(expr+4)) * 180.0 / M_PI;

    if (strncmp(expr, "atan", 4) == 0)
        return atan(atof(expr+4)) * 180.0 / M_PI;

    if (strncmp(expr, "log", 3) == 0)
        return log10(atof(expr+3));

    if (strncmp(expr, "ln", 2) == 0)
        return log(atof(expr+2));

    if (strncmp(expr, "sqrt", 4) == 0)
        return sqrt(atof(expr+4));

    if (strncmp(expr, "cbrt", 4) == 0)
        return cbrt(atof(expr+4));

    char *fact = strchr(expr, '!');
    if (fact) {
        *fact = 0;
        int n = atoi(expr);
        double r = 1;
        for (int i = 2; i <= n; i++) r *= i;
        return r;
    }

    char *pot = strchr(expr, '^');
    if (pot) {
        *pot = 0;
        return pow(atof(expr), atof(pot+1));
    }

    if (strstr(expr, "^-1")) {
        char *p = strstr(expr, "^-1");
        *p = 0;
        double v = atof(expr);
        return v != 0 ? 1.0 / v : 0;
    }

    double res = 0;
    char op = '+';
    char num[32] = {0};
    int np = 0;

    for (int i = 0; ; i++) {
        char c = expr[i];

        if ((c >= '0' && c <= '9') ||
            c == '.' ||
            (c == '-' && np == 0)) {
            num[np++] = c;
            num[np] = 0;
        } else {
            if (np > 0) {
                double v = atof(num);

                if (op == '+') res += v;
                else if (op == '-') res -= v;
                else if (op == '*') res *= v;
                else if (op == '/') res = (v != 0) ? res / v : 0;

                np = 0;
                num[0] = 0;
            }

            if (c == '+' || c == '-' || c == '*' || c == '/')
                op = c;

            if (c == 0) break;
        }
    }

    return res;
}
    if (modo_bot) {
        char t[64];
        strncpy(t, expresion, 63);
        for (int i = 0; t[i]; i++) t[i] = tolower(t[i]);

        if (strstr(t, "numero natural"))
            strcpy(resultado, "0,1,2,3,...; no negativos");

        else if (strstr(t, "naturales"))
            strcpy(resultado, "Enteros usados para contar");

        else if (strstr(t, "numero entero"))
            strcpy(resultado, "...,-2,-1,0,1,2,...");

        else if (strstr(t, "enteros"))
            strcpy(resultado, "Incluyen positivos, negativos y cero");

        else if (strstr(t, "numero racional"))
            strcpy(resultado, "Puede escribirse como a/b");

        else if (strstr(t, "racionales"))
            strcpy(resultado, "Fracciones con denominador no cero");

        else if (strstr(t, "irracional"))
            strcpy(resultado, "No puede expresarse como fraccion");

        else if (strstr(t, "irracionales"))
            strcpy(resultado, "Ejemplos: pi, e y raiz de 2");

        else if (strstr(t, "numero real"))
            strcpy(resultado, "Incluye racionales e irracionales");

        else if (strstr(t, "reales"))
            strcpy(resultado, "Todos los puntos de la recta real");

        else if (strstr(t, "numero complejo"))
            strcpy(resultado, "Tiene forma a+bi");

        else if (strstr(t, "complejos"))
            strcpy(resultado, "Usan i, donde i^2=-1");

        else if (strstr(t, "suma"))
            strcpy(resultado, "Combina cantidades mediante +");

        else if (strstr(t, "resta"))
            strcpy(resultado, "Es la suma del opuesto");

        else if (strstr(t, "multiplicacion"))
            strcpy(resultado, "Suma repetida o producto");

        else if (strstr(t, "division"))
            strcpy(resultado, "Reparte una cantidad en partes");

        else if (strstr(t, "divisor"))
            strcpy(resultado, "Numero que divide exactamente");

        else if (strstr(t, "dividendo"))
            strcpy(resultado, "Numero que se divide");

        else if (strstr(t, "cociente"))
            strcpy(resultado, "Resultado de una division");

        else if (strstr(t, "residuo"))
            strcpy(resultado, "Resto de una division");

        else if (strstr(t, "operaciones"))
            strcpy(resultado, "Suma resta producto y cociente");

        else if (strstr(t, "jerarquia"))
            strcpy(resultado, "Parentesis, potencias, x/, luego +/-");

        else if (strstr(t, "orden de operaciones"))
            strcpy(resultado, "Parentesis antes de operaciones");

        else if (strstr(t, "valor absoluto"))
            strcpy(resultado, "Distancia de un numero a cero");

        else if (strstr(t, "absoluto"))
            strcpy(resultado, "|x| nunca es negativo");

        else if (strstr(t, "opuesto"))
            strcpy(resultado, "El opuesto de x es -x");

        else if (strstr(t, "reciproco"))
            strcpy(resultado, "Reciproco de x: 1/x");

        else if (strstr(t, "porcentaje"))
            strcpy(resultado, "Porcentaje = parte/total por 100");

        else if (strstr(t, "proporcion"))
            strcpy(resultado, "Igualdad entre dos razones");

        else if (strstr(t, "razon"))
            strcpy(resultado, "Comparacion mediante division");

        else if (strstr(t, "fraccion"))
            strcpy(resultado, "Numerador sobre denominador");

        else if (strstr(t, "numerador"))
            strcpy(resultado, "Numero situado arriba");

        else if (strstr(t, "denominador"))
            strcpy(resultado, "Numero situado abajo");

        else if (strstr(t, "fraccion propia"))
            strcpy(resultado, "Numerador menor que denominador");

        else if (strstr(t, "fraccion impropia"))
            strcpy(resultado, "Numerador mayor o igual");

        else if (strstr(t, "decimal"))
            strcpy(resultado, "Representacion en base diez");

        else if (strstr(t, "redondeo"))
            strcpy(resultado, "Aproxima un numero a cierta cifra");

        else if (strstr(t, "truncamiento"))
            strcpy(resultado, "Elimina cifras sin redondear");
        else if (strstr(t, "potencia"))
            strcpy(resultado, "a^n multiplica a n factores a");

        else if (strstr(t, "exponente"))
            strcpy(resultado, "Indica cuantas veces se multiplica");

        else if (strstr(t, "base"))
            strcpy(resultado, "Numero elevado a una potencia");

        else if (strstr(t, "exponente cero"))
            strcpy(resultado, "a^0=1 si a no es cero");

        else if (strstr(t, "exponente uno"))
            strcpy(resultado, "a^1=a");

        else if (strstr(t, "exponente negativo"))
            strcpy(resultado, "a^-n=1/a^n");

        else if (strstr(t, "potencia de potencia"))
            strcpy(resultado, "(a^m)^n=a^(mn)");

        else if (strstr(t, "producto de potencias"))
            strcpy(resultado, "a^m*a^n=a^(m+n)");

        else if (strstr(t, "cociente de potencias"))
            strcpy(resultado, "a^m/a^n=a^(m-n)");

        else if (strstr(t, "raiz cuadrada"))
            strcpy(resultado, "Numero que al cuadrado da x");

        else if (strstr(t, "raiz cubica"))
            strcpy(resultado, "Numero cuyo cubo produce x");

        else if (strstr(t, "raiz"))
            strcpy(resultado, "Operacion inversa de una potencia");

        else if (strstr(t, "radical"))
            strcpy(resultado, "Expresion que contiene una raiz");

        else if (strstr(t, "logaritmo"))
            strcpy(resultado, "log_b(x)=y si b^y=x");

        else if (strstr(t, "logaritmo natural"))
            strcpy(resultado, "ln(x) usa base e");

        else if (strstr(t, "logaritmo decimal"))
            strcpy(resultado, "Logaritmo de base 10");

        else if (strstr(t, "base diez"))
            strcpy(resultado, "Base comun del logaritmo decimal");

        else if (strstr(t, "base e"))
            strcpy(resultado, "e es aproximadamente 2.71828");

        else if (strstr(t, "propiedad logaritmica"))
            strcpy(resultado, "log(ab)=log(a)+log(b)");

        else if (strstr(t, "logaritmo producto"))
            strcpy(resultado, "Convierte producto en suma");

        else if (strstr(t, "logaritmo cociente"))
            strcpy(resultado, "log(a/b)=log(a)-log(b)");

        else if (strstr(t, "logaritmo potencia"))
            strcpy(resultado, "log(a^n)=n log(a)");

        else if (strstr(t, "cambio de base"))
            strcpy(resultado, "log_b(a)=ln(a)/ln(b)");

        else if (strstr(t, "numero e"))
            strcpy(resultado, "e=2.718281828...");

        else if (strstr(t, "pi"))
            strcpy(resultado, "pi=3.141592653589793...");

        else if (strstr(t, "phi"))
            strcpy(resultado, "Phi=(1+raiz(5))/2");

        else if (strstr(t, "infinito"))
            strcpy(resultado, "Concepto de magnitud sin limite");

        else if (strstr(t, "indeterminacion"))
            strcpy(resultado, "Forma cuyo valor no queda definido");

        else if (strstr(t, "cero"))
            strcpy(resultado, "Elemento neutro de la suma");

        else if (strstr(t, "uno"))
            strcpy(resultado, "Elemento neutro de la multiplicacion");
        else if (strstr(t, "algebra"))
            strcpy(resultado, "Estudia simbolos y relaciones");

        else if (strstr(t, "variable"))
            strcpy(resultado, "Simbolo que representa un valor");

        else if (strstr(t, "constante"))
            strcpy(resultado, "Cantidad que no cambia");

        else if (strstr(t, "expresion algebraica"))
            strcpy(resultado, "Combina numeros variables y operaciones");

        else if (strstr(t, "termino"))
            strcpy(resultado, "Parte separada por suma o resta");

        else if (strstr(t, "monomio"))
            strcpy(resultado, "Producto de numeros y variables");

        else if (strstr(t, "binomio"))
            strcpy(resultado, "Polinomio de dos terminos");

        else if (strstr(t, "trinomio"))
            strcpy(resultado, "Polinomio de tres terminos");

        else if (strstr(t, "polinomio"))
            strcpy(resultado, "Suma finita de monomios");

        else if (strstr(t, "coeficiente"))
            strcpy(resultado, "Numero que multiplica una variable");

        else if (strstr(t, "grado"))
            strcpy(resultado, "Mayor exponente de un polinomio");

        else if (strstr(t, "terminos semejantes"))
            strcpy(resultado, "Tienen las mismas variables y exponentes");

        else if (strstr(t, "distributiva"))
            strcpy(resultado, "a(b+c)=ab+ac");

        else if (strstr(t, "conmutativa"))
            strcpy(resultado, "a+b=b+a y ab=ba");

        else if (strstr(t, "asociativa"))
            strcpy(resultado, "(a+b)+c=a+(b+c)");

        else if (strstr(t, "identidad"))
            strcpy(resultado, "Igualdad verdadera para todo valor");

        else if (strstr(t, "factor comun"))
            strcpy(resultado, "Extrae el factor presente en terminos");

        else if (strstr(t, "factorizacion"))
            strcpy(resultado, "Expresa un polinomio como productos");

        else if (strstr(t, "diferencia de cuadrados"))
            strcpy(resultado, "a^2-b^2=(a-b)(a+b)");

        else if (strstr(t, "binomio al cuadrado"))
            strcpy(resultado, "(a+b)^2=a^2+2ab+b^2");

        else if (strstr(t, "binomio al cubo"))
            strcpy(resultado, "(a+b)^3=a^3+3a^2b+3ab^2+b^3");

        else if (strstr(t, "cuadrado de suma"))
            strcpy(resultado, "a^2+2ab+b^2");

        else if (strstr(t, "cuadrado de diferencia"))
            strcpy(resultado, "a^2-2ab+b^2");

        else if (strstr(t, "identidad notable"))
            strcpy(resultado, "Formula algebraica de uso frecuente");

        else if (strstr(t, "ecuacion"))
            strcpy(resultado, "Igualdad con una o mas incognitas");

        else if (strstr(t, "incognita"))
            strcpy(resultado, "Valor desconocido en una ecuacion");

        else if (strstr(t, "igualdad"))
            strcpy(resultado, "Relacion que usa el signo =");

        else if (strstr(t, "solucion"))
            strcpy(resultado, "Valor que hace verdadera la ecuacion");
        else if (strstr(t, "ecuacion lineal"))
            strcpy(resultado, "Ecuacion de primer grado");

        else if (strstr(t, "primer grado"))
            strcpy(resultado, "Mayor exponente de x es 1");

        else if (strstr(t, "segundo grado"))
            strcpy(resultado, "Mayor exponente de x es 2");

        else if (strstr(t, "ecuacion cuadratica"))
            strcpy(resultado, "ax^2+bx+c=0");

        else if (strstr(t, "formula general"))
            strcpy(resultado, "x=(-b+-raiz(b^2-4ac))/2a");

        else if (strstr(t, "discriminante"))
            strcpy(resultado, "Delta=b^2-4ac");

        else if (strstr(t, "discriminante positivo"))
            strcpy(resultado, "Hay dos raices reales distintas");

        else if (strstr(t, "discriminante cero"))
            strcpy(resultado, "Hay una raiz real doble");

        else if (strstr(t, "discriminante negativo"))
            strcpy(resultado, "No hay raices reales");

        else if (strstr(t, "raices"))
            strcpy(resultado, "Valores que hacen cero el polinomio");

        else if (strstr(t, "ecuaciones simultaneas"))
            strcpy(resultado, "Ecuaciones que deben cumplirse juntas");

        else if (strstr(t, "sistema lineal"))
            strcpy(resultado, "Conjunto de ecuaciones lineales");

        else if (strstr(t, "sustitucion"))
            strcpy(resultado, "Despeja y reemplaza una variable");

        else if (strstr(t, "igualacion"))
            strcpy(resultado, "Iguala dos expresiones despejadas");

        else if (strstr(t, "eliminacion"))
            strcpy(resultado, "Combina ecuaciones para cancelar variables");

        else if (strstr(t, "inecuacion"))
            strcpy(resultado, "Desigualdad con una incognita");

        else if (strstr(t, "desigualdad"))
            strcpy(resultado, "Usa <, >, <= o >=");

        else if (strstr(t, "intervalo"))
            strcpy(resultado, "Conjunto continuo de numeros reales");

        else if (strstr(t, "intervalo abierto"))
            strcpy(resultado, "No incluye los extremos");

        else if (strstr(t, "intervalo cerrado"))
            strcpy(resultado, "Incluye ambos extremos");

        else if (strstr(t, "intervalo semiabierto"))
            strcpy(resultado, "Incluye un extremo solamente");

        else if (strstr(t, "valor desconocido"))
            strcpy(resultado, "Representado normalmente por x");

        else if (strstr(t, "despejar"))
            strcpy(resultado, "Aislar una variable mediante operaciones");

        else if (strstr(t, "equivalencia"))
            strcpy(resultado, "Dos expresiones tienen el mismo valor");
        else if (strstr(t, "funcion"))
            strcpy(resultado, "Relacion que asigna una salida");

        else if (strstr(t, "dominio"))
            strcpy(resultado, "Valores permitidos de entrada");

        else if (strstr(t, "rango"))
            strcpy(resultado, "Valores posibles de salida");

        else if (strstr(t, "imagen"))
            strcpy(resultado, "Salida correspondiente a una entrada");

        else if (strstr(t, "preimagen"))
            strcpy(resultado, "Entrada asociada a una salida");

        else if (strstr(t, "variable independiente"))
            strcpy(resultado, "Variable que recibe valores");

        else if (strstr(t, "variable dependiente"))
            strcpy(resultado, "Variable determinada por otra");

        else if (strstr(t, "funcion lineal"))
            strcpy(resultado, "Forma comun: f(x)=mx+b");

        else if (strstr(t, "funcion constante"))
            strcpy(resultado, "Su salida permanece constante");

        else if (strstr(t, "funcion cuadratica"))
            strcpy(resultado, "Tiene forma ax^2+bx+c");

        else if (strstr(t, "funcion cubica"))
            strcpy(resultado, "Polinomio de grado tres");

        else if (strstr(t, "funcion polinomica"))
            strcpy(resultado, "Funcion definida por un polinomio");

        else if (strstr(t, "funcion racional"))
            strcpy(resultado, "Cociente de dos polinomios");

        else if (strstr(t, "funcion exponencial"))
            strcpy(resultado, "Forma f(x)=a^x");

        else if (strstr(t, "funcion logaritmica"))
            strcpy(resultado, "Inversa de una exponencial");

        else if (strstr(t, "funcion valor absoluto"))
            strcpy(resultado, "f(x)=|x|");

        else if (strstr(t, "funcion inversa"))
            strcpy(resultado, "Deshace el efecto de una funcion");

        else if (strstr(t, "composicion"))
            strcpy(resultado, "(f o g)(x)=f(g(x))");

        else if (strstr(t, "inyectiva"))
            strcpy(resultado, "Entradas distintas dan salidas distintas");

        else if (strstr(t, "sobreyectiva"))
            strcpy(resultado, "Todo elemento del codominio es alcanzado");

        else if (strstr(t, "biyectiva"))
            strcpy(resultado, "Es inyectiva y sobreyectiva");

        else if (strstr(t, "creciente"))
            strcpy(resultado, "Aumenta cuando aumenta x");

        else if (strstr(t, "decreciente"))
            strcpy(resultado, "Disminuye cuando aumenta x");

        else if (strstr(t, "periodica"))
            strcpy(resultado, "Repite valores tras cierto periodo");

        else if (strstr(t, "par"))
            strcpy(resultado, "f(-x)=f(x)");

        else if (strstr(t, "impar"))
            strcpy(resultado, "f(-x)=-f(x)");

        else if (strstr(t, "cero de funcion"))
            strcpy(resultado, "Valor x donde f(x)=0");
        else if (strstr(t, "geometria"))
            strcpy(resultado, "Estudia formas, medidas y posiciones");

        else if (strstr(t, "punto"))
            strcpy(resultado, "Ubicacion sin dimensiones");

        else if (strstr(t, "recta"))
            strcpy(resultado, "Linea infinita en ambas direcciones");

        else if (strstr(t, "segmento"))
            strcpy(resultado, "Parte de una recta entre dos puntos");

        else if (strstr(t, "semirrecta"))
            strcpy(resultado, "Recta con un punto inicial");

        else if (strstr(t, "angulo"))
            strcpy(resultado, "Figura formada por dos rayos");

        else if (strstr(t, "grado angular"))
            strcpy(resultado, "Unidad comun para medir angulos");

        else if (strstr(t, "angulo recto"))
            strcpy(resultado, "Mide 90 grados");

        else if (strstr(t, "angulo agudo"))
            strcpy(resultado, "Mide menos de 90 grados");

        else if (strstr(t, "angulo obtuso"))
            strcpy(resultado, "Mide mas de 90 y menos de 180");

        else if (strstr(t, "angulo llano"))
            strcpy(resultado, "Mide 180 grados");

        else if (strstr(t, "angulo completo"))
            strcpy(resultado, "Mide 360 grados");

        else if (strstr(t, "perimetro"))
            strcpy(resultado, "Suma de los lados de una figura");

        else if (strstr(t, "area"))
            strcpy(resultado, "Medida de una superficie");

        else if (strstr(t, "volumen"))
            strcpy(resultado, "Espacio ocupado por un cuerpo");

        else if (strstr(t, "distancia"))
            strcpy(resultado, "Medida entre dos puntos");

        else if (strstr(t, "paralelas"))
            strcpy(resultado, "Rectas que nunca se intersectan");

        else if (strstr(t, "perpendiculares"))
            strcpy(resultado, "Rectas que forman 90 grados");

        else if (strstr(t, "congruencia"))
            strcpy(resultado, "Misma forma y mismo tamano");

        else if (strstr(t, "semejanza"))
            strcpy(resultado, "Misma forma con escala proporcional");

        else if (strstr(t, "simetria"))
            strcpy(resultado, "Correspondencia respecto a una transformacion");

        else if (strstr(t, "transformacion"))
            strcpy(resultado, "Cambio geometrico de una figura");

        else if (strstr(t, "traslacion"))
            strcpy(resultado, "Desplazamiento sin cambiar forma");

        else if (strstr(t, "rotacion"))
            strcpy(resultado, "Giro alrededor de un punto");

        else if (strstr(t, "reflexion"))
            strcpy(resultado, "Simetria respecto a una recta");

        else if (strstr(t, "escala"))
            strcpy(resultado, "Multiplicacion de las dimensiones");
        else if (strstr(t, "triangulo"))
            strcpy(resultado, "Poligono de tres lados");

        else if (strstr(t, "triangulo equilatero"))
            strcpy(resultado, "Tres lados iguales");

        else if (strstr(t, "triangulo isosceles"))
            strcpy(resultado, "Dos lados iguales");

        else if (strstr(t, "triangulo escaleno"))
            strcpy(resultado, "Todos sus lados diferentes");

        else if (strstr(t, "triangulo rectangulo"))
            strcpy(resultado, "Tiene un angulo de 90 grados");

        else if (strstr(t, "hipotenusa"))
            strcpy(resultado, "Lado opuesto al angulo recto");

        else if (strstr(t, "cateto"))
            strcpy(resultado, "Lado que forma el angulo recto");

        else if (strstr(t, "pitagoras"))
            strcpy(resultado, "a^2+b^2=c^2");

        else if (strstr(t, "teorema de pitagoras"))
            strcpy(resultado, "Relacion entre lados de triangulo recto");

        else if (strstr(t, "area triangulo"))
            strcpy(resultado, "A=b*h/2");

        else if (strstr(t, "altura triangulo"))
            strcpy(resultado, "Perpendicular desde vertice a base");

        else if (strstr(t, "mediana"))
            strcpy(resultado, "Une vertice con punto medio opuesto");

        else if (strstr(t, "bisectriz"))
            strcpy(resultado, "Divide un angulo en dos iguales");

        else if (strstr(t, "baricentro"))
            strcpy(resultado, "Interseccion de las medianas");

        else if (strstr(t, "ortocentro"))
            strcpy(resultado, "Interseccion de las alturas");

        else if (strstr(t, "circuncentro"))
            strcpy(resultado, "Centro de la circunferencia circunscrita");

        else if (strstr(t, "incentro"))
            strcpy(resultado, "Centro de la circunferencia inscrita");

        else if (strstr(t, "suma angulos triangulo"))
            strcpy(resultado, "Siempre es 180 grados");

        else if (strstr(t, "heron"))
            strcpy(resultado, "A=raiz(s(s-a)(s-b)(s-c))");

        else if (strstr(t, "semiperimetro"))
            strcpy(resultado, "s=(a+b+c)/2");

        else if (strstr(t, "ley de senos"))
            strcpy(resultado, "a/senA=b/senB=c/senC");

        else if (strstr(t, "ley de cosenos"))
            strcpy(resultado, "c^2=a^2+b^2-2ab cosC");
        else if (strstr(t, "poligono"))
            strcpy(resultado, "Figura plana cerrada con lados");

        else if (strstr(t, "cuadrado"))
            strcpy(resultado, "Cuatro lados iguales y cuatro angulos rectos");

        else if (strstr(t, "rectangulo"))
            strcpy(resultado, "Cuatro angulos rectos");

        else if (strstr(t, "rombo"))
            strcpy(resultado, "Cuatro lados iguales");

        else if (strstr(t, "romboide"))
            strcpy(resultado, "Paralelogramo con lados opuestos iguales");

        else if (strstr(t, "trapecio"))
            strcpy(resultado, "Tiene al menos un par de lados paralelos");

        else if (strstr(t, "paralelogramo"))
            strcpy(resultado, "Dos pares de lados paralelos");

        else if (strstr(t, "pentagono"))
            strcpy(resultado, "Poligono de cinco lados");

        else if (strstr(t, "hexagono"))
            strcpy(resultado, "Poligono de seis lados");

        else if (strstr(t, "heptagono"))
            strcpy(resultado, "Poligono de siete lados");

        else if (strstr(t, "octagono"))
            strcpy(resultado, "Poligono de ocho lados");

        else if (strstr(t, "decagono"))
            strcpy(resultado, "Poligono de diez lados");

        else if (strstr(t, "suma angulos internos"))
            strcpy(resultado, "(n-2)*180 grados");

        else if (strstr(t, "angulo interno"))
            strcpy(resultado, "Angulo formado dentro del poligono");

        else if (strstr(t, "circunferencia"))
            strcpy(resultado, "Conjunto de puntos a igual distancia");

        else if (strstr(t, "circulo"))
            strcpy(resultado, "Region interior de una circunferencia");

        else if (strstr(t, "radio"))
            strcpy(resultado, "Distancia del centro a la circunferencia");

        else if (strstr(t, "diametro"))
            strcpy(resultado, "Diametro=2*radio");

        else if (strstr(t, "longitud circunferencia"))
            strcpy(resultado, "L=2*pi*r");

        else if (strstr(t, "area circulo"))
            strcpy(resultado, "A=pi*r^2");

        else if (strstr(t, "cuerda"))
            strcpy(resultado, "Segmento con extremos en la circunferencia");

        else if (strstr(t, "secante"))
            strcpy(resultado, "Recta que corta una circunferencia");

        else if (strstr(t, "tangente"))
            strcpy(resultado, "Recta que toca en un solo punto");
        else if (strstr(t, "cubo"))
            strcpy(resultado, "Seis caras cuadradas iguales");

        else if (strstr(t, "area cubo"))
            strcpy(resultado, "A=6a^2");

        else if (strstr(t, "volumen cubo"))
            strcpy(resultado, "V=a^3");

        else if (strstr(t, "prisma"))
            strcpy(resultado, "Dos bases paralelas y congruentes");

        else if (strstr(t, "volumen prisma"))
            strcpy(resultado, "V=area base*altura");

        else if (strstr(t, "cilindro"))
            strcpy(resultado, "Dos bases circulares paralelas");

        else if (strstr(t, "volumen cilindro"))
            strcpy(resultado, "V=pi*r^2*h");

        else if (strstr(t, "cono"))
            strcpy(resultado, "Cuerpo con base circular y vertice");

        else if (strstr(t, "volumen cono"))
            strcpy(resultado, "V=pi*r^2*h/3");

        else if (strstr(t, "esfera"))
            strcpy(resultado, "Cuerpo donde todos los puntos equidistan");

        else if (strstr(t, "volumen esfera"))
            strcpy(resultado, "V=4*pi*r^3/3");

        else if (strstr(t, "area esfera"))
            strcpy(resultado, "A=4*pi*r^2");

        else if (strstr(t, "piramide"))
            strcpy(resultado, "Base poligonal y caras triangulares");

        else if (strstr(t, "volumen piramide"))
            strcpy(resultado, "V=area base*altura/3");

        else if (strstr(t, "apotema"))
            strcpy(resultado, "Altura de una cara regular");

        else if (strstr(t, "superficie"))
            strcpy(resultado, "Medida total de una superficie");

        else if (strstr(t, "area lateral"))
            strcpy(resultado, "Area de las caras laterales");

        else if (strstr(t, "area total"))
            strcpy(resultado, "Suma de areas de todas las caras");

        else if (strstr(t, "generatriz"))
            strcpy(resultado, "Segmento lateral de cono o cilindro");
        else if (strstr(t, "trigonometria"))
            strcpy(resultado, "Estudia relaciones entre angulos y lados");

        else if (strstr(t, "seno"))
            strcpy(resultado, "sen(x)=opuesto/hipotenusa");

        else if (strstr(t, "sin"))
            strcpy(resultado, "Seno de un angulo");

        else if (strstr(t, "coseno"))
            strcpy(resultado, "cos(x)=adyacente/hipotenusa");

        else if (strstr(t, "cos"))
            strcpy(resultado, "Coseno de un angulo");

        else if (strstr(t, "tangente"))
            strcpy(resultado, "tan(x)=opuesto/adyacente");

        else if (strstr(t, "tan"))
            strcpy(resultado, "Tangente de un angulo");

        else if (strstr(t, "cotangente"))
            strcpy(resultado, "cot(x)=1/tan(x)");

        else if (strstr(t, "secante trigonometrica"))
            strcpy(resultado, "sec(x)=1/cos(x)");

        else if (strstr(t, "cosecante"))
            strcpy(resultado, "csc(x)=1/sin(x)");

        else if (strstr(t, "identidad trigonometrica"))
            strcpy(resultado, "sen^2x+cos^2x=1");

        else if (strstr(t, "identidad fundamental"))
            strcpy(resultado, "sen^2(x)+cos^2(x)=1");

        else if (strstr(t, "identidad tangente"))
            strcpy(resultado, "tan(x)=sen(x)/cos(x)");

        else if (strstr(t, "radian"))
            strcpy(resultado, "180 grados equivalen a pi radianes");

        else if (strstr(t, "grados a radianes"))
            strcpy(resultado, "Multiplica grados por pi/180");

        else if (strstr(t, "radianes a grados"))
            strcpy(resultado, "Multiplica radianes por 180/pi");

        else if (strstr(t, "circulo unitario"))
            strcpy(resultado, "Tiene radio 1 y centro en origen");

        else if (strstr(t, "arcoseno"))
            strcpy(resultado, "Funcion inversa del seno");

        else if (strstr(t, "arccoseno"))
            strcpy(resultado, "Funcion inversa del coseno");

        else if (strstr(t, "arcotangente"))
            strcpy(resultado, "Funcion inversa de la tangente");

        else if (strstr(t, "periodo seno"))
            strcpy(resultado, "El periodo del seno es 2*pi");

        else if (strstr(t, "periodo coseno"))
            strcpy(resultado, "El periodo del coseno es 2*pi");

        else if (strstr(t, "periodo tangente"))
            strcpy(resultado, "El periodo de tan es pi");
        else if (strstr(t, "suma de angulos"))
            strcpy(resultado, "sen(a+b)=sena cosb+cosa senb");

        else if (strstr(t, "seno suma"))
            strcpy(resultado, "sen(a+b)=sena cosb+cosa senb");

        else if (strstr(t, "seno diferencia"))
            strcpy(resultado, "sen(a-b)=sena cosb-cosa senb");

        else if (strstr(t, "coseno suma"))
            strcpy(resultado, "cos(a+b)=cosa cosb-sena senb");

        else if (strstr(t, "coseno diferencia"))
            strcpy(resultado, "cos(a-b)=cosa cosb+sena senb");

        else if (strstr(t, "tangente suma"))
            strcpy(resultado, "tan(a+b)=(tana+tanb)/(1-tana*tanb)");

        else if (strstr(t, "angulo doble"))
            strcpy(resultado, "sen(2x)=2senx cosx");

        else if (strstr(t, "coseno doble"))
            strcpy(resultado, "cos(2x)=cos^2x-sen^2x");

        else if (strstr(t, "tangente doble"))
            strcpy(resultado, "tan(2x)=2tanx/(1-tan^2x)");

        else if (strstr(t, "medio angulo"))
            strcpy(resultado, "Usa formulas con x/2");

        else if (strstr(t, "pitagorica trigonometrica"))
            strcpy(resultado, "1+tan^2x=sec^2x");

        else if (strstr(t, "secante cuadrada"))
            strcpy(resultado, "sec^2x=1+tan^2x");

        else if (strstr(t, "cosecante cuadrada"))
            strcpy(resultado, "csc^2x=1+cot^2x");

        else if (strstr(t, "identidad par"))
            strcpy(resultado, "cos(-x)=cos(x)");

        else if (strstr(t, "seno impar"))
            strcpy(resultado, "sen(-x)=-sen(x)");

        else if (strstr(t, "coseno par"))
            strcpy(resultado, "cos(-x)=cos(x)");

        else if (strstr(t, "tangente impar"))
            strcpy(resultado, "tan(-x)=-tan(x)");

        else if (strstr(t, "angulo complementario"))
            strcpy(resultado, "Suman 90 grados");

        else if (strstr(t, "angulo suplementario"))
            strcpy(resultado, "Suman 180 grados");
        else if (strstr(t, "geometria analitica"))
            strcpy(resultado, "Estudia geometria con coordenadas");

        else if (strstr(t, "plano cartesiano"))
            strcpy(resultado, "Usa ejes x e y");

        else if (strstr(t, "eje x"))
            strcpy(resultado, "Eje horizontal del plano");

        else if (strstr(t, "eje y"))
            strcpy(resultado, "Eje vertical del plano");

        else if (strstr(t, "origen"))
            strcpy(resultado, "Punto (0,0)");

        else if (strstr(t, "coordenadas"))
            strcpy(resultado, "Ubican puntos mediante numeros");

        else if (strstr(t, "pendiente"))
            strcpy(resultado, "m=(y2-y1)/(x2-x1)");

        else if (strstr(t, "recta pendiente"))
            strcpy(resultado, "y=mx+b");

        else if (strstr(t, "ordenada al origen"))
            strcpy(resultado, "Valor b en y=mx+b");

        else if (strstr(t, "abscisa"))
            strcpy(resultado, "Coordenada x");

        else if (strstr(t, "ordenada"))
            strcpy(resultado, "Coordenada y");

        else if (strstr(t, "punto medio"))
            strcpy(resultado, "M=((x1+x2)/2,(y1+y2)/2)");

        else if (strstr(t, "distancia entre puntos"))
            strcpy(resultado, "d=raiz((x2-x1)^2+(y2-y1)^2)");

        else if (strstr(t, "ecuacion recta"))
            strcpy(resultado, "y-y1=m(x-x1)");

        else if (strstr(t, "rectas paralelas"))
            strcpy(resultado, "Tienen la misma pendiente");

        else if (strstr(t, "rectas perpendiculares"))
            strcpy(resultado, "Pendientes cuyo producto es -1");

        else if (strstr(t, "circunferencia analitica"))
            strcpy(resultado, "(x-h)^2+(y-k)^2=r^2");

        else if (strstr(t, "parabola"))
            strcpy(resultado, "Curva definida por foco y directriz");

        else if (strstr(t, "elipse"))
            strcpy(resultado, "Suma de distancias a dos focos constante");

        else if (strstr(t, "hiperbola"))
            strcpy(resultado, "Diferencia de distancias a focos constante");

        else if (strstr(t, "foco"))
            strcpy(resultado, "Punto que define ciertas conicas");

        else if (strstr(t, "directriz"))
            strcpy(resultado, "Recta usada para definir una conica");
        else if (strstr(t, "calculo"))
            strcpy(resultado, "Estudia cambio, limites y acumulacion");

        else if (strstr(t, "limite"))
            strcpy(resultado, "Valor al que tiende una expresion");

        else if (strstr(t, "limite infinito"))
            strcpy(resultado, "Analiza comportamiento sin cota");

        else if (strstr(t, "limite lateral"))
            strcpy(resultado, "Limite desde un solo lado");

        else if (strstr(t, "izquierda limite"))
            strcpy(resultado, "Se aproxima usando valores menores");

        else if (strstr(t, "derecha limite"))
            strcpy(resultado, "Se aproxima usando valores mayores");

        else if (strstr(t, "continuidad"))
            strcpy(resultado, "No presenta salto en un punto");

        else if (strstr(t, "continua"))
            strcpy(resultado, "lim f(x)=f(a) en x=a");

        else if (strstr(t, "discontinuidad"))
            strcpy(resultado, "Punto donde falla la continuidad");

        else if (strstr(t, "indeterminacion 0/0"))
            strcpy(resultado, "Puede requerir simplificacion");

        else if (strstr(t, "infinito sobre infinito"))
            strcpy(resultado, "Forma indeterminada comun");

        else if (strstr(t, "teorema del sandwich"))
            strcpy(resultado, "Acota una funcion entre dos funciones");

        else if (strstr(t, "limite suma"))
            strcpy(resultado, "El limite de una suma es la suma");

        else if (strstr(t, "limite producto"))
            strcpy(resultado, "El limite del producto es el producto");

        else if (strstr(t, "limite cociente"))
            strcpy(resultado, "Se divide si el denominador no tiende a cero");

        else if (strstr(t, "limite constante"))
            strcpy(resultado, "El limite de una constante es ella misma");

        else if (strstr(t, "limite polinomio"))
            strcpy(resultado, "Se puede evaluar directamente");

        else if (strstr(t, "limite exponencial"))
            strcpy(resultado, "Depende de la base y el comportamiento");

        else if (strstr(t, "limite trigonometrico"))
            strcpy(resultado, "lim sen(x)/x=1 cuando x tiende a 0");
        else if (strstr(t, "derivada"))
            strcpy(resultado, "Mide la tasa de cambio instantanea");

        else if (strstr(t, "derivada geometrica"))
            strcpy(resultado, "Pendiente de la recta tangente");

        else if (strstr(t, "derivada de constante"))
            strcpy(resultado, "La derivada de C es 0");

        else if (strstr(t, "derivada de x"))
            strcpy(resultado, "La derivada de x es 1");

        else if (strstr(t, "derivada potencia"))
            strcpy(resultado, "d(x^n)=n*x^(n-1)");

        else if (strstr(t, "derivada seno"))
            strcpy(resultado, "d(senx)=cosx");

        else if (strstr(t, "derivada coseno"))
            strcpy(resultado, "d(cosx)=-senx");

        else if (strstr(t, "derivada tangente"))
            strcpy(resultado, "d(tanx)=sec^2x");

        else if (strstr(t, "derivada exponencial"))
            strcpy(resultado, "d(e^x)=e^x");

        else if (strstr(t, "derivada logaritmo"))
            strcpy(resultado, "d(lnx)=1/x");

        else if (strstr(t, "derivada log"))
            strcpy(resultado, "d(log10x)=1/(x ln10)");

        else if (strstr(t, "regla producto"))
            strcpy(resultado, "(fg)'=f'g+fg'");

        else if (strstr(t, "regla cociente"))
            strcpy(resultado, "(f/g)'=(f'g-fg')/g^2");

        else if (strstr(t, "regla cadena"))
            strcpy(resultado, "(f(g))'=f'(g)g'");

        else if (strstr(t, "derivada segunda"))
            strcpy(resultado, Derivada);

        else if (strstr(t, "segunda derivada"))
            strcpy(resultado, "Mide cambio de la primera derivada");

        else if (strstr(t, "derivada tercera"))
            strcpy(resultado, "Derivada aplicada tres veces");

        else if (strstr(t, "derivada parcial"))
            strcpy(resultado, "Derivada respecto a una variable");

        else if (strstr(t, "gradiente"))
            strcpy(resultado, "Vector de derivadas parciales");

        else if (strstr(t, "derivada implicita"))
            strcpy(resultado, "Deriva una ecuacion con variables mezcladas");
        else if (strstr(t, "punto critico"))
            strcpy(resultado, "Punto donde f'=0 o no existe");

        else if (strstr(t, "maximo local"))
            strcpy(resultado, "Valor mayor que los vecinos cercanos");

        else if (strstr(t, "minimo local"))
            strcpy(resultado, "Valor menor que los vecinos cercanos");

        else if (strstr(t, "maximo absoluto"))
            strcpy(resultado, "Mayor valor de toda la funcion");

        else if (strstr(t, "minimo absoluto"))
            strcpy(resultado, "Menor valor de toda la funcion");

        else if (strstr(t, "crecimiento derivada"))
            strcpy(resultado, "f'>0 indica crecimiento");

        else if (strstr(t, "decrecimiento derivada"))
            strcpy(resultado, "f'<0 indica decrecimiento");

        else if (strstr(t, "concavidad"))
            strcpy(resultado, "Describe como se curva una funcion");

        else if (strstr(t, "concava arriba"))
            strcpy(resultado, "Usualmente f''>0");

        else if (strstr(t, "concava abajo"))
            strcpy(resultado, "Usualmente f''<0");

        else if (strstr(t, "punto inflexion"))
            strcpy(resultado, "Cambia el sentido de concavidad");

        else if (strstr(t, "optimizacion"))
            strcpy(resultado, "Busca maximizar o minimizar una cantidad");

        else if (strstr(t, "tasa de cambio"))
            strcpy(resultado, "Cambio de salida respecto a entrada");

        else if (strstr(t, "tangente"))
            strcpy(resultado, "Recta que aproxima localmente una curva");

        else if (strstr(t, "aproximacion lineal"))
            strcpy(resultado, "f(x) aprox f(a)+f'(a)(x-a)");

        else if (strstr(t, "diferencial"))
            strcpy(resultado, "Pequena variacion aproximada");

        else if (strstr(t, "newton"))
            strcpy(resultado, "Metodo iterativo para hallar raices");

        else if (strstr(t, "metodo de newton"))
            strcpy(resultado, "x_nuevo=x-f(x)/f'(x)");

        else if (strstr(t, "velocidad"))
            strcpy(resultado, "Derivada de la posicion");

        else if (strstr(t, "aceleracion"))
            strcpy(resultado, "Segunda derivada de la posicion");
        else if (strstr(t, "sucesion"))
            strcpy(resultado, "Lista ordenada de numeros");

        else if (strstr(t, "termino general"))
            strcpy(resultado, "Formula que determina a_n");

        else if (strstr(t, "sucesion aritmetica"))
            strcpy(resultado, "Tiene diferencia constante");

        else if (strstr(t, "diferencia comun"))
            strcpy(resultado, "Resta entre terminos consecutivos");

        else if (strstr(t, "termino aritmetico"))
            strcpy(resultado, "a_n=a_1+(n-1)d");

        else if (strstr(t, "suma aritmetica"))
            strcpy(resultado, "S_n=n(a_1+a_n)/2");

        else if (strstr(t, "sucesion geometrica"))
            strcpy(resultado, "Tiene razon constante");

        else if (strstr(t, "razon geometrica"))
            strcpy(resultado, "Cociente entre terminos consecutivos");

        else if (strstr(t, "termino geometrico"))
            strcpy(resultado, "a_n=a_1*r^(n-1)");

        else if (strstr(t, "suma geometrica"))
            strcpy(resultado, "S_n=a_1(1-r^n)/(1-r)");

        else if (strstr(t, "suma infinita"))
            strcpy(resultado, "Puede converger si los terminos disminuyen");

        else if (strstr(t, "geometrica infinita"))
            strcpy(resultado, "S=a_1/(1-r), si |r|<1");

        else if (strstr(t, "fibonacci"))
            strcpy(resultado, "Cada termino suma los dos anteriores");

        else if (strstr(t, "sucesion fibonacci"))
            strcpy(resultado, "0,1,1,2,3,5,8,...");

        else if (strstr(t, "recurrencia"))
            strcpy(resultado, "Define terminos usando otros anteriores");

        else if (strstr(t, "secuencia"))
            strcpy(resultado, "Otra forma de decir sucesion");

        else if (strstr(t, "monotona"))
            strcpy(resultado, "Siempre crece o siempre decrece");

        else if (strstr(t, "acotada"))
            strcpy(resultado, "Tiene limites superior o inferior");
        else if (strstr(t, "serie"))
            strcpy(resultado, "Suma de los terminos de una sucesion");

        else if (strstr(t, "serie geometrica"))
            strcpy(resultado, "Suma basada en potencias de una razon");

        else if (strstr(t, "serie armonica"))
            strcpy(resultado, "1+1/2+1/3+1/4+...");

        else if (strstr(t, "serie convergente"))
            strcpy(resultado, "Sus sumas parciales tienden a un valor");

        else if (strstr(t, "serie divergente"))
            strcpy(resultado, "No tiene suma finita limite");

        else if (strstr(t, "convergencia"))
            strcpy(resultado, "Acercamiento a un valor limite");

        else if (strstr(t, "sumatoria"))
            strcpy(resultado, "Usa sigma para representar sumas");

        else if (strstr(t, "sigma"))
            strcpy(resultado, "Simbolo usado para sumatorias");

        else if (strstr(t, "termino general serie"))
            strcpy(resultado, "Formula para el termino n-esimo");

        else if (strstr(t, "serie telescopica"))
            strcpy(resultado, "Muchos terminos se cancelan");

        else if (strstr(t, "serie alternante"))
            strcpy(resultado, "Sus signos alternan");

        else if (strstr(t, "serie de potencias"))
            strcpy(resultado, "Suma de potencias de una variable");

        else if (strstr(t, "taylor"))
            strcpy(resultado, "Representa funciones mediante potencias");

        else if (strstr(t, "maclaurin"))
            strcpy(resultado, "Serie de Taylor centrada en cero");

        else if (strstr(t, "serie taylor"))
            strcpy(resultado, "Usa derivadas evaluadas en un punto");

        else if (strstr(t, "radio convergencia"))
            strcpy(resultado, "Distancia donde una serie converge");

        else if (strstr(t, "intervalo convergencia"))
            strcpy(resultado, "Valores de x donde converge la serie");

        else if (strstr(t, "criterio comparacion"))
            strcpy(resultado, "Compara una serie con otra conocida");

        else if (strstr(t, "criterio integral"))
            strcpy(resultado, "Relaciona series con integrales");
        else if (strstr(t, "probabilidad"))
            strcpy(resultado, "Mide la posibilidad de un evento");

        else if (strstr(t, "experimento"))
            strcpy(resultado, "Proceso que produce resultados");

        else if (strstr(t, "evento"))
            strcpy(resultado, "Conjunto de resultados posibles");

        else if (strstr(t, "espacio muestral"))
            strcpy(resultado, "Todos los resultados posibles");

        else if (strstr(t, "evento seguro"))
            strcpy(resultado, "Probabilidad igual a 1");

        else if (strstr(t, "evento imposible"))
            strcpy(resultado, "Probabilidad igual a 0");

        else if (strstr(t, "eventos independientes"))
            strcpy(resultado, "Uno no cambia la probabilidad del otro");

        else if (strstr(t, "eventos dependientes"))
            strcpy(resultado, "Uno modifica la probabilidad del otro");

        else if (strstr(t, "probabilidad condicional"))
            strcpy(resultado, "P(A|B)=P(A y B)/P(B)");

        else if (strstr(t, "regla suma"))
            strcpy(resultado, "P(A o B)=P(A)+P(B)-P(A y B)");

        else if (strstr(t, "regla producto"))
            strcpy(resultado, "P(A y B)=P(A)P(B|A)");

        else if (strstr(t, "complemento"))
            strcpy(resultado, "P(no A)=1-P(A)");

        else if (strstr(t, "bayes"))
            strcpy(resultado, "Invierte probabilidades condicionales");

        else if (strstr(t, "teorema de bayes"))
            strcpy(resultado, "P(A|B)=P(B|A)P(A)/P(B)");

        else if (strstr(t, "probabilidad uniforme"))
            strcpy(resultado, "Casos favorables sobre casos posibles");

        else if (strstr(t, "azar"))
            strcpy(resultado, "Resultado no determinado con certeza");

        else if (strstr(t, "variable aleatoria"))
            strcpy(resultado, "Asigna numeros a resultados aleatorios");

        else if (strstr(t, "esperanza"))
            strcpy(resultado, "Promedio teorico ponderado");

        else if (strstr(t, "valor esperado"))
            strcpy(resultado, "E[X] suma x por su probabilidad");

        else if (strstr(t, "varianza"))
            strcpy(resultado, "Mide dispersion respecto a la media");
        else if (strstr(t, "combinatoria"))
            strcpy(resultado, "Cuenta formas de seleccionar u ordenar");

        else if (strstr(t, "permutacion"))
            strcpy(resultado, "Importa el orden de los elementos");

        else if (strstr(t, "combinacion"))
            strcpy(resultado, "No importa el orden");

        else if (strstr(t, "permutacion simple"))
            strcpy(resultado, "P(n,r)=n!/(n-r)!");

        else if (strstr(t, "combinacion simple"))
            strcpy(resultado, "C(n,r)=n!/(r!(n-r)!)");

        else if (strstr(t, "factorial"))
            strcpy(resultado, "n!=n(n-1)...1");

        else if (strstr(t, "factorial cero"))
            strcpy(resultado, "0!=1");

        else if (strstr(t, "coeficiente binomial"))
            strcpy(resultado, "Representa combinaciones C(n,k)");

        else if (strstr(t, "binomio de newton"))
            strcpy(resultado, "Expande potencias de un binomio");

        else if (strstr(t, "triangulo pascal"))
            strcpy(resultado, "Contiene coeficientes binomiales");

        else if (strstr(t, "principio multiplicativo"))
            strcpy(resultado, "Multiplica opciones independientes");

        else if (strstr(t, "principio aditivo"))
            strcpy(resultado, "Suma opciones mutuamente excluyentes");

        else if (strstr(t, "pigeonhole"))
            strcpy(resultado, "Principio de casillas o palomar");

        else if (strstr(t, "principio del palomar"))
            strcpy(resultado, "Mas objetos que casillas implica repeticion");

        else if (strstr(t, "permutacion circular"))
            strcpy(resultado, "Arreglos alrededor de un circulo");

        else if (strstr(t, "multiconjunto"))
            strcpy(resultado, "Conjunto que permite repeticiones");

        else if (strstr(t, "coeficiente"))
            strcpy(resultado, "Factor numerico de un termino");

        else if (strstr(t, "orden importa"))
            strcpy(resultado, "Usualmente indica una permutacion");

        else if (strstr(t, "orden no importa"))
            strcpy(resultado, "Usualmente indica una combinacion");
        else if (strstr(t, "estadistica"))
            strcpy(resultado, "Analiza datos y su variabilidad");

        else if (strstr(t, "media"))
            strcpy(resultado, "Suma de datos dividida entre cantidad");

        else if (strstr(t, "promedio"))
            strcpy(resultado, "Generalmente es la media aritmetica");

        else if (strstr(t, "mediana"))
            strcpy(resultado, "Dato central al ordenar");

        else if (strstr(t, "moda"))
            strcpy(resultado, "Dato que aparece mas veces");

        else if (strstr(t, "rango estadistico"))
            strcpy(resultado, "Maximo menos minimo");

        else if (strstr(t, "varianza estadistica"))
            strcpy(resultado, "Media de cuadrados de desviaciones");

        else if (strstr(t, "desviacion estandar"))
            strcpy(resultado, "Raiz cuadrada de la varianza");

        else if (strstr(t, "poblacion"))
            strcpy(resultado, "Conjunto total que se estudia");

        else if (strstr(t, "muestra"))
            strcpy(resultado, "Subconjunto de una poblacion");

        else if (strstr(t, "frecuencia"))
            strcpy(resultado, "Numero de veces que aparece un dato");

        else if (strstr(t, "frecuencia relativa"))
            strcpy(resultado, "Proporcion respecto al total");

        else if (strstr(t, "histograma"))
            strcpy(resultado, "Grafica de frecuencias por intervalos");

        else if (strstr(t, "media ponderada"))
            strcpy(resultado, "Promedio donde cada dato tiene peso");

        else if (strstr(t, "percentil"))
            strcpy(resultado, "Indica posicion relativa de un dato");

        else if (strstr(t, "cuartil"))
            strcpy(resultado, "Divide datos ordenados en cuatro partes");

        else if (strstr(t, "rango intercuartil"))
            strcpy(resultado, "Q3-Q1");

        else if (strstr(t, "correlacion"))
            strcpy(resultado, "Mide relacion lineal entre variables");

        else if (strstr(t, "covarianza"))
            strcpy(resultado, "Mide variacion conjunta");

        else if (strstr(t, "regresion"))
            strcpy(resultado, "Modela relacion entre variables");
        else if (strstr(t, "distribucion normal"))
            strcpy(resultado, "Distribucion simetrica en forma de campana");

        else if (strstr(t, "campana de gauss"))
            strcpy(resultado, "Otra forma de llamar a la normal");

        else if (strstr(t, "media normal"))
            strcpy(resultado, "Centro de la distribucion normal");

        else if (strstr(t, "desviacion normal"))
            strcpy(resultado, "Controla la dispersion de la campana");

        else if (strstr(t, "z score"))
            strcpy(resultado, "Mide desviaciones respecto a la media");

        else if (strstr(t, "binomial"))
            strcpy(resultado, "Cuenta exitos en ensayos independientes");

        else if (strstr(t, "distribucion binomial"))
            strcpy(resultado, "Usa n ensayos y probabilidad p");

        else if (strstr(t, "poisson"))
            strcpy(resultado, "Modela conteos de eventos");

        else if (strstr(t, "distribucion uniforme"))
            strcpy(resultado, "Todos los valores tienen igual densidad");

        else if (strstr(t, "distribucion geometrica"))
            strcpy(resultado, "Cuenta ensayos hasta primer exito");

        else if (strstr(t, "muestra aleatoria"))
            strcpy(resultado, "Muestra elegida mediante azar");

        else if (strstr(t, "estimador"))
            strcpy(resultado, "Cantidad usada para estimar un parametro");

        else if (strstr(t, "parametro"))
            strcpy(resultado, "Caracteristica numerica de una poblacion");

        else if (strstr(t, "hipotesis"))
            strcpy(resultado, "Afirmacion que se puede contrastar");

        else if (strstr(t, "hipotesis nula"))
            strcpy(resultado, "Hipotesis base de una prueba");

        else if (strstr(t, "nivel de significancia"))
            strcpy(resultado, "Probabilidad limite para rechazar H0");

        else if (strstr(t, "valor p"))
            strcpy(resultado, "Evidencia contra la hipotesis nula");

        else if (strstr(t, "intervalo confianza"))
            strcpy(resultado, "Rango usado para estimar un parametro");

        else if (strstr(t, "error estandar"))
            strcpy(resultado, "Mide variabilidad de un estimador");
        else if (strstr(t, "matriz"))
            strcpy(resultado, "Arreglo rectangular de numeros");

        else if (strstr(t, "fila matriz"))
            strcpy(resultado, "Linea horizontal de una matriz");

        else if (strstr(t, "columna matriz"))
            strcpy(resultado, "Linea vertical de una matriz");

        else if (strstr(t, "matriz cuadrada"))
            strcpy(resultado, "Tiene igual numero de filas y columnas");

        else if (strstr(t, "matriz identidad"))
            strcpy(resultado, "Diagonal principal con unos");

        else if (strstr(t, "matriz cero"))
            strcpy(resultado, "Todos sus elementos son cero");

        else if (strstr(t, "matriz diagonal"))
            strcpy(resultado, "Fuera de diagonal todos son cero");

        else if (strstr(t, "matriz triangular"))
            strcpy(resultado, "Tiene ceros sobre o bajo diagonal");

        else if (strstr(t, "determinante"))
            strcpy(resultado, "Numero asociado a una matriz cuadrada");

        else if (strstr(t, "determinante 2x2"))
            strcpy(resultado, "ad-bc");

        else if (strstr(t, "matriz inversa"))
            strcpy(resultado, "A*A^-1=I");

        else if (strstr(t, "matrices iguales"))
            strcpy(resultado, "Misma dimension y mismos elementos");

        else if (strstr(t, "suma matrices"))
            strcpy(resultado, "Se suman elementos correspondientes");

        else if (strstr(t, "producto matrices"))
            strcpy(resultado, "Fila por columna");

        else if (strstr(t, "transpuesta"))
            strcpy(resultado, "Intercambia filas y columnas");

        else if (strstr(t, "rango matriz"))
            strcpy(resultado, "Numero maximo de filas independientes");

        else if (strstr(t, "traza"))
            strcpy(resultado, "Suma de la diagonal principal");

        else if (strstr(t, "eigenvalor"))
            strcpy(resultado, "Escalar asociado a un eigenvector");

        else if (strstr(t, "valor propio"))
            strcpy(resultado, "Otra forma de decir eigenvalor");

        else if (strstr(t, "vector propio"))
            strcpy(resultado, "Vector cuya direccion no cambia");
        else if (strstr(t, "vector"))
            strcpy(resultado, "Cantidad con magnitud y direccion");

        else if (strstr(t, "vector cero"))
            strcpy(resultado, "Vector con todas sus componentes cero");

        else if (strstr(t, "magnitud vector"))
            strcpy(resultado, "Longitud del vector");

        else if (strstr(t, "norma"))
            strcpy(resultado, "Medida de longitud de un vector");

        else if (strstr(t, "vector unitario"))
            strcpy(resultado, "Vector de norma 1");

        else if (strstr(t, "vector posicion"))
            strcpy(resultado, "Va desde el origen hasta un punto");

        else if (strstr(t, "producto punto"))
            strcpy(resultado, "a·b=sum(ai*bi)");

        else if (strstr(t, "producto escalar"))
            strcpy(resultado, "Otro nombre para producto punto");

        else if (strstr(t, "producto cruz"))
            strcpy(resultado, "Vector perpendicular a dos vectores");

        else if (strstr(t, "producto vectorial"))
            strcpy(resultado, "Otro nombre para producto cruz");

        else if (strstr(t, "ortogonal"))
            strcpy(resultado, "Producto punto igual a cero");

        else if (strstr(t, "paralelo vector"))
            strcpy(resultado, "Vectores con direcciones proporcionales");

        else if (strstr(t, "combinacion lineal"))
            strcpy(resultado, "Suma de vectores multiplicados por escalares");

        else if (strstr(t, "independencia lineal"))
            strcpy(resultado, "Ningun vector depende de los otros");

        else if (strstr(t, "base vectorial"))
            strcpy(resultado, "Conjunto independiente que genera el espacio");

        else if (strstr(t, "dimension"))
            strcpy(resultado, "Numero de vectores de una base");

        else if (strstr(t, "espacio vectorial"))
            strcpy(resultado, "Conjunto cerrado bajo operaciones vectoriales");

        else if (strstr(t, "escalar"))
            strcpy(resultado, "Numero que multiplica un vector");
        else if (strstr(t, "teoria de numeros"))
            strcpy(resultado, "Estudia propiedades de los enteros");

        else if (strstr(t, "numero primo"))
            strcpy(resultado, "Tiene exactamente dos divisores positivos");

        else if (strstr(t, "primo"))
            strcpy(resultado, "Solo divisible por 1 y por si mismo");

        else if (strstr(t, "numero compuesto"))
            strcpy(resultado, "Tiene mas de dos divisores positivos");

        else if (strstr(t, "factor primo"))
            strcpy(resultado, "Factor que es un numero primo");

        else if (strstr(t, "factorizacion prima"))
            strcpy(resultado, "Producto de factores primos");

        else if (strstr(t, "mcd"))
            strcpy(resultado, "Maximo comun divisor");

        else if (strstr(t, "maximo comun divisor"))
            strcpy(resultado, "Mayor divisor comun");

        else if (strstr(t, "mcm"))
            strcpy(resultado, "Minimo comun multiplo");

        else if (strstr(t, "minimo comun multiplo"))
            strcpy(resultado, "Menor multiplo comun positivo");

        else if (strstr(t, "euclides"))
            strcpy(resultado, "Algoritmo para hallar el MCD");

        else if (strstr(t, "algoritmo de euclides"))
            strcpy(resultado, "Usa divisiones sucesivas");

        else if (strstr(t, "divisibilidad"))
            strcpy(resultado, "Determina si un entero divide a otro");

        else if (strstr(t, "paridad"))
            strcpy(resultado, "Clasifica numeros como pares o impares");

        else if (strstr(t, "numero par"))
            strcpy(resultado, "Es divisible entre 2");

        else if (strstr(t, "numero impar"))
            strcpy(resultado, "No es divisible entre 2");

        else if (strstr(t, "congruencia modular"))
            strcpy(resultado, "a es congruente con b modulo n");

        else if (strstr(t, "modulo"))
            strcpy(resultado, "Resto de una division entera");

        else if (strstr(t, "aritmetica modular"))
            strcpy(resultado, "Opera con restos modulo n");

        else if (strstr(t, "fermat"))
            strcpy(resultado, "Teorema importante sobre potencias modulares");

        else if (strstr(t, "euler teoria numeros"))
            strcpy(resultado, "Relaciona coprimos y potencias modulares");
        else if (strstr(t, "teoria de conjuntos"))
            strcpy(resultado, "Estudia colecciones de elementos");

        else if (strstr(t, "conjunto"))
            strcpy(resultado, "Coleccion bien definida de elementos");

        else if (strstr(t, "elemento"))
            strcpy(resultado, "Objeto perteneciente a un conjunto");

        else if (strstr(t, "subconjunto"))
            strcpy(resultado, "Conjunto cuyos elementos estan contenidos");

        else if (strstr(t, "union"))
            strcpy(resultado, "Elementos que estan en A o B");

        else if (strstr(t, "interseccion"))
            strcpy(resultado, "Elementos comunes a A y B");

        else if (strstr(t, "complemento conjunto"))
            strcpy(resultado, "Elementos que no pertenecen al conjunto");

        else if (strstr(t, "conjunto vacio"))
            strcpy(resultado, "Conjunto sin elementos");

        else if (strstr(t, "conjunto universal"))
            strcpy(resultado, "Contiene todos los elementos del contexto");

        else if (strstr(t, "producto cartesiano"))
            strcpy(resultado, "Conjunto de pares ordenados");

        else if (strstr(t, "cardinalidad"))
            strcpy(resultado, "Numero de elementos de un conjunto");

        else if (strstr(t, "logica"))
            strcpy(resultado, "Estudia proposiciones y razonamiento");

        else if (strstr(t, "proposicion"))
            strcpy(resultado, "Enunciado que puede ser verdadero o falso");

        else if (strstr(t, "negacion"))
            strcpy(resultado, "Cambia verdadero por falso");

        else if (strstr(t, "conjuncion"))
            strcpy(resultado, "A y B son verdaderas");

        else if (strstr(t, "disyuncion"))
            strcpy(resultado, "A o B");

        else if (strstr(t, "implicacion"))
            strcpy(resultado, "Si A entonces B");

        else if (strstr(t, "bicondicional"))
            strcpy(resultado, "A si y solo si B");

        else if (strstr(t, "tautologia"))
            strcpy(resultado, "Proposicion siempre verdadera");

        else if (strstr(t, "contradiccion"))
            strcpy(resultado, "Proposicion siempre falsa");

        else if (strstr(t, "cuantificador"))
            strcpy(resultado, "Expresa cantidad o alcance logico");
        else if (strstr(t, "cuantificador universal"))
            strcpy(resultado, "Para todo elemento");

        else if (strstr(t, "cuantificador existencial"))
            strcpy(resultado, "Existe al menos un elemento");

        else if (strstr(t, "demostracion"))
            strcpy(resultado, "Argumento que prueba una afirmacion");

        else if (strstr(t, "axioma"))
            strcpy(resultado, "Proposicion aceptada como base");

        else if (strstr(t, "teorema"))
            strcpy(resultado, "Proposicion demostrada matematicamente");

        else if (strstr(t, "lema"))
            strcpy(resultado, "Resultado auxiliar para una demostracion");

        else if (strstr(t, "corolario"))
            strcpy(resultado, "Resultado que se deduce de un teorema");

        else if (strstr(t, "conjetura"))
            strcpy(resultado, "Afirmacion propuesta sin demostracion completa");

        else if (strstr(t, "induccion matematica"))
            strcpy(resultado, "Prueba base y paso inductivo");

        else if (strstr(t, "induccion"))
            strcpy(resultado, "Metodo para probar casos infinitos");

        else if (strstr(t, "recursion"))
            strcpy(resultado, "Define algo usando versiones anteriores");

        else if (strstr(t, "algoritmo"))
            strcpy(resultado, "Procedimiento finito para resolver problemas");

        else if (strstr(t, "grafo"))
            strcpy(resultado, "Conjunto de vertices y aristas");

        else if (strstr(t, "vertice grafo"))
            strcpy(resultado, "Nodo de un grafo");

        else if (strstr(t, "arista"))
            strcpy(resultado, "Conexion entre vertices");

        else if (strstr(t, "camino"))
            strcpy(resultado, "Secuencia de vertices conectados");

        else if (strstr(t, "ciclo"))
            strcpy(resultado, "Camino que regresa al inicio");

        else if (strstr(t, "arbol matematico"))
            strcpy(resultado, "Grafo conectado sin ciclos");

        else if (strstr(t, "grado vertice"))
            strcpy(resultado, "Numero de aristas incidentes");

        else if (strstr(t, "matematicas discretas"))
            strcpy(resultado, "Estudian estructuras finitas o contables");
        else if (strstr(t, "teoria de numeros avanzada"))
            strcpy(resultado, "Estudia enteros y sus estructuras");

        else if (strstr(t, "analisis"))
            strcpy(resultado, "Estudia limites continuidad y calculo");

        else if (strstr(t, "algebra lineal"))
            strcpy(resultado, "Estudia vectores matrices y espacios");

        else if (strstr(t, "algebra abstracta"))
            strcpy(resultado, "Estudia estructuras algebraicas");

        else if (strstr(t, "grupo"))
            strcpy(resultado, "Conjunto con una operacion y axiomas");

        else if (strstr(t, "anillo"))
            strcpy(resultado, "Estructura con suma y multiplicacion");

        else if (strstr(t, "cuerpo"))
            strcpy(resultado, "Estructura con operaciones y divisiones");

        else if (strstr(t, "topologia"))
            strcpy(resultado, "Estudia propiedades bajo deformaciones continuas");

        else if (strstr(t, "espacio metrico"))
            strcpy(resultado, "Conjunto equipado con una distancia");

        else if (strstr(t, "distancia metrica"))
            strcpy(resultado, "Funcion que mide separacion entre puntos");

        else if (strstr(t, "matematica"))
            strcpy(resultado, "Ciencia de cantidades formas y relaciones");

        else
            strcpy(resultado, "Tema matematico no encontrado");

        expresion[0] = 0;
        cursor = 0;
        actualizar();
        return;
    }

    double r = evaluar(expresion);
    snprintf(resultado, 63, "%.10g", r);
    expresion[0] = 0;
    cursor = 0;
    actualizar();
}

// ===================== BOTONES TÁCTILES =====================

typedef struct {
    int x, y, w, h;
    const char *label;
    const char *normal;
    const char *shift;
    int especial;
} Boton;

Boton botones[] = {
    {  2,  2, 48, 24, "sin",  "sin",  "asin", 0 },
    { 52,  2, 48, 24, "cos",  "cos",  "acos", 0 },
    {102,  2, 48, 24, "tan",  "tan",  "atan", 0 },
    {152,  2, 48, 24, "log",  "log",  "10^",  0 },
    {202,  2, 52, 24, "ln",   "ln",   "e^",   0 },

    {  2, 28, 48, 24, "x^2",  "^2",   "^3",   0 },
    { 52, 28, 48, 24, "sqrt", "sqrt", "cbrt", 0 },
    {102, 28, 48, 24, "^",    "^",    "^-1",  0 },
    {152, 28, 48, 24, "(",    "(",    "!",    0 },
    {202, 28, 52, 24, ")",    ")",    "pi",   0 },

    {  2, 54, 48, 24, "7", "7", "7", 0 },
    { 52, 54, 48, 24, "8", "8", "8", 0 },
    {102, 54, 48, 24, "9", "9", "9", 0 },
    {152, 54, 48, 24, "DEL", "", "", 2 },
    {202, 54, 52, 24, "AC",  "", "", 1 },

    {  2, 80, 48, 24, "4", "4", "4", 0 },
    { 52, 80, 48, 24, "5", "5", "5", 0 },
    {102, 80, 48, 24, "6", "6", "6", 0 },
    {152, 80, 48, 24, "x", "*", "*", 0 },
    {202, 80, 52, 24, "/", "/", "/", 0 },

    {  2,106, 48, 24, "1", "1", "1", 0 },
    { 52,106, 48, 24, "2", "2", "2", 0 },
    {102,106, 48, 24, "3", "3", "3", 0 },
    {152,106, 48, 24, "+", "+", "+", 0 },
    {202,106, 52, 24, "-", "-", "-", 0 },

    {  2,132, 48, 24, "0", "0", "0", 0 },
    { 52,132, 48, 24, ".", ".", ".", 0 },
    {102,132, 48, 24, "SHIFT","", "", 4 },
    {152,132,102, 24, "=", "", "", 3 },
};

#define NBOT (sizeof(botones)/sizeof(Boton))
void dibujar_teclado() {
    consoleSelect(&bottom);
    consoleClear();

    for (int i = 0; i < NBOT; i++) {
        Boton *b = &botones[i];
        const char *txt = (shift && b->shift[0]) ? b->shift : b->label;

        int fila = b->y / 13 + 1;
        int col  = b->x / 8 + 1;

        printf("\x1b[%d;%dH%-5s", fila, col, txt);
    }

    if (shift) {
        printf("\x1b[19;1H[SHIFT ACTIVO]");
    }
}

int tocar(int tx, int ty) {
    for (int i = 0; i < NBOT; i++) {
        Boton *b = &botones[i];

        if (tx >= b->x && tx < b->x + b->w &&
            ty >= b->y && ty < b->y + b->h)
            return i;
    }

    return -1;
}

void procesar(int idx) {
    if (idx < 0) return;

    Boton *b = &botones[idx];

    switch (b->especial) {
        case 1:
            limpiar();
            break;

        case 2:
            borrar();
            break;

        case 3:
            calcular();
            break;

        case 4:
            shift = !shift;
            dibujar_teclado();
            actualizar();
            break;

        default: {
            const char *ins =
                (shift && b->shift[0]) ? b->shift : b->normal;

            if (ins[0])
                agregar(ins);

            if (shift) {
                shift = 0;
                dibujar_teclado();
            }

            break;
        }
    }
}

// ===================== MAIN =====================

int main(void) {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&top, 3, BgType_Text4bpp,
                BgSize_T_256x256, 31, 0, true, true);

    consoleInit(&bottom, 3, BgType_Text4bpp,
                BgSize_T_256x256, 31, 0, false, true);

    actualizar();
    dibujar_teclado();

    touchPosition touch;

    while (1) {
        swiWaitForVBlank();
        scanKeys();

        u32 k = keysDown();

        if (k & KEY_TOUCH) {
            touchRead(&touch);
            procesar(tocar(touch.px, touch.py));
            dibujar_teclado();
        }

        if (k & KEY_A) calcular();
        if (k & KEY_B) borrar();
        if (k & KEY_X) limpiar();

        if (k & KEY_Y) {
            shift = !shift;
            dibujar_teclado();
            actualizar();
        }

        if (k & KEY_START)
            break;
    }

    return 0;
}
