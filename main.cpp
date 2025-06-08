
#include <iostream>
#include <string>
#include <time.h>
#include <random>

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

struct Usuario
{
    string nombre;
    string alias;
    int nivel;
    int puntosTotales;
    Logro *listaLogros;
    Usuario *sig;
};

char aMayus(char c)
{
    return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
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
    static mt19937 gen(static_cast<unsigned>(time(nullptr)));
    uniform_int_distribution<int> dist(minimo, maximo);
    return dist(gen);
}

Logro *nuevoLogro(const string &n, const string &d, char r, int pts)
{
    return new Logro{n, d, r, pts, fechaHoy(), nullptr};
}

Usuario *nuevoUsuario(const string &n, const string &a)
{
    return new Usuario{n, a, 1, 0, nullptr, nullptr};
}

/*================  Lista enlazada de usuarios  ==============*/
void insertarUsuario(Usuario *&cab, const string &n, const string &a)
{
    if (!cab)
    {
        cab = nuevoUsuario(n, a);
        return;
    }
    Usuario *p = cab;
    while (p->sig)
        p = p->sig;
    p->sig = nuevoUsuario(n, a);
}

Usuario *buscarUsuario(Usuario *cab, const string &a)
{
    while (cab && cab->alias != a)
        cab = cab->sig;
    return cab;
}

void eliminarUsuario(Usuario *&cab, const string &a)
{
    Usuario *p = cab, *ant = nullptr;
    while (p && p->alias != a)
    {
        ant = p;
        p = p->sig;
    }
    if (!p)
    {
        cout << "Alias no encontrado\n";
        return;
    }

    for (Logro *l = p->listaLogros; l;)
    {
        Logro *tmp = l;
        l = l->sig;
        delete tmp;
    }
    if (!ant)
        cab = p->sig;
    else
        ant->sig = p->sig;
    delete p;
    cout << "Perfil eliminado\n";
}

void agregarLogro(Usuario *u, const string &nombre, const string &descripcion, char nuevoRango, int puntosBase)
{
    auto valor = [](char r)
    { return (r == 'O') ? 3 : (r == 'P') ? 2
                                         : 1; };
    auto mult = [](char r)
    { return (r == 'O') ? 1.5 : (r == 'P') ? 1.2
                                           : 1.0; };

    Logro *l = u->listaLogros;
    while (l && l->nombre != nombre)
        l = l->sig;

    if (!l)
    {
        /* ——— crear logro ——— */
        Logro *nuevo = new Logro{nombre, descripcion,
                                 nuevoRango, puntosBase,
                                 fechaHoy(), u->listaLogros};
        u->listaLogros = nuevo;
        u->puntosTotales += int(puntosBase * mult(nuevoRango));
        recalcularNivel(u);
        cout << "Logro [" << nombre << "] creado en rango "
             << nuevoRango << ".\n";
    }
    else if (valor(nuevoRango) > valor(l->rango))
    {
        /* ——— mejora de rango ——— */
        int antes = int(l->puntosBase * mult(l->rango));
        int ahora = int(l->puntosBase * mult(nuevoRango));
        l->rango = nuevoRango;
        l->fecha = fechaHoy();
        u->puntosTotales += (ahora - antes);
        recalcularNivel(u);
        cout << "Logro [" << nombre << "] mejorado a rango "
             << nuevoRango << ".\n";
    }
    /* si es igual o peor, no cambia nada */
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
    for (; cab; cab = cab->sig)
        mostrarUno(cab);
}

