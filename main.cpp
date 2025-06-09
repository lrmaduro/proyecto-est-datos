
#include <iostream>
#include <string>
#include <time.h>
#include <random>
#include <cctype>

using namespace std;

struct Logro
{
    string nombre;
    string descripcion;
    char rango; // 'O', 'P' o 'B'
    int puntosBase;
    string fecha; // dd-mm-aaaa
    Logro *sig;
};

struct Mision
{
    string titulo;
    string descripcion;
    string requisito;
    int puntos;
    int nivelRequisito;
    int id;
    Mision *prox;
};

struct Usuario
{
    string nombre;
    string alias;
    int nivel;
    int puntosTotales;
    Logro *listaLogros;
    Mision *listaMisiones;
    Usuario *sig;
};



struct Acertijo {
    string acertijo;
    string respuesta;
};


char aMayus(char c)
{
    return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

string aMinus(string str)
{
    for (int i = 0; i < str.length(); i++) 
    {
        str[i] = tolower(str[i]);
    }

    return str;
}

double multiplicador(char r)
{
    const double MULT_BRONCE = 1.0;
    const double MULT_PLATA = 1.2;
    const double MULT_ORO = 1.5;

    switch (aMayus(r))
    {
    case 'O':
        return MULT_ORO;
    case 'P':
        return MULT_PLATA;
    default:
        return MULT_BRONCE;
    }
}



string fechaHoy()
{
    time_t t = time(nullptr);
    tm *f = localtime(&t);
    int d = f->tm_mday, m = 1 + f->tm_mon, y = 1900 + f->tm_year;
    string s;
    s += (d < 10 ? "0" : "") + to_string(d) + "-";
    s += (m < 10 ? "0" : "") + to_string(m) + "-";
    s += to_string(y);
    return s;
}

void recalcularNivel(Usuario *u)
{
    const int PUNTOS_NIVEL = 1000;
    u->nivel = u->puntosTotales / PUNTOS_NIVEL + 1;
}

int aleatorio(int minimo, int maximo)
{
    static default_random_engine gen(static_cast<unsigned>(time(nullptr)));
    uniform_int_distribution<int> dist(minimo, maximo);
    return dist(gen);
}

Logro *nuevoLogro(const string &n, const string &d, char r, int pts)
{
    return new Logro{n, d, r, pts, fechaHoy(), nullptr};
}

Usuario *nuevoUsuario(const string &n, const string &a)
{
    return new Usuario{n, a, 1, 0, nullptr, nullptr, nullptr};
}

Mision *nuevaMision(const string &titulo, const string &descripcion, const string &req, int pts, int nivel, int id)
{
    return new Mision{titulo, descripcion, req, pts, nivel, id, nullptr};
}


void insertarUsuario(Usuario *&cab, const string &n, const string &a)
{
    if (!cab)
    {
        cab = nuevoUsuario(n, a);
        return;
    }
    Usuario *posicion = cab;
    while (posicion->sig)
        posicion = posicion->sig;
    posicion->sig = nuevoUsuario(n, a);
}

Usuario *buscarUsuario(Usuario *cab, const string &a)
{
    while (cab && cab->alias != a)
        cab = cab->sig;
    return cab;
}

void eliminarUsuario(Usuario *&cab, const string &a)
{
    Usuario *mover = cab, *ant = nullptr;
    while (mover && mover->alias != a)
    {
        ant = mover;
        mover = mover->sig;
    }
    if (!mover)
    {
        cout << "Alias no encontrado\n";
        return;
    }

    Logro *l = mover->listaLogros;
    Mision *m = mover->listaMisiones;

    while (l)
    {
        Logro *tmp = l;
        l = l->sig;
        delete tmp;
    }

    while (m)
    {
        Mision *tmpM = m;
        m = m->prox;
        delete tmpM;
    }

    if (!ant)
        cab = mover->sig;
    else
        ant->sig = mover->sig;
    delete mover;
    cout << "Perfil eliminado\n";
}

void agregarLogro(Usuario* u, const string& nombre, const string& descripcion, char nuevoRango, int  puntosBase)
{
    int  valorNuevo;
    double multNuevo;

    if      (nuevoRango == 'O') { valorNuevo = 3; multNuevo = 1.5; }
    else if (nuevoRango == 'P') { valorNuevo = 2; multNuevo = 1.2; }
    else                        { valorNuevo = 1; multNuevo = 1.0; }

    Logro* l = u->listaLogros;
    while (l && l->nombre != nombre)
        l = l->sig;

    if (!l)
    {
        Logro* nuevo = new Logro;
        nuevo->nombre       = nombre;
        nuevo->descripcion  = descripcion;
        nuevo->rango        = nuevoRango;
        nuevo->puntosBase   = puntosBase;
        nuevo->fecha        = fechaHoy();
        nuevo->sig          = u->listaLogros;    
        u->listaLogros      = nuevo;

        u->puntosTotales += int(puntosBase * multNuevo);
        recalcularNivel(u);
        cout << "Logro [" << nombre << "] creado (rango "
             << nuevoRango << ").\n";
        return;
    }

    int valorActual;
    double multActual;

    if      (l->rango == 'O') { valorActual = 3; multActual = 1.5; }
    else if (l->rango == 'P') { valorActual = 2; multActual = 1.2; }
    else                       { valorActual = 1; multActual = 1.0; }

    if (valorNuevo > valorActual)
    {
        int puntosAntes = int(l->puntosBase * multActual);
        int puntosDesp  = int(l->puntosBase * multNuevo);

        l->rango = nuevoRango;
        l->fecha = fechaHoy();

        u->puntosTotales += (puntosDesp - puntosAntes);
        recalcularNivel(u);

        cout << "Logro [" << nombre << "] mejorado a rango "
             << nuevoRango << ".\n";
    }
}



void agregarMision(Usuario *u, const string &titulo, const string &desc, const string &req, int pts, int nivel, int id)
{
    if (!u->listaMisiones)
    {
        u->listaMisiones = nuevaMision(titulo, desc, req, pts, nivel, id);
        return;
    }

    Mision *mover = u->listaMisiones;

    while (mover->prox != nullptr)
    {
        mover = mover->prox;
    }

    mover->prox = nuevaMision(titulo, desc, req, pts, nivel, id);
}

Mision *elegirMision(int id)
{
    string t, d, r;
    char rang;
    int pts, nivel;
    switch (id)
    {
        case 0:
            t = "Ganar en PPT!";
            d = "Ganarle a la computadora en Piedra, Papel o Tijeras.";
            r = "Ser nivel 1.";
            pts = 500;
            nivel = 1;
            break;

         case 1:
            t = "Adivinar el acertijo (facil)!";
            d = "Adivinar el acertijo random en la dificultad facil.";
            r = "Ser nivel 1.";
            pts = 500;
            nivel = 1;
            break;

        case 2:
            t = "Adivinar la Secuencia Aritmetica!";
            d = "Completar el minijuego de adivinar el siguiente numero en la secuencia.";
            r = "Ser nivel 2.";
            pts = 500;
            nivel = 2;
            break;

        case 3:
            t = "Adivinar el acertijo (Dificil)!";
            d = "Adivinar el acertijo random en la dificultad dificil.";
            r = "Ser nivel 2.";
            pts = 500;
            nivel = 2;
            break;

        case 4:
            t = "Ganar en Suma 21!";
            d = "Ganarle a la computadora en el juego de sumar 21.";
            r = "Ser nivel 2.";
            pts = 500;
            nivel = 2;
            break;

        case 5:
            t = "Ganar jugando al Ahoracado!";
            d = "Adivinar la palabra en el juego del ahorcado.";
            r = "Ser nivel 3.";
            pts = 500;
            nivel = 3;
            break;

        case 6:
            t = "Resolver el problema matematico!";
            d = "Resolver la operacion matematica planteada.";
            r = "Ser nivel 3.";
            pts = 500;
            nivel = 3;
            break;
        
        case 7:
            t = "Ganar en el juego de Pasos Exactos!";
            d = "Completar el juego de pasos exactos.";
            r = "Ser nivel 3.";
            pts = 500;
            nivel = 3;
            break;
    }

    return nuevaMision(t,d,r,pts,nivel,id);
}


void eliminarMision(Usuario* u, int id)
{
    Mision* mover    = u->listaMisiones;
    Mision* anterior = nullptr;


    while (mover && mover->id != id) {
        anterior = mover;
        mover    = mover->prox;
    }
    if (!mover) return;                  

    if (anterior)                        
        anterior->prox = mover->prox;
    else          
        u->listaMisiones = mover->prox;

    delete mover;
}



void inicializarMisiones(Usuario *u, const string &alias)
{
   Usuario *j = buscarUsuario(u, alias);
   Mision *m = j->listaMisiones, *n = nullptr;
   for (int i = 0; i < 8; i++)
   {
        n = elegirMision(i);
        string t = n->titulo, desc = n->descripcion, req = n->requisito;
        int nivel = n->nivelRequisito, ptos = n->puntos;
        agregarMision(j, t, desc, req, ptos, nivel, i);
   }
}

void mostrarMisiones(Usuario *cab, const string &a)
{
    Usuario *j = buscarUsuario(cab, a);
    if (!j)
    {
        cout << "El usuario no existe\n";
    }
    else
    {
        Mision *m = j->listaMisiones;
        while (m)
        {
            cout <<"Mision #" << m->id+1 << ": " << m->titulo << endl;
            cout <<"Descripcion: " << m->descripcion << endl;
            cout <<"Requisito: " << m->requisito << endl;
            cout <<"Nivel de requisito: " << m->nivelRequisito << endl;
            cout <<"Puntos a obtener: " << m->puntos << endl;
            cout<<endl; 
            m = m->prox;
        }    
    }
}

void mostrarUno(Usuario *u)
{
    cout << "\nJugador: " << u->nombre
         << " | Alias: " << u->alias
         << " | Nivel: " << u->nivel
         << " | Puntos: " << u->puntosTotales << "\n";
    for (Logro *l = u->listaLogros; l; l = l->sig)
    {
        cout << "  * " << l->nombre
             << " (" << l->descripcion << ") "
             << "R:" << l->rango
             << " PtsBase:" << l->puntosBase
             << " Fecha:" << l->fecha << "\n";
    }
}

void mostrarTodos(Usuario *cab)
{
    if (!cab)
    {
        cout << "No hay jugadores\n";
        return;
    }
    while (cab)
    {
        mostrarUno(cab);
        cab = cab->sig;
    }
}

void mostrarRanking(Usuario *cabeza)
{
    if (!cabeza)
    {
        cout << "Sin jugadores\n";
        return;
    }

    Usuario *primero = nullptr;
    Usuario *segundo = nullptr;
    Usuario *tercero = nullptr;

    Usuario *mover = cabeza;

    while (mover)
    {
        if (!primero || mover->puntosTotales > primero->puntosTotales)
        {
            tercero = segundo;
            segundo = primero;
            primero = mover;
        }
        else if (!segundo || mover->puntosTotales > segundo->puntosTotales)
        {
            tercero = segundo;
            segundo = mover;
        }
        else if (!tercero || mover->puntosTotales > tercero->puntosTotales)
        {
            tercero = mover;
        }
        mover = mover->sig;
    }

    cout << "\n=== PODIO ===\n";
    if (primero)
        cout << "#1 " << primero->alias << " (" << primero->puntosTotales << " pts)\n";
    if (segundo)
        cout << "#2 " << segundo->alias << " (" << segundo->puntosTotales << " pts)\n";
    if (tercero)
        cout << "#3 " << tercero->alias << " (" << tercero->puntosTotales << " pts)\n";

    cout << "\n=== RANKING COMPLETO ===\n";
    int lugar = 1;
    long puntajeAnterior = 2147483647; 
    while (true)
    {
        Usuario *mejor = nullptr;
        for (Usuario *p = cabeza; p; p = p->sig)
            if (p->puntosTotales < puntajeAnterior)
                if (!mejor || p->puntosTotales > mejor->puntosTotales)
                    mejor = p;

        if (!mejor)
            break;
        cout << lugar << ". " << mejor->alias
             << " | Puntos: " << mejor->puntosTotales
             << " | Nivel: " << mejor->nivel << "\n";
        puntajeAnterior = mejor->puntosTotales;
        ++lugar;
    }
}

bool nivelSuficiente(Usuario *u, int req)
{
    if (u->nivel >= req)
        return true;
    cout << "Se requiere nivel " << req << " para jugar\n";
    return false;
}
char elegirPPT()
{
    int n = aleatorio(1, 3);
    if (n == 1)
        return 't';
    else if (n == 2)
        return 'p';
    else
        return 'l';
}

char rangoPorIntentos(int c) 
{
    if (c <= 1)
    {
        return 'O';
    } else if (c > 1 && c <= 5)
    {
        return 'P';
    }
    else
    {
        return 'B';
    }
}

void juegoPPT(Usuario* jugador)
{
    if (jugador->nivel < 1) {
        cout << "Se requiere nivel 1\n";
        return;
    }

    int  intentos      = 0;
    char repetir       = 's';

    while (repetir == 's' || repetir == 'S')
    {
        ++intentos;

        cout << "\nPiedra (p), Papel (l), Tijeras (t): ";
        char eleccion;
        cin  >> eleccion;
        cin.ignore();

        char cpu = elegirPPT();
        cout << "CPU elige "
             << (cpu=='p'?"Piedra": cpu=='l'?"Papel":"Tijeras") << '\n';

        bool gana =
            (eleccion=='p'&&cpu=='t') ||
            (eleccion=='l'&&cpu=='p') ||
            (eleccion=='t'&&cpu=='l');

        if (eleccion == cpu) {
            cout << "Empate. Sin logro.\n";
        }
        else if (gana) {
            cout << "¡Ganaste!\n";

            char rango =
                (intentos == 1)       ? 'O' :
                (intentos <= 5)       ? 'P' : 'B';

            agregarLogro(jugador,
                         "Piedra Papel Tijeras",
                         "Ganar al CPU en PPT",
                         rango,
                         500);

            if (intentos == 1)        // solo la primera victoria
                eliminarMision(jugador, 0);
        }
        else {
            cout << "Perdiste. Sin logro.\n";
        }

        cout << "¿Jugar otra vez? (s/n): ";
        cin  >> repetir;
        cin.ignore();
    }
}



Acertijo pedirAcertijoFacil()
{
    int n = aleatorio(1, 3);
    Acertijo acertijoNuevo;

    switch (n)
    {
        case 1:
            acertijoNuevo.acertijo = "Oro no es, Plata no es. Que es? ";
            acertijoNuevo.respuesta = "Platano";
            break;
        case 2:
            acertijoNuevo.acertijo = "Que tiene manos pero no puede aplaudir? ";
            acertijoNuevo.respuesta = "Reloj";
            break;
        case 3:
            acertijoNuevo.acertijo = "Te la digo, te la digo y te la vuelvo a repetir. Te la digo 5 veces y no sabes que decir. Que es? ";
            acertijoNuevo.respuesta = "Tela";
            break;
    }

    return acertijoNuevo;
}

void acertijoFacil(Usuario *j)
{
    if (j->nivel < 1)
    {
        cout << "Se requiere nivel 1\n";
        return;
    }

    Acertijo acertijo = pedirAcertijoFacil();

    cout << "\nAcertijo: '" << acertijo.acertijo << "'\nTu respuesta: ";
    string r;
    getline(cin, r);

    if (aMinus(r).compare(aMinus(acertijo.respuesta)))
    {
        cout << "Correcto!\n";
        agregarLogro(j, "Acertijo 1", acertijo.respuesta, 'O', 1000);
        eliminarMision(j, 1);

    }
    else
    {
        cout << "Incorrecto. Era '" << acertijo.respuesta << "'. Sin logro.\n";
    }
}


void juegoSecuencia(Usuario *j)
{
    if (j->nivel < 2)
    {
        cout << "Se requiere nivel 2\n";
        return;
    }

    int a = aleatorio(1, 10), d = aleatorio(2, 5);
    cout << "\nSecuencia: ";
    for (int k = 0; k < 4; k++)
        cout << a + k * d << " ";
    cout << "?\nSiguiente numero: ";
    int resp;
    cin >> resp;
    cin.ignore();

    if (resp == a + 4 * d)
    {
        cout << "Correcto!\n";
        agregarLogro(j, "Secuencia", "Numero siguiente", 'O', 1000);
        eliminarMision(j, 2);

    }
    else
    {
        cout << "Incorrecto. Era " << a + 4 * d << ". Sin logro.\n";
    }
}

Acertijo pedirAcertijoMedio()
{
    int n = aleatorio(1, 3);
    Acertijo acertijoNuevo;

    switch (n)
    {
        case 1:
            acertijoNuevo.acertijo = "Que no esta ni dentro ni fuera de la casa, pero es necesario para cualquier hogar? ";
            acertijoNuevo.respuesta = "Ventana";
            break;
        case 2:
            acertijoNuevo.acertijo = "Me mojo para secarte. Que soy? ";
            acertijoNuevo.respuesta = "Toalla";
            break;
        case 3:
            acertijoNuevo.acertijo = "No muchas personas me han pisado. Nunca me quedo llena por mucho tiempo. Tengo un lado oscuro. Quien soy? ";
            acertijoNuevo.respuesta = "Luna";
            break;
    }

    return acertijoNuevo;   
}

void acertijoNivel2(Usuario *j)
{
    if (j->nivel < 2)
    {
        cout << "Se requiere nivel 2\n";
        return;
    }

    int indice = aleatorio(0,2);

    Acertijo acertijo = pedirAcertijoMedio();

    cout << "\nAcertijo: '" << acertijo.acertijo << "'\nRespuesta: ";
    string r;
    getline(cin, r);
    if (aMinus(r).compare(aMinus(acertijo.respuesta)))
    {
        cout << "Correcto!\n";
        agregarLogro(j, "Acertijo 2", acertijo.respuesta, 'O', 1000);
        eliminarMision(j, 3);

    }
    else
    {
        cout << "Incorrecto. Era '" << acertijo.respuesta << "'. Sin logro.\n";
    }
}


void juego21(Usuario *j)
{
    if (j->nivel < 2)
    {
        cout << "Se requiere nivel 2\n";
        return;
    }

    int total = 0;
    bool turnoJugador = true;
    cout << "\nLlega a 21 sin pasarte. Sumas 1–3.\n";
    while (total < 21)
    {
        if (turnoJugador)
        {
            int n;
            do
            {
                cout << "Elige 1,2 o 3: ";
                cin >> n;
            } while (n < 1 || n > 3);
            cin.ignore();
            total += n;
        }
        else
        {
            int nCPU = aleatorio(1, 3);
            cout << "CPU suma " << nCPU << "\n";
            total += nCPU;
        }
        cout << "Total: " << total << "\n";
        if (total >= 21)
            break;
        turnoJugador = !turnoJugador;
    }

    bool gana = (!turnoJugador); 
    if (gana)
    {
        cout << "CPU se pasa. Ganas!\n";
        agregarLogro(j, "Suma 21", "Juego contra CPU", 'O', 100);
        eliminarMision(j, 4);

    }
    else
    {
        cout << "Te pasaste. Sin logro.\n";
    }
}

void juegoAhorcado(Usuario *j)
{
    if (j->nivel < 3)
    {
        cout << "Se requiere nivel 3\n";
        return;
    }

    const string pal[10] = {"casa", "gato", "perro", "agua", "avion",
                            "raton", "verde", "planta", "libro", "silla"};
    string palabra = pal[aleatorio(0, 9)], oculta(palabra.size(), '_');
    int vidas = 6;

    while (vidas > 0 && oculta != palabra)
    {
        cout << "\n"
             << oculta << "  Vidas:" << vidas << "\nLetra: ";
        char l;
        cin >> l;
        l = aMayus(l);
        cin.ignore();
        bool ok = false;
        for (size_t i = 0; i < palabra.size(); ++i)
            if (aMayus(palabra[i]) == l)
            {
                oculta[i] = palabra[i];
                ok = true;
            }
        if (!ok)
        {
            --vidas;
            cout << "Fallaste\n";
        }
    }

    if (oculta == palabra)
    {
        cout << "Ganaste! Era '" << palabra << "'\n";
        char rango = (vidas >= 4) ? 'O' : (vidas >= 2 ? 'P' : 'B');
        agregarLogro(j, "Ahorcado", "Adivinar palabra", rango, 1000);
        eliminarMision(j, 5);
    }
    else
    {
        cout << "Derrota. Era '" << palabra << "'. Sin logro.\n";
    }
}

void problemaMat(Usuario *j)
{
    if (j->nivel < 3)
    {
        cout << "Se requiere nivel 3\n";
        return;
    }

    int x = aleatorio(2, 8), y = aleatorio(3, 9), z = aleatorio(1, 5);
    cout << "\nPedro tiene " << x << " y compra " << y
         << ". Regala " << z << ". Cuantos quedan? ";
    int r;
    cin >> r;
    cin.ignore();
    if (r == x + y - z)
    {
        cout << "Correcto!\n";
        agregarLogro(j, "Problema Mat", "Calculo facil", 'O', 1000);
        eliminarMision(j, 6);
    }
    else
    {
        cout << "Incorrecto. Era " << x + y - z << ". Sin logro.\n";
    }
}

void juegoTresMov(Usuario *j)
{
    if (j->nivel < 3)
    {
        cout << "Se requiere nivel 3\n";
        return;
    }

    int meta = aleatorio(10, 18), c = 0;
    cout << "\nLlega exactamente a " << meta << " con pasos 1–3.\n";
    while (c < meta)
    {
        int s;
        cout << "Paso (1-3): ";
        cin >> s;
        cin.ignore();
        if (s < 1 || s > 3)
        {
            cout << "Solo 1–3\n";
            continue;
        }
        c += s;
        cout << "Cuenta: " << c << "\n";
        if (c == meta)
        {
            cout << "Exacto! Ganaste\n";
            agregarLogro(j, "Movimientos", "Meta exacta", 'O', 1000);
            eliminarMision(j, 7);
            return;
        }
        if (c > meta)
        {
            cout << "Te pasaste. Sin logro.\n";
            return;
        }
    }
}

void menuMinijuegos(Usuario *u)
{
    int op;
    do
    {
        cout << "\n--- MINIJUEGOS (Nivel " << u->nivel << ") ---\n"
             << "1) Piedra/Papel/Tijeras\n"
             << "2) Acertijo facil\n"
             << "3) Secuencia aritmetica\n"
             << "4) Acertijo nivel 2\n"
             << "5) Suma 21\n"
             << "6) Ahorcado\n"
             << "7) Problema matematico\n"
             << "8) Pasos exactos\n"
             << "0) Volver\nOpcion: ";
        cin >> op;
        cin.ignore();
        switch (op)
        {
        case 1:
            juegoPPT(u);
            break;
        case 2:
            acertijoFacil(u);
            break;
        case 3:
            juegoSecuencia(u);
            break;
        case 4:
            acertijoNivel2(u);
            break;
        case 5:
            juego21(u);
            break;
        case 6:
            juegoAhorcado(u);
            break;
        case 7:
            problemaMat(u);
            break;
        case 8:
            juegoTresMov(u);
            break;
        case 0:
            break;
        default:
            cout << "Opcion invalida\n";
        }
    } while (op != 0);
}

int main()
{
    Usuario *lista = nullptr;
    int op;

    do
    {
        cout << "\n===== MENU PRINCIPAL =====\n"
             << "1) Registrar jugador\n"
             << "2) Eliminar jugador\n"
             << "3) Agregar logro\n"
             << "4) Mostrar jugadores\n"
             << "5) Ranking y podio\n"
             << "6) Minijuegos\n"
             << "7) Ver misiones pendientes\n"
             << "8) Ver perfil\n"
             << "0) Salir\n"
             << "Opcion: ";
        cin >> op;
        cin.ignore();

        string nom, ali, n, d;
        Usuario *u;
        switch (op)
        {
            case 1:
            {
                cout << "Nombre: ";
                getline(cin, nom);
                cout << "Alias : ";
                getline(cin, ali);
                insertarUsuario(lista, nom, ali);
                inicializarMisiones(lista, ali);
                break;
            }
            case 2:
            {
                cout << "Alias a eliminar: ";
                getline(cin, ali);
                eliminarUsuario(lista, ali);
                break;
            }
            case 3:
            {
                cout << "Alias: ";
                getline(cin, ali);
                Usuario *u = buscarUsuario(lista, ali);
                if (!u)
                {
                    cout << "Alias no encontrado, debe registrarse primero.\n";
                    break;
                }
                char r;
                int pts;
                cout << "Nombre logro : ";
                getline(cin, n);
                cout << "Descripcion  : ";
                getline(cin, d);
                cout << "Rango (O/P/B): ";
                cin >> r;
                cin.ignore();
                cout << "Puntos base  : ";
                cin >> pts;
                cin.ignore();
                agregarLogro(u, n, d, r, pts);
                break;
            }
            case 4:
            {
                mostrarTodos(lista);
                break;
            }
            case 5:
            {
                mostrarRanking(lista);
                break;
            }
            case 6:
            {
                cout << "Alias: ";
                getline(cin, ali);
                u = buscarUsuario(lista, ali);
                if (u)
                    menuMinijuegos(u);
                else
                    cout << "Alias no encontrado, debe registrarse primero.\n";
                break;
            }
            case 7:
            {
                cout << "Alias: ";
                getline(cin, ali);
                u = buscarUsuario(lista, ali);
                if (u)
                    mostrarMisiones(u, ali);
                else
                    cout << "Alias no encontrado, debe registrarse primero.\n";
                break;
            }
            case 8:
            {
                cout << "Alias: ";
                getline(cin, ali);
                u = buscarUsuario(lista, ali);
                if (u)
                    mostrarUno(u);
                else
                    cout << "Alias no encontrado, debe registrarse primero.\n";
                break;
            }
            case 0:
            {
                cout << "Hasta luego!\n";
                break;
            }
            default:
            {
                cout << "Opcion invalida\n";
            }
        }
    } while (op != 0);

    while (lista)
    {
        Usuario *u = lista;
        lista = lista->sig;
        for (Logro *l = u->listaLogros; l;)
        {
            Logro *t = l;
            l = l->sig;
            delete t;
        }
        delete u;
    }
    return 0;
}
