#include <iostream>
#include <string>
using namespace std;

struct Logros
{
    string nombreLogro;
    string descripcion;
    char rango;
    int puntosObtenidos;
    string fechaObtencion;
    Logros *prox;
};

struct Usuario
{
    string nombre;
    string alias;
    int nivel;
    Logros *logros;
    Usuario *prox;
};

Logros *crearLogro(string nombre, string desc, char rango, int puntos, string fecha)
{
    Logros *nuevo = new Logros;
    nuevo->nombreLogro = nombre;
    nuevo->descripcion = desc;
    nuevo->rango = rango;
    nuevo->puntosObtenidos = puntos;
    nuevo->fechaObtencion = fecha;
    nuevo->prox = NULL;
}

Usuario *crearUsuario(string nombre, string alias)
{
    Usuario *nuevo = new Usuario;
    nuevo->nombre = nombre;
    nuevo->alias = alias;
    nuevo->nivel = 1;
    nuevo->logros = NULL;
    nuevo->prox = NULL;
    return nuevo;
}

bool listaVacia(Usuario *inicio)
{
    return inicio == NULL;
}

void mostrarUsuarios(Usuario *inicio)
{
    Usuario *mover;
    Logros *mov;
    int num = 0;
    if (!listaVacia(inicio))
    {
        mover = inicio;
        while (mover != NULL)
        {
            num += 1;
            cout << "Jugador " << num << ":" << endl;
            cout << mover->nombre << endl;
            cout << mover->alias << endl;
            cout << mover->nivel << endl;
            cout << "Logros de " << mover->alias << ":" << endl;
            mov = mover->logros;
            while (mov != NULL)
            {
                cout << "    Nombre del logro:" << mov->nombreLogro << endl;
                cout << "    Descripcion: " << mov->descripcion << endl;
                cout << "    Rango " << mov->rango << endl;
                cout << "    Puntos obtenidos " << mov->puntosObtenidos << endl;
                cout << "    Fecha: " << mov->fechaObtencion << endl;
                mov = mov->prox;
            }
            mover = mover->prox;
        }
        cout << "NULL" << endl;
    }
    else
        cout << "Lista esta vacia" << endl;
}

void insertarUsuario(Usuario **inicio, string nombre, string alias)
{
    Usuario *nuevo = crearUsuario(nombre, alias);

    if (listaVacia(*inicio))
    {
        *inicio = nuevo;
    }
    else
    {
        Usuario *auxiliar = *inicio;
        while (auxiliar->prox != NULL)
        {
            auxiliar = auxiliar->prox;
        }
        auxiliar->prox = nuevo;
    }
}

void eliminarUsuario(Usuario **inicio, string alias)
{
    Usuario *mover, *anterior = NULL;

    if (listaVacia(*inicio))
        cout << "No hay usuarios" << endl;
    else
    {
        mover = *inicio;
        while (mover != NULL && mover->alias != alias)
        {
            anterior = mover;
            mover = mover->prox;
        }
        if (mover == NULL)
            cout << "El usuario no existe en la lista " << endl;
        else
        {
            if (mover == *inicio)
                *inicio = (*inicio)->prox;
            else
                anterior->prox = mover->prox;
            delete mover;
        }
    }
}

void buscarUsuario(Usuario *inicio, string alias)
{
    Usuario *aux = inicio;
    bool encontrado = false;
    if (listaVacia(inicio))
        cout << "Lista vacia" << endl;
    else
    {
        while (aux != NULL && encontrado == false)
        {
            if (aux->alias == alias)
            {
                cout << "El usuario " << alias << " ya existe" << endl;
                encontrado = true;
            }
            else
                aux = aux->prox;
        }
        if (encontrado == false)
            cout << "Valor no encontrado en la lista " << endl;
    }
}

void agregarLogroAUsuario(Usuario *lista, string alias, string nombreLogro, string descLogro, char rangoLogro, int puntosLogro, string fechaLogro)
{
    Usuario *u = lista;
    while (u != NULL && u->alias != alias)
        u = u->prox;

    if (u == NULL)
    {
        cout << "Alias no encontrado" << endl;
        return;
    }

    Logros *nuevo = crearLogro(nombreLogro, descLogro, rangoLogro, puntosLogro, fechaLogro);

    if (u->logros == NULL)
        u->logros = nuevo;
    else
    {
        Logros *aux = u->logros;
        while (aux->prox != NULL)
            aux = aux->prox;
        aux->prox = nuevo;
    }
    cout << "Logro agregado a " << alias << endl;
}

string pedirDatosUsuarioNombre()
{
    string nombre = "";

    cout << "Indica nombre: ";
    cin >> nombre;
    return nombre;
}
string pedirDatosUsuarioAlias()
{
    string alias;
    cout << "Indica el alias: ";
    cin >> alias;
    return alias;
}

int main()
{
    Usuario *primero = nullptr;
    int opcion = 0;

    do
    {
        cout << "\n===== MENU =====\n"
             << "1) Crear usuario\n"
             << "2) Mostrar usuarios\n"
             << "3) Agregar logro a usuario\n"
             << "4) Eliminar un usuari\n"
             << "5) Salir\n"
             << "Opcion: ";
        cin >> opcion;
        cout << endl;

        switch (opcion)
        {
        case 1:
        {
            string nombre, alias;

            cout << "Nombre: ";
            cin.ignore();
            getline(cin, nombre);

            cout << "Alias: ";
            getline(cin, alias);

            insertarUsuario(&primero, nombre, alias);
            cout << "Usuario creado.\n";
            break;
        }

        case 2:
            mostrarUsuarios(primero);
            break;

        case 3:
        {
            string alias, nombreL, descL, fechaL;
            char rangoL;
            int puntosL;

            cout << "Alias del jugador: ";
            cin.ignore();
            getline(cin, alias);

            cout << "Nombre del logro: ";
            getline(cin, nombreL);

            cout << "Descripcion: ";
            getline(cin, descL);

            cout << "Rango (O/P/B): ";
            cin >> rangoL;
            cin.ignore();

            cout << "Puntos: ";
            cin >> puntosL;
            cin.ignore();

            cout << "Fecha (dd-mm-aaaa): ";
            getline(cin, fechaL);

            agregarLogroAUsuario(primero, alias,
                                 nombreL, descL, rangoL, puntosL, fechaL);
            break;
        }

        case 4:
        {
            string alias;
            cout << "Ingresa el alias del usuario a eliminar: ";
            cin.ignore(); 
            getline(cin, alias);
            eliminarUsuario(&primero, alias);
        break;
        }
        case 5:
            cout << "Fin.\n";
            break;

        default:
            cout << "Opción inválida.\n";
        }
    } while (opcion != 5);

    return 0;
}