void mostrarRanking(Usuario *cabeza)
{
    if (!cabeza)
    {
        cout << "Sin jugadores\n";
        return;
    }

    /* top3 */
    Usuario *primero = nullptr;
    Usuario *segundo = nullptr;
    Usuario *tercero = nullptr;

    for (Usuario *p = cabeza; p; p = p->sig)
    {
        if (!primero || p->puntosTotales > primero->puntosTotales)
        {
            tercero = segundo;
            segundo = primero;
            primero = p;
        }
        else if (!segundo || p->puntosTotales > segundo->puntosTotales)
        {
            tercero = segundo;
            segundo = p;
        }
        else if (!tercero || p->puntosTotales > tercero->puntosTotales)
        {
            tercero = p;
        }
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

void juegoPPT(Usuario *j)
{
    if (j->nivel < 1)
    {
        cout << "Se requiere nivel 1\n";
        return;
    }

    char otra = 's';
    char opc[3] = {'p', 'l', 't'}; 
    while (otra == 's' || otra == 'S')
    {
        cout << "\nPiedra (p), Papel (l), Tijeras (t): ";
        char elec;
        cin >> elec;
        cin.ignore();
        char cpu = opc[aleatorio(0, 2)];

        cout << "CPU elige " << (cpu == 'p' ? "Piedra" : cpu == 'l' ? "Papel"
                                                                    : "Tijeras")
             << "\n";

        bool gana = (elec == 'p' && cpu == 't') || (elec == 'l' && cpu == 'p') || (elec == 't' && cpu == 'l');
        if (elec == cpu)
            cout << "Empate. Sin logro.\n";
        else if (gana)
        {
            cout << "Ganaste!\n";
            agregarLogro(j, "PPT", "Piedra Papel Tijeras", 'O', 1000);
        }
        else
            cout << "Perdiste. Sin logro.\n";

        cout << "¿Jugar otra vez? (s/n): ";
        cin >> otra;
        cin.ignore();
    }
}

/* Acertijo fácil */
void acertijoFacil(Usuario *j)
{
    if (j->nivel < 1)
    {
        cout << "Se requiere nivel 1\n";
        return;
    }

    cout << "\nAcertijo: 'Que tiene manos pero no puede aplaudir?'\nTu respuesta: ";
    string r;
    getline(cin, r);
    if (r == "reloj" || r == "Reloj")
    {
        cout << "Correcto!\n";
        agregarLogro(j, "Acertijo 1", "Reloj", 'O', 1000);
    }
    else
    {
        cout << "Incorrecto. Era 'reloj'. Sin logro.\n";
    }
}

/* =============== NIVEL 2 =============== */

/* Secuencia aritmética */
void juegoSecuencia(Usuario *j)
{
    if (j->nivel < 2)
    {
        cout << "Se requiere nivel 2\n";
        return;
    }

    int a = aleatorio(1, 10), d = aleatorio(2, 5);
    cout << "\nSecuencia: ";
    for (int k = 0; k < 4; ++k)
        cout << a + k * d << " ";
    cout << "?\nSiguiente numero: ";
    int resp;
    cin >> resp;
    cin.ignore();

    if (resp == a + 4 * d)
    {
        cout << "Correcto!\n";
        agregarLogro(j, "Secuencia", "Numero siguiente", 'O', 1000);
    }
    else
    {
        cout << "Incorrecto. Era " << a + 4 * d << ". Sin logro.\n";
    }
}

/* Acertijo nivel 2 */
void acertijoNivel2(Usuario *j)
{
    if (j->nivel < 2)
    {
        cout << "Se requiere nivel 2\n";
        return;
    }

    cout << "\nAcertijo: 'Me mojo para secarte. Que soy?'\nRespuesta: ";
    string r;
    getline(cin, r);
    if (r == "toalla" || r == "Toalla")
    {
        cout << "Correcto!\n";
        agregarLogro(j, "Acertijo 2", "Toalla", 'O', 1000);
    }
    else
    {
        cout << "Incorrecto. Era 'toalla'. Sin logro.\n";
    }
}

/* Suma 21 (jugador gana si CPU se pasa) */
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

    bool gana = (!turnoJugador); // CPU hizo la jugada final y pasó de 21
    if (gana)
    {
        cout << "CPU se pasa. Ganas!\n";
        agregarLogro(j, "Suma 21", "Juego contra CPU", 'O', 100);
    }
    else
    {
        cout << "Te pasaste. Sin logro.\n";
    }
}

/* =============== NIVEL 3 =============== */

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
             << "7) Ver perfil\n"
             << "0) Salir\n"
             << "Opcion: ";
        cin >> op;
        cin.ignore();

        switch (op)
        {
        case 1:
        {
            string nom, ali;
            cout << "Nombre: ";
            getline(cin, nom);
            cout << "Alias : ";
            getline(cin, ali);
            insertarUsuario(lista, nom, ali);
            break;
        }
        case 2:
        {
            string ali;
            cout << "Alias a eliminar: ";
            getline(cin, ali);
            eliminarUsuario(lista, ali);
            break;
        }
        case 3:
        {
            string ali;
            cout << "Alias: ";
            getline(cin, ali);
            Usuario *u = buscarUsuario(lista, ali);
            if (!u)
            {
                cout << "Alias no encontrado\n";
                break;
            }
            string n, d;
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
            mostrarTodos(lista);
            break;
        case 5:
            mostrarRanking(lista);
            break;
        case 6:
        {
            string ali;
            cout << "Alias: ";
            getline(cin, ali);
            Usuario *u = buscarUsuario(lista, ali);
            if (u)
                menuMinijuegos(u);
            else
                cout << "Alias no encontrado\n";
            break;
        }
        case 7:
        {
            string ali;
            cout << "Alias: ";
            getline(cin, ali);
            Usuario *u = buscarUsuario(lista, ali);
            if (u)
                mostrarUno(u);
            else
                cout << "Alias no encontrado\n";
            break;
        }
        case 0:
            cout << "Hasta luego\n";
            break;
        default:
            cout << "Opcion invalida\n";
        }
    } while (op != 0);

    /* liberar memoria */
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
