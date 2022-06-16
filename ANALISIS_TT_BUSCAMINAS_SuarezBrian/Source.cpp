#include <iostream>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <locale.h>
#include <string>
#include <ctime>

using namespace std;

// Enumeradores.
enum class DIFICULTAD { PRINCIPIANTE = 1, INTERMEDIO, EXPERTO, PERSONALIZAR, SALIR }; // DIFICULTAD DEL JUEGO, se puede configurar en el menu.
enum class NUMEROS { CERO = 0, UNO, DOS, TRES, CUATRO, CINCO, SEIS, SIETE, OCHO, NUEVE = 9 , MENOSUNO = -1 }; // NUMERO DEL TABLERO.

// Estructuras.
struct POSICION // Guardo las posiciones del tablero.
{
	int fila = 0;
	int columna = 0;
};

// Variables globales configurables en el menu.
int auxCondicionVictoria = 0; // 0 = Sigue jugando, 1 = Perdio, 2 = Gano, 3 = Abandono. 

bool mostrarReglas = false; // Esta variable la utilizo para mostrar la reglas, es configurable en el menu.
bool activarCheat = false; // Esta variable la utilizo para mostrar una ayuda, es configurable en el menu.
bool NoContinua = false;
bool EsSalir = false;

size_t pColumna = 8; // Esta variable marca el maximo de columnas del tablero, es configurable desde el menu.
size_t pFilas = 8; // Esta variable marca el maximo de filas del tablero, es configurable desde el menu.

int cantidadMinas = 10; // La cantidad maxima de mina del tablero, es configurable desde el menu.

POSICION auxPo[2] = { { -1, -1 },{ -1 , -1 } }; // Un variable auxiliar, donde guardo las posiciones del tablero, node no recorrio el iterador.
bool cambio = false; // Variable que utilizo en la funcion recursiva "DestaparCasillas"", para que no entre en una condicion.

float gameInitTime = 0.0f; // Variable, para obtener los segundo que tarda en ejecutar la funcion.
float finalTime = 0.0f; // Variable que calcula el tiempo total que jugo.
float timeElapsed = 0.0f; // Variable que saca la diferencia entre gameInitTime - finalTime.

bool StartedGame = false; // Para saber si el juego ya inicio.

// Funcion para reservar memoria.
int** ReservarMemoriaINT(); // De esta forma mi tablero es dinamico y solo reservo el espacio de memoria que se le configure.
bool** ReservarMemoriaBOOL(); // De esta forma mi tablero es dinamico y solo reservo el espacio de memoria que se le configure.

// El resto de las funciones.
void MostrarMensaje(string); // Funcion que muestra un mensaje, cundo le envio un string por parametro.
void MenuInicial(); // Este es mi funcion que muestra el menu principal.
void SeColorNumeros(NUMEROS); // Seteo el color de los numeros del tablero, 1, 2, 3, etc..
void MenuFinal(int, int**, bool**, int**); // Muestro el menu final, si gano o perdio el usuario.
void MostrarReglas(); // Muestra las reglas, del juego.
void Configurar(); // Esta funcion muestra el menu de configuraciones.
void MensajeDeError(); // Muestra un mensaje de error en los switch cuando sale por default.
void ConfigurarDificultad(DIFICULTAD); // Menu que muestra las dificultad para configurar.
void SetColor(int); // Seteo el color, resive como parametro el numero.
void InicializarMatriz(int**); // Inicializa mi matriz de entero en 0.
void InicializarVisibilidad(bool**); // Inicializa mi matriz bool en false.
void SetMina(int**, POSICION); // Seteo la minas dentro del tablero.
void BucleDelJuego(int**, bool**, int**); // Este es el bucle principal de mi juego.
void CalcularMinas(int**); // Valida las posiciones de la minas en el tablero, y suma +1 a las casillas adyacentes.
void DestaparCasillas(int**, bool**, POSICION); // Funcion recursiva, que sirve para destapar las casillas con valores 0.
void ValidarPosicionIngresada(int**, bool**, POSICION); // Valida posicion que se le envie por parametro, para saber si tengo que llamar a mi funcion recursiva o no.
int ValidarBuscaMina(int**, bool**, POSICION); // Devuelve 1 - Si el usuario perdio, 2 - Si el usuario gano o 0 si sigue jugando.
void VerificaColumnasDerecha(int**, bool**, POSICION); // Verifica las columas desde la posicion inicial para la derecha.
void VerificaColumnasIzquierda(int**, bool**, POSICION); // Verifica las columas desde la posicion inicial para la izquierda.
void MostrarTablero(int**, bool**, int**); // Muestra mi tablero por pantalla.
void ValidarInput(int&, int, int); // Valida cualquier input, que reciba entero. se le envia el min y el max.
void ValidarInputChar(int&, int, int); // Valida cualquier input char, que reciba entero. se le envia el min y el max.
char InterfazDeOpciones(); // Interfaz de opciones para jugar.
void ActivarCheat(int**, size_t); // Me muestra el tablero con los numeros, para ayudarme a testear.
int CharToInt(char); // Combierte el char numerico en entero numerico.
void CargarElTablero(int**, POSICION); // Carga las minas despues de poner la primera posicion.
int ContarMinas(int**); // Cuenta las minas restantes.
void MarcarMina(int**, bool**, POSICION); // Marca la mina.
void DesMarcarMina(int**, bool**, POSICION);

/* MAIN */
void main()
{
	system("mode con: cols=235 lines=60");
	srand(time(NULL));
	MenuInicial(); // Inicia el juego.
	system("pause");
}


/* BUCLE PRINCIPAL DEL JUEGO */
void BucleDelJuego(int** pTablero, bool** pTableroVisible, int** pTableroBanderas)
{
	StartedGame = false;
	bool EsPrimeraVez = true;
	InicializarMatriz(pTablero); // Inicializo mi matriz en 0.	
	InicializarMatriz(pTableroBanderas); // Inicializo mi matriz en 0.	
	InicializarVisibilidad(pTableroVisible); // Inicializo mi matriz de bool en false.		
	
	char tecla = ' '; // Tecla que aprieta el usuario.	
	POSICION posicion; // Una estructura que me guarda la posicion ingresada por el usuario.

	do
	{
		if (mostrarReglas) // En el caso de que el usuario haya activado las reglas del juego.
		{
			MostrarReglas(); // Funcion que me muestra las reglas.
		}

		MostrarTablero(pTablero, pTableroVisible, pTableroBanderas); // Muestra el tablero.

		tecla = InterfazDeOpciones(); // Guardo la tecla presionada.

		switch (tecla)
		{
		// DESCUBRIR.
		case '1':
			cout << "\n- INGRESAR \nINGRESE LA POSICION DE LA ";

			SetColor(12); // Seteo el color de la palabra.
			cout << "FILA ";
			SetColor(15); // Seteo el color para que vuelva a ser blanco.			

			ValidarInput(posicion.fila, 0, pFilas - 1); // Valida el input.

			cout << "INGRESE LA POSICION DE LA ";

			SetColor(12); // Seteo el color de la palabra.
			cout << "COLUMNA ";
			SetColor(15); // Seteo el color para que vuelva a ser blanco.
			
			ValidarInput(posicion.columna, 0, pColumna - 1); // Valida el input.

			if (EsPrimeraVez) // Esto lo hace la primera vez que se ingresa una posicion.
			{
				CargarElTablero(pTablero, posicion); // Genero todo el tablero, alrededor de la posicion ingresada.
				EsPrimeraVez = false; // pongo en false, la bandera.
				StartedGame = true;
			}

			ValidarPosicionIngresada(pTablero, pTableroVisible, posicion);	 // Validar la posicion ingresa por el usuario.

			if (auxPo[1].fila >= 0) // Si se quedo alguna posicion sin validar cuando se itero por todas las casillas, por cualquier motivo. Le mando esa posicion.
			{
				ValidarPosicionIngresada(pTablero, pTableroVisible, auxPo[1]); // Valida la posicion que no se pudo validar al principio.
			}
			if (auxPo[0].fila >= 0)
			{
				ValidarPosicionIngresada(pTablero, pTableroVisible, auxPo[0]); // Valida la posicion que no se pudo validar al principio.
			}

			cambio = false; // Condicion que utilizo en la funcion recursiva, para que no entre a una condicion en especifico.

			auxCondicionVictoria = ValidarBuscaMina(pTablero, pTableroVisible, posicion); // Me retorna un entero, el cual me dice si el usuario gano, perdio o sigue jugando : 1 = PERDIO, 2 = GANO, 0 = SIGUE JUGANDO.
			break;
		// MARCAR.
		case '2':
			if (StartedGame)
			{
				cout << "\n- MARCAR \nINGRESE LA POSICION DE LA ";

				SetColor(12); // Seteo el color de la palabra.
				cout << "FILA ";
				SetColor(15); // Seteo el color para que vuelva a ser blanco.			

				ValidarInput(posicion.fila, 0, pFilas - 1); // Valida el input.

				cout << "INGRESE LA POSICION DE LA ";

				SetColor(12); // Seteo el color de la palabra.
				cout << "COLUMNA ";
				SetColor(15); // Seteo el color para que vuelva a ser blanco.

				ValidarInput(posicion.columna, 0, pColumna - 1); // Valida el input.
				MarcarMina(pTableroBanderas, pTableroVisible, posicion);
			}
			break;
		// DESMARCAR.
		case '3':
			if (StartedGame)
			{
				cout << "\n- DESMARCAR \nINGRESE LA POSICION DE LA ";

				SetColor(12); // Seteo el color de la palabra.
				cout << "FILA ";
				SetColor(15); // Seteo el color para que vuelva a ser blanco.			

				ValidarInput(posicion.fila, 0, pFilas - 1); // Valida el input.

				cout << "INGRESE LA POSICION DE LA ";

				SetColor(12); // Seteo el color de la palabra.
				cout << "COLUMNA ";
				SetColor(15); // Seteo el color para que vuelva a ser blanco.

				ValidarInput(posicion.columna, 0, pColumna - 1); // Valida el input.
				DesMarcarMina(pTableroBanderas, pTableroVisible, posicion);
			}
			break;
		// ACTIVAR REGLAS.
		case '4':
			mostrarReglas == true ? mostrarReglas = false : mostrarReglas = true; // En el caso de que este activo, lo desactivo, lo mismo para caso contrario.
			break;
		// ACTIVAR CHEAT.
		case '5':
			activarCheat == true ? activarCheat = false : activarCheat = true; // En el caso de que este activo, lo desactivo, lo mismo para caso contrario.
			break;
		// SALIR.
		case '6':
			mostrarReglas = false; // Reseteo los valores por defecto.
			activarCheat = false; // Reseteo los valores por defecto.
			finalTime = clock(); // Termino el clock.
			timeElapsed = finalTime - gameInitTime; // Saco la diferencia, para obtener el valor mas preciso.
			MenuInicial(); // Vuelvo al menu.	
			break;
		default:
			MensajeDeError(); // Si se presiona una tecla no esperada, tira error.			
			break;
		}

		system("cls");

	} while (auxCondicionVictoria == 0); // Si auxCondicionVictoria == 0 sigue jugando.

	if (!NoContinua)
	{
		MenuFinal(auxCondicionVictoria, pTablero, pTableroVisible, pTableroBanderas); // En el caso de auxCondicionVictoria != 0, entonces muestro el menu final, que dice si gano o perdio.
	}
}

/* MENU INICIAL */
void MenuInicial()
{
	int opcion = 0; // Variable que guarda la opcion seleccionada.
	int** tablero = 0; // Declaro mi tablero de enteros.
	int** tableroBanderas = 0; // Declaro mi tablero de enteros.
	bool** tableroVisible = 0; // Declaro mi tablero de bool.

	system("cls");

	cout << "*****************************************************************************" << endl;
	SetColor(2); // Seteo el color de la palabra.
	cout << ".:: BUSCA MINAS :: .";
	SetColor(11); // Seteo el color de la palabra.
	cout << " TIMEPO TRANSCURRIDO DEL ULTIMO JUEGO : ";
	SetColor(12); // Seteo el color de la palabra.
	cout << timeElapsed << "s" << endl;
	SetColor(15); // Seteo el color para que vuelva a ser blanco.
	cout << "*****************************************************************************" << endl;

	cout << "\nMENU INICIAL - \n1 - JUGAR\n2 - VER REGLAS\n3 - CONFIGURAR\n";

	SetColor(12); // Seteo el color de la palabra.
	cout << "4 - SALIR\n" << endl;
	SetColor(15); // Seteo el color para que vuelva a ser blanco.

	ValidarInputChar(opcion, 1, 4); // Valido que el input no supere los rangos min y max.

	switch (opcion) // Dependiendo que opcion haya seleccionado, entrare en algun case.
	{
	case 1:
		tablero = ReservarMemoriaINT(); // Cuando le doy a jugar, recien reservo los espacio para la matrices, con la cantidad de fila y columna configurada.
		tableroBanderas = ReservarMemoriaINT(); // Cuando le doy a jugar, recien reservo los espacio para la matrices, con la cantidad de fila y columna configurada.
		tableroVisible = ReservarMemoriaBOOL(); // Cuando le doy a jugar, recien reservo los espacio para la matrices, con la cantidad de fila y columna configurada.

		system("cls");

		gameInitTime = clock();
		BucleDelJuego(tablero, tableroVisible, tableroBanderas); // Inicia el bucle del juego.
		break;
	case 2:
		MostrarReglas(); // Muestro las reglas del juego.
		system("pause");
		MenuInicial(); // Vuevlo al menu de incio.
		break;
	case 3:
		Configurar(); // Muestro el menu de configuracion.
		if (!EsSalir)
		{
			MenuInicial(); // Vuevlo al menu de incio.
		}
		break;
	case 4:	
		auxCondicionVictoria = 3;
		MenuFinal(auxCondicionVictoria, tablero, tableroVisible, tableroBanderas);
		EsSalir = true;
		break;
	default:
		MensajeDeError(); // Un mensaje de error.
		MenuInicial(); // Vuelvo al menu de inicio.
		break;
	}
}

/* MENU PARA CUANDO SE TERMINA EL JUEGO */
void MenuFinal(int pCondicionVictoria, int** pTablero, bool** pTableroVisible, int** pTableroBanderas)
{
	char opcion = ' '; // Variable que guarda la opcion seleccionada.

	switch (pCondicionVictoria) // pCondicionVictoria = 1 PERDIO, pCondicionVictoria = 2 GANO.
	{
	case 1:

		finalTime = clock(); // Termino el clock.
		timeElapsed = finalTime - gameInitTime; // Saco la diferencia, para obtener el valor mas preciso.

		MostrarTablero(pTablero, pTableroVisible, pTableroBanderas); // Muestro el tablero.

		SetColor(12); // Seteo el color de la palabra.
		cout << "\nHAS PERDIDO, SUERTE PARA LA PROXIMA,";
		SetColor(8); // Seteo el color de la palabra.
		cout << " TIEMPO APROXIMADO : " << timeElapsed << "s ..." << endl;
		SetColor(15); // Seteo el color para que vuelva a ser blanco.
		break;
	case 2:

		finalTime = clock(); // Termino el clock.
		timeElapsed = finalTime - gameInitTime; // Saco la diferencia, para obtener el valor mas preciso.

		MostrarTablero(pTablero, pTableroVisible, pTableroBanderas);  // Muestro el tablero.

		SetColor(2); // Seteo el color de la palabra.
		cout << "\nFELICIDADES HAS GANADO," << endl;
		SetColor(8); // Seteo el color de la palabra.
		cout << " TIEMPO APROXIMADO : " << timeElapsed << "s ..." << endl;
		SetColor(15); // Seteo el color para que vuelva a ser blanco.
		break;
	case 3:
		system("cls");
		SetColor(2); // Seteo el color de la palabra.
		cout << "EL USUARIO CERRO EL JUEGO..." << endl;
		SetColor(15); // Seteo el color para que vuelva a ser blanco.
		break;
	default:
		MensajeDeError(); // Un mensaje de error.
		MenuInicial(); // Vuelvo al menu de inicio.
		break;
	}
	
	if (pCondicionVictoria != 3)
	{
		cout << "\nCONTINUAR JUGANDO ? S/N" << endl;

		cin >> opcion;

		if (opcion == 's' || opcion == 'S') // Si ingreso la tecla s o S, reseteo todos los valores, por defecto, y vuelvo al menu de inicio.
		{
			mostrarReglas = false; // Reseteo los valores por defecto.
			activarCheat = false; // Reseteo los valores por defecto.
			pColumna = 8; // Reseteo los valores por defecto.
			pFilas = 8; // Reseteo los valores por defecto.
			cantidadMinas = 10; // Reseteo los valores por defecto.
			auxCondicionVictoria = 0; // Reseteo la condicion de victoria.

			// Vacio el espacio en memoria por las dudas.
			delete[] pTablero;
			delete[] pTableroVisible;		

			MenuInicial(); // Vuelvo al menu de inicio.
		}
		else
		{
			NoContinua = true;
		}
	}
}

/* MENU DE REGLAS */
void MostrarReglas()
{
	system("cls");
	SetColor(2); // Seteo el color de la palabra.
	cout << ".:: BUSCA MINAS - REGLAS :: ." << endl;
	SetColor(15); // Seteo el color para que vuelva a ser blanco.

	SetColor(2); // Seteo el color de la palabra.
	cout << "\n\n- Reglas del Juego" << endl;
	SetColor(15); // Seteo el color para que vuelva a ser blanco.

	cout << "\nEl juego consiste en despejar todas las casillas de una pantalla que no oculten una mina." << endl;
	cout << "Algunas casillas tienen un numero, el cual indica la cantidad de minas que hay en las casillas circundantes." << endl;
	cout << "Si una casilla tiene el numero 3, significa que de las ocho casillas que hay alrededor hay 3 con minas y 5 sin minas." << endl;
	cout << "Si se descubre una casilla sin número las casillas vecinas se descubren automaticamente." << endl;
	cout << "Si se descubre una casilla con una mina se pierde la partida." << endl;
	cout << "Podes marcar las casillas que creas que haya minas, pero ten encuenta que si la casilla es marcada y no es una mina, luego tendras que desmarcar y descubrir, para ganar." << endl;

	SetColor(2); // Seteo el color de la palabra.
	cout << "\n\n- Niveles de Juego" << endl;
	SetColor(15); // Seteo el color para que vuelva a ser blanco.

	cout << "Nivel principiante : 8 x 8 casillas y 10 minas." << endl;
	cout << "Nivel intermedio : 16 x 16 casillas y 40 minas." << endl;
	cout << "Nivel experto : 16 x 30 casillas y 99 minas." << endl;
	cout << "Nivel personalizado : en este caso el usuario personaliza su juego eligiendo el numero de minas y el tamaño del tablero.\n" << endl;

}

/* MENU DE CONFIGURACION */
void Configurar()
{
	int opcion = 0; // Variable que guarda la opcion seleccionada.

	system("cls");

	SetColor(2); // Seteo el color de la palabra.
	cout << "MENU DE CONFIGURACION " << endl;
	SetColor(15); // Seteo el color para que vuelva a ser blanco.

	cout << "\n1 - DIFICULTAD\n2 - MOSTRAR SIEMPRE LAS REGLAS\n3 - ACTIVAR EL MODO DESARROLLO (este modo, le otorga al usuario una ayuda)\n";

	SetColor(12); // Seteo el color de la palabra.
	cout << "4 - VOLVER\n" << endl;
	SetColor(15); // Seteo el color para que vuelva a ser blanco.

	ValidarInputChar(opcion, 1, 4); // Valido que el input no supere los rangos min y max.

	switch (opcion)
	{
	case 1:
		system("cls");

		SetColor(2); // Seteo el color de la palabra.
		cout << "MENU DE DIFICULTADES " << endl;
		SetColor(15); // Seteo el color para que vuelva a ser blanco.

		cout << "\n1 - PRINCIPIANTE\n2 - INTERMEDIO\n3 - EXPERTO\n4 - PERSONALIZADO\n";

		SetColor(12); // Seteo el color de la palabra.
		cout << "5 - SALIR\n" << endl;
		SetColor(15); // Seteo el color para que vuelva a ser blanco.

		ValidarInputChar(opcion, 1, 5); // Valido que el input no supere los rangos min y max.

		ConfigurarDificultad((DIFICULTAD)opcion); // Voy al menu para configurar la dificultad, con la opcion seleccionada.

		if (!EsSalir)
		{
			SetColor(12); // Seteo el color de la palabra.
			MostrarMensaje("\nLOS CAMBIOS HAN SIDO GUARDADOS !!!");
			SetColor(15); // Seteo el color para que vuelva a ser blanco.

			system("pause");
		}

		break;
	case 2:
		mostrarReglas == true ? mostrarReglas = false : mostrarReglas = true; // En el caso de que este activo, lo desactivo, lo mismo para caso contrario.

		SetColor(12); // Seteo el color de la palabra.
		MostrarMensaje("\nLOS CAMBIOS HAN SIDO GUARDADOS !!!");
		SetColor(15); // Seteo el color para que vuelva a ser blanco.

		system("pause");
		Configurar();
		break;
	case 3:		
		activarCheat == true ? activarCheat = false : activarCheat = true; // En el caso de que este activo, lo desactivo, lo mismo para caso contrario.

		SetColor(12); // Seteo el color de la palabra.
		MostrarMensaje("\nLOS CAMBIOS HAN SIDO GUARDADOS !!!");
		SetColor(15); // Seteo el color para que vuelva a ser blanco.

		system("pause");
		Configurar();
		break;
	case 4:
		MenuInicial(); // Vuelvo al menu de inicio.
		break;
	default:
		MensajeDeError(); // Muestro un mensaje de error.
		Configurar(); // Vuelvo al menu de configuracion.
		break;
	}
}

/* MENU PARA CONFIGURAR LA DIFICULTAD */
void ConfigurarDificultad(DIFICULTAD pDificultad)
{
	int aux = 0;
	switch (pDificultad)
	{
	case DIFICULTAD::PRINCIPIANTE:		
		pFilas = 8;
		pColumna = 8;
		cantidadMinas = 10;
		break;
	case DIFICULTAD::INTERMEDIO:	
		pFilas = 16;
		pColumna = 16;
		cantidadMinas = 40;
		break;
	case DIFICULTAD::EXPERTO:		
		pFilas = 16;
		pColumna = 30;
		cantidadMinas = 99;
		break;
	case DIFICULTAD::PERSONALIZAR:
		cout << "\nINGRESAR EL NUMERO DE FILAS MIN(8)-MAX(40) : ";
		ValidarInput(aux, 8, 40); // Valido el input.
		pFilas = aux; // Guardo el valor de las filas.

		cout << "INGRESAR EL NUMERO DE COLUMNAS MIN(8)-MAX(30) : ";
		ValidarInput(aux, 8, 30); // Valido el input.
		pColumna = aux; // Guardo el valor de las columnas.

		cout << "INGRESE EL NUMERO DE MINAS MIN(1)-MAX(200) : ";
		ValidarInput(aux, 1, 200);
		cantidadMinas = aux; // Guardo la cantidad de minas.	
		break;
	case DIFICULTAD::SALIR:
		MenuInicial(); // Vuelvo al menu de inicio.
		break;
	default:
		MensajeDeError(); // Muestro un mensaje de Error.
		Configurar(); // Vuelvo al menu de configuracion.
		break;
	}
}

/* MATRIZ BOOL - RESERVO ESPACIO EN LA MEMORIA, SEGUN LA DIFICULTAD */
bool** ReservarMemoriaBOOL()
{
	bool** tablero = new bool* [pFilas]; // Reservo la cantidad de filas en memoria.

	
	for (size_t i(0); i < pFilas; i++) // Recorro la fila y voy generando el espacio en la memoria de las columnas.
	{
		tablero[i] = new bool[pColumna]; // Por cada filas, reservo la cantidad de columna en memoria.
	}

	return tablero; // Devuelvo el tablero armado.
}

/* MATRIZ DE ENTERO - RESERVO ESPACIO EN LA MEMORIA, SEGUN LA DIFICULTAD */
int** ReservarMemoriaINT()
{
	int** tablero = new int* [pFilas]; // Reservo la cantidad de filas en memoria.

	// Recorro la fila y voy generando el espacio en la memoria de las columnas.
	for (size_t i(0); i < pFilas; i++)
	{
		tablero[i] = new int[pColumna]; // Por cada filas, reservo la cantidad de columna en memoria.
	}

	return tablero; // Devuelvo el tablero armado.
}

/* CARGO LA MATRIZ CON VALORES NULOS, 0 */
void InicializarMatriz(int** pTablero)
{
	// Comienzo con todas las casillas con un valor de 0.
	for (size_t i(0); i < pFilas; i++)
	{
		for (size_t k(0); k < pColumna; k++)
		{
			pTablero[i][k] = (int)NUMEROS::CERO; // Recorro casilla por casilla y lo inicializo en 0.
		}
	}
}

/* CARGO LA MATRIZ DE BOOL CON VALORES FALSE */
void InicializarVisibilidad(bool** pTableroVisible)
{
	for (size_t i(0); i < pFilas; i++)
	{
		for (size_t k(0); k < pColumna; k++)
		{
			pTableroVisible[i][k] = false; // Recorro casilla por casilla y lo inicializo en false.
		}
	}
}

/* SETEO LAS MINAS */
void SetMina(int** pTablero, POSICION pPosicion)
{
	int minas = 0;
	int fila = 0;
	int columna = 0;

	do
	{
		// Pongo minas en lugares random del tablero.
		do
		{
			fila = 1 + rand() % pFilas - 1; // Genero una fila random.
			columna = 1 + rand() % pColumna - 1; // Genero una columna random.

		} while (pTablero[fila][columna] == (int)NUMEROS::NUEVE || fila == pPosicion.fila && columna == pPosicion.columna); // En el caso de que en esa casilla, haya mina, no agrego nada, sigue iterando.
		
		pTablero[fila][columna] = (int)NUMEROS::NUEVE; // Coloco la maina.
		
		minas++; // Sumo a mi auxiliar.
	} while (minas < cantidadMinas); // Cuando se alcance la cantidad de minas deseada en el teblero dejo de iterar.

}

// Esta funcion toma el lugar de una mina, y va sumando +1 a las casillas adyacentes. [INTENTAR OPTIMIZARLA]
void CalcularMinas(int** pTablero)
{
	for (size_t i(0); i < pFilas; i++)
	{
		for (size_t k(0); k < pColumna; k++)
		{
			if (pTablero[i][k] == (int)NUMEROS::NUEVE)
			{
				if (k == 0 && i == 0)
				{
					pTablero[i][k + 1] != 9 ? pTablero[i][k + 1] += 1 : pTablero[i][k + 1] += 0;

					pTablero[i + 1][k] != 9 ? pTablero[i + 1][k] += 1 : pTablero[i + 1][k] += 0;

					pTablero[i + 1][k + 1] != 9 ? pTablero[i + 1][k + 1] += 1 : pTablero[i + 1][k + 1] += 0;
				}
				else if (k == (pColumna - 1) && i == 0)
				{
					pTablero[i][k - 1] != 9 ? pTablero[i][k - 1] += 1 : pTablero[i][k - 1] += 0;

					pTablero[i + 1][k - 1] != 9 ? pTablero[i + 1][k - 1] += 1 : pTablero[i + 1][k - 1] += 0;

					pTablero[i + 1][k] != 9 ? pTablero[i + 1][k] += 1 : pTablero[i + 1][k] += 0;
				}
				else if (k == (pColumna - 1) && i == (pFilas - 1))
				{
					pTablero[i][k - 1] != 9 ? pTablero[i][k - 1] += 1 : pTablero[i][k - 1] += 0;

					pTablero[i - 1][k - 1] != 9 ? pTablero[i - 1][k - 1] += 1 : pTablero[i - 1][k - 1] += 0;

					pTablero[i - 1][k] != 9 ? pTablero[i - 1][k] += 1 : pTablero[i - 1][k] += 0;
				}
				else if (k == 0 && i == (pFilas - 1))
				{
					pTablero[i - 1][k] != 9 ? pTablero[i - 1][k] += 1 : pTablero[i - 1][k] += 0;

					pTablero[i - 1][k + 1] != 9 ? pTablero[i - 1][k + 1] += 1 : pTablero[i - 1][k + 1] += 0;

					pTablero[i][k + 1] != 9 ? pTablero[i][k + 1] += 1 : pTablero[i][k + 1] += 0;
				}
				else if (k == 0)
				{
					pTablero[i - 1][k] != 9 ? pTablero[i - 1][k] += 1 : pTablero[i - 1][k] += 0;

					pTablero[i - 1][k + 1] != 9 ? pTablero[i - 1][k + 1] += 1 : pTablero[i - 1][k + 1] += 0;

					pTablero[i][k + 1] != 9 ? pTablero[i][k + 1] += 1 : pTablero[i][k + 1] += 0;

					pTablero[i + 1][k] != 9 ? pTablero[i + 1][k] += 1 : pTablero[i + 1][k] += 0;

					pTablero[i + 1][k + 1] != 9 ? pTablero[i + 1][k + 1] += 1 : pTablero[i + 1][k + 1] += 0;
				}
				else if (k == (pColumna - 1))
				{
					pTablero[i - 1][k] != 9 ? pTablero[i - 1][k] += 1 : pTablero[i - 1][k] += 0;

					pTablero[i - 1][k - 1] != 9 ? pTablero[i - 1][k - 1] += 1 : pTablero[i - 1][k - 1] += 0;

					pTablero[i][k - 1] != 9 ? pTablero[i][k - 1] += 1 : pTablero[i][k - 1] += 0;

					pTablero[i + 1][k - 1] != 9 ? pTablero[i + 1][k - 1] += 1 : pTablero[i + 1][k - 1] += 0;

					pTablero[i + 1][k] != 9 ? pTablero[i + 1][k] += 1 : pTablero[i + 1][k] += 0;
				}
				else if (i == 0)
				{
					pTablero[i][k + 1] != 9 ? pTablero[i][k + 1] += 1 : pTablero[i][k + 1] += 0;

					pTablero[i + 1][k + 1] != 9 ? pTablero[i + 1][k + 1] += 1 : pTablero[i + 1][k + 1] += 0;

					pTablero[i + 1][k] != 9 ? pTablero[i + 1][k] += 1 : pTablero[i + 1][k] += 0;

					pTablero[i + 1][k - 1] != 9 ? pTablero[i + 1][k - 1] += 1 : pTablero[i + 1][k - 1] += 0;

					pTablero[i][k - 1] != 9 ? pTablero[i][k - 1] += 1 : pTablero[i][k - 1] += 0;
				}
				else if (i == (pFilas - 1))
				{
					pTablero[i][k - 1] != 9 ? pTablero[i][k - 1] += 1 : pTablero[i][k - 1] += 0;

					pTablero[i - 1][k - 1] != 9 ? pTablero[i - 1][k - 1] += 1 : pTablero[i - 1][k - 1] += 0;

					pTablero[i - 1][k] != 9 ? pTablero[i - 1][k] += 1 : pTablero[i - 1][k] += 0;

					pTablero[i - 1][k + 1] != 9 ? pTablero[i - 1][k + 1] += 1 : pTablero[i - 1][k + 1] += 0;

					pTablero[i][k + 1] != 9 ? pTablero[i][k + 1] += 1 : pTablero[i][k + 1] += 0;
				}
				else
				{
					pTablero[i][k + 1] != 9 ? pTablero[i][k + 1] += 1 : pTablero[i][k + 1] += 0;

					pTablero[i + 1][k + 1] != 9 ? pTablero[i + 1][k + 1] += 1 : pTablero[i + 1][k + 1] += 0;

					pTablero[i + 1][k] != 9 ? pTablero[i + 1][k] += 1 : pTablero[i + 1][k] += 0;

					pTablero[i + 1][k - 1] != 9 ? pTablero[i + 1][k - 1] += 1 : pTablero[i + 1][k - 1] += 0;

					pTablero[i][k - 1] != 9 ? pTablero[i][k - 1] += 1 : pTablero[i][k - 1] += 0;

					pTablero[i - 1][k - 1] != 9 ? pTablero[i - 1][k - 1] += 1 : pTablero[i - 1][k - 1] += 0;

					pTablero[i - 1][k] != 9 ? pTablero[i - 1][k] += 1 : pTablero[i - 1][k] += 0;

					pTablero[i - 1][k + 1] != 9 ? pTablero[i - 1][k + 1] += 1 : pTablero[i - 1][k + 1] += 0;
				}
			}
		}
	}
}

/* CONDICION DE VICTORIA O DERROTA */
int ValidarBuscaMina(int** pTablero, bool** pTableroVisible, POSICION pPosicion)
{
	int contador = 0;
	for (size_t i(0); i < pFilas; i++)
	{
		for (size_t k(0); k < pColumna; k++)
		{
			if (pTableroVisible[i][k])
			{
				contador++; // Cuento todos las casillas descubiertas.
			}
		}
	}

	if (contador == (pFilas * pColumna) - cantidadMinas) // Si casillas descubiertas es igual a total de casillas menos la cantidad de minas del tablero, el usuario gana.
	{
		return 2;
	}
	else if (pTablero[pPosicion.fila][pPosicion.columna] == (int)NUMEROS::NUEVE) // Si la casilla seleccionada es una mina, el usuario pierde.
	{
		return 1;
	}
	else if (pTablero[pPosicion.fila][pPosicion.columna] != (int)NUMEROS::NUEVE) // Si no pasa nada de lo anterior, sigue jugando.
	{
		return 0;
	}
}

/* VALIDA UNA CASILLA QUE LE MANDE POR PARAMETRO */
void ValidarPosicionIngresada(int** pTablero, bool** pTableroVisible, POSICION pPosicion)
{
	if (pTablero[pPosicion.fila][pPosicion.columna] == (int)NUMEROS::CERO) // Si la casilla es 0, voy a la funcion recursiva.
	{
		DestaparCasillas(pTablero, pTableroVisible, pPosicion); // Llamo a la funcion recursiva.
	}
	else // Caso contrario solo muestro esa casilla y salgo de la funcion.
	{
		pTableroVisible[pPosicion.fila][pPosicion.columna] = true; // Muestro la casilla.
	}
}

/* VERIFICO LAS CASILLA DE LA DERECHA, DEPENDIENDO LA POSICION QUE SE LE MANDE */
void VerificaColumnasDerecha(int** pTablero, bool** pTableroVisible, POSICION pPosicion)
{
	POSICION posicion;
	int auxFila = pPosicion.fila;
	int auxColumna = pPosicion.columna;

	// Verifico las columnas de la derecha.
	do
	{

		if (pTablero[auxFila][auxColumna] == (int)NUMEROS::CERO) // Si la casilla es 0.
		{
			pTableroVisible[auxFila][auxColumna] = true; // Muestro la casilla.

			if (auxFila != 0 && pTablero[auxFila - 1][auxColumna] != (int)NUMEROS::CERO) // Valido la casilla de arriba, si es distinto de 0, la muestro.
			{
				pTableroVisible[auxFila - 1][auxColumna] = true; // Muestro la casilla.
			}
			if (auxFila < (pFilas - 1) && pTablero[auxFila + 1][auxColumna] != (int)NUMEROS::CERO) // Valido la casilla de abajo, si es distinto de 0, la muestro.
			{ 
				pTableroVisible[auxFila + 1][auxColumna] = true; // Muestro la casilla.
			}
		}
		else if (pTablero[auxFila][auxColumna] == (int)NUMEROS::NUEVE) // Si la casilla seleccionada es una bomba, salgo del while.
		{
			break;
		}
		else
		{
			pTableroVisible[auxFila][auxColumna] = true; // Si es cualquier otro numero lo muestro y salgo del while.
			break;
		}

		auxColumna++; // Paso a la casilla de alado ->.

	} while (auxColumna <= pColumna); // Este do-while recorre hasta que llega a extremo derecho del tablero o hasta que me tope con algun numero.


	/*
		En cierto casos, por ejemplo si estoy validado las casillas de la derecha y arriba hay un 0, mi codigo nunca contemplaba el 0 y seguia validando las casillas de la derecha hasta encontra un numero
		por eso cuando destapaba todas las casillas habian casillas sin detapar, lo unico que se me ocurrio sin modificar toda la logica que ya tenia era esto :
		En el caso de tener 0 en una casilla de arriba o de abajo, entonces guardo la posicion, de alguna de las 2.
		Basicamente para que luego vuelva a llamar a la funcion recursiva desde los punto que no pude validar antes, y los valide bien.
	*/

	if (auxFila != 0 && pTablero[auxFila - 1][auxColumna] == (int)NUMEROS::CERO) // Valida la casilla de arriba que el numero sea 0.
	{		
		auxPo[0].fila = auxFila - 1; // Guardo en una variable auxiliar global la posicion de la fila.
		auxPo[0].columna = auxColumna; // Guardo en una variable auxiliar global la posicion de la columna.
	}
	if (auxFila < (pFilas - 1) && pTablero[auxFila + 1][auxColumna] == (int)NUMEROS::CERO) // Valida la casilla de abajo que el numero sea 0.
	{
		auxPo[0].fila = auxFila + 1; // Guardo en una variable auxiliar global la posicion de la fila.
		auxPo[0].columna = auxColumna; // Guardo en una variable auxiliar global la posicion de la columna.
	}

}

/* VERIFICO LAS CASILLA DE LA IZQUIERDA, DEPENDIENDO LA POSICION QUE SE LE MANDE */
void VerificaColumnasIzquierda(int** pTablero, bool** pTableroVisible, POSICION pPosicion)
{
	POSICION posicion;
	int auxFila = pPosicion.fila;
	int auxColumna = pPosicion.columna;

	// Verifico las columnas izquierda.
	do
	{

		if (pTablero[auxFila][auxColumna] == (int)NUMEROS::CERO) // Si la casilla es 0.
		{
			pTableroVisible[auxFila][auxColumna] = true; // Muestro la casilla.

			if (auxFila != 0 && pTablero[auxFila - 1][auxColumna] != (int)NUMEROS::CERO) // Valido la casilla de arriba, si es distinto de 0, la muestro.
			{
				pTableroVisible[auxFila - 1][auxColumna] = true; // Muestro la casilla.
			}

			if (auxFila < (pFilas - 1) && pTablero[auxFila + 1][auxColumna] != (int)NUMEROS::CERO) // Valido la casilla de abajo, si es distinto de 0, la muestro.
			{
				pTableroVisible[auxFila + 1][auxColumna] = true; // Muestro la casilla.
			}

		}
		else if (pTablero[auxFila][auxColumna] == (int)NUMEROS::NUEVE) // Si la casilla seleccionada es una bomba, salgo del while.
		{
			break;
		}
		else
		{
			pTableroVisible[auxFila][auxColumna] = true; // Si es cualquier otro numero lo muestro y salgo del while.
			break;
		}

		auxColumna--; // Paso a la casilla de alado <-.
	} while (auxColumna >= 0); // Este do-while recorre hasta que llega a extremo izquierdo del tablero o hasta que me tope con algun numero.

	/*
		En cierto casos, por ejemplo si estoy validado las casillas de la izquierda y arriba hay un 0, mi codigo nunca contemplaba el 0 y seguia validando las casillas de la izquierda hasta encontra un numero
		por eso cuando destapaba todas las casillas habian casillas sin detapar, lo unico que se me ocurrio sin modificar toda la logica que ya tenia era esto :
		En el caso de tener 0 en una casilla de arriba o de abajo, entonces guardo la posicion, de alguna de las 2.
		Basicamente para que luego vuelva a llamar a la funcion recursiva desde los punto que no pude validar antes, y los valide bien.
	*/

	if (auxFila != 0 && pTablero[auxFila - 1][auxColumna] == (int)NUMEROS::CERO) // Valida la casilla de arriba que el numero sea 0.
	{
		auxPo[1].fila = auxFila - 1; // Guardo en una variable auxiliar global la posicion de la fila.
		auxPo[1].columna = auxColumna; // Guardo en una variable auxiliar global la posicion de la columna.
	}
	if (auxFila < (pFilas - 1) && pTablero[auxFila + 1][auxColumna] == (int)NUMEROS::CERO) // Valida la casilla de abajo que el numero sea 0.
	{
		auxPo[1].fila = auxFila + 1; // Guardo en una variable auxiliar global la posicion de la fila.
		auxPo[1].columna = auxColumna; // Guardo en una variable auxiliar global la posicion de la columna.
	}
}

/* FUNCION RECURSIVA PARA DESTAPAR CASILLAS */
void DestaparCasillas(int** pTablero, bool** pTableroVisible, POSICION posicion)
{
	POSICION auxPosicion; // Variable auxiliar, para guardar posiciones.
	auxPo[0] = { -1, -1 }; // Reseteo las variables auxiliares globales, cada vez que se llame a la funcion.
	auxPo[1] = { -1, -1 }; // Reseteo las variables auxiliares globales, cada vez que se llame a la funcion.
	auxPosicion.fila = posicion.fila; // Guardo la fila en mi variable auxiliar.
	bool auxBool = true;

	do
	{
		auxPosicion.columna = posicion.columna; // Guardo la columna en mi variable auxiliar.

		if (pTablero[auxPosicion.fila][auxPosicion.columna] == (int)NUMEROS::NUEVE) // Si la posicion es nueve, seteo una bandera en false y salgo del do-while.
		{
			auxBool = false; // Flag.
			break;
		}
		else if (pTablero[auxPosicion.fila][auxPosicion.columna] != (int)NUMEROS::CERO) // Si la posicion es un numero distinto de 0 y distinto de 9, muestro la casilla y salgo del do-while.
		{
			pTableroVisible[auxPosicion.fila][auxPosicion.columna] = true; // Muestro casilla.
			break;
		}

		// Validar las columnas de la derecha.
		VerificaColumnasDerecha(pTablero, pTableroVisible, auxPosicion);
		// Validar las columnas de la izquierda.			
		VerificaColumnasIzquierda(pTablero, pTableroVisible, auxPosicion);

		auxPosicion.fila--; // Descuento la fila y paso a la de arriba.
	} while (auxPosicion.fila >= 0); // Este do-while se ejecutara hasta que se encuentre un numero en las filas que esta recorriendo o hasta que la fila sea menor a 0.

	auxPosicion.fila = posicion.fila + 1; // Guardo en mi auxiliar la posicion de la fila, de abajo. por que la de arriba ya esta validada.
	while (auxPosicion.fila < pFilas) // Mientras la fila de abajo sea una fila valida, se ejecuta el while.
	{
		auxPosicion.columna = posicion.columna; // Guardo la columna en mi variable auxiliar.

		if (pTablero[auxPosicion.fila][auxPosicion.columna] == (int)NUMEROS::NUEVE) // Si la posicion es nueve, seteo una bandera en false y salgo del do-while.
		{
			auxBool = false; // Flag.
			break;
		}
		else if (pTablero[auxPosicion.fila][auxPosicion.columna] != (int)NUMEROS::CERO) // Si la posicion es un numero distinto de 0 y distinto de 9, muestro la casilla y salgo del do-while.
		{
			pTableroVisible[auxPosicion.fila][auxPosicion.columna] = true; // Muestro casilla.
			break;
		}

		// Validar las columnas de la derecha.
		VerificaColumnasDerecha(pTablero, pTableroVisible, auxPosicion);
		// Validar las columnas de la izquierda.	
		VerificaColumnasIzquierda(pTablero, pTableroVisible, auxPosicion);

		auxPosicion.fila++; // Bajo a la siguiente fila.
	}

	int auxx = 0; // Variable auxiliar, para guardar la columna.

	// En este punto vuelvo a llamar a mi misma funcion pero cambiando sumando la columna.
	if (posicion.columna <= pColumna && auxBool)
	{

		if (!cambio) // Variable global seteada en false, las primeras iteraciones recursiva entra aca.
		{
			auxx = posicion.columna + 1;
			if (pTablero[posicion.fila][auxx] == 0)
			{
				posicion.columna++; // Suma columna ->.
				DestaparCasillas(pTablero, pTableroVisible, posicion); // Llamo a la funcion de nuevo, con las nuevas posiciones.
			}
		}
		cambio = true; // Cambio la vandera a false, asi no entra al if de arriba.

		auxx = posicion.columna - 1; // Ahora recorro para el otro lado, "se puede optimizar".
		if (pTablero[posicion.fila][auxx] == 0)
		{
			posicion.columna--; // Resta columna <-.
			DestaparCasillas(pTablero, pTableroVisible, posicion); // Llamo a la funcion de nuevo, con las nuevas posiciones.
		}
	}

}


void MostrarTablero(int** pTablero, bool** pTableroVisible, int** pTableroBanderas)
{
	int minasCant = ContarMinas(pTableroBanderas);
	int numero = 0;

	if (minasCant >= 10 && minasCant <= 99)
	{
		numero = 24;
	}
	else if(minasCant >= 100)
	{
		numero = 25;
	}
	else
	{
		numero = 23;
	}

	/* MUESTRA LOS NUMERO DE LA FILA */
	cout << "\t\t";
	for (int i = 0; i < pColumna; i++)
	{
		if (i < 10)
		{
			cout << "  " << i;
		}
		else
		{
			cout << " " << i;
		}
	}

	/* DIBUJA LA PARTE SUPERIOR DEL TABLERO */
	cout << endl;
	cout << "\t\t";
	for (int i = 0; i < pColumna * 3 + 1 - numero; i++) // 11 es la cantidad de caracteres de "BUSCA-MINAS"
	{
		if (i == 0)
		{
			cout << "\311";

			SetColor(2);
			cout << "BUSCA-MINAS - ";
			SetColor(12);			
		    cout << " MINAS: " << ContarMinas(pTableroBanderas);
			SetColor(15);
		}

		if (i == (pColumna * 3 + 1) - numero - 1)
		{		
			cout << "\273";
		}
		else
		{
			cout << "\315";
		}
	}

	/* DIBUJA TODO EL INTERIOR DEL TABLERO */
	cout << endl;
	for (size_t i(0); i < pFilas; i++)
	{
		cout << "\t\t";
		cout << "\272 ";
		for (size_t k(0); k < pColumna; k++)
		{
			if (pTableroVisible[i][k] == false)
			{
				/* DIBUJA LAS CUADRADITOS, CUANDO LAS CASILLAS NO SON VISIBLES */
				if (pTableroBanderas[i][k] == (int)NUMEROS::MENOSUNO)
				{
					if (k == pColumna - 1)
					{
						SetColor(12);
						cout << "! ";
						SetColor(15);
					}
					else
					{
						SetColor(12);
						cout << "!  ";
						SetColor(15);
					}
				}
				else
				{
					if (k == pColumna - 1)
					{
						SetColor(8);
						cout << "\337 ";
						SetColor(15);
					}
					else
					{
						SetColor(8);
						cout << "\337  ";
						SetColor(15);
					}
				}
			}
			else
			{
				if (pTablero[i][k] == (int)NUMEROS::NUEVE)
				{
					/* DIBUJA LA BOMBA */
					if (k == pColumna - 1)
					{
						cout << "\317 ";
					}
					else
					{
						cout << "\317  ";
					}
				}
				else if (pTablero[i][k] == (int)NUMEROS::CERO)
				{
					/* DIBUJA LOS NUMEROS 0 */
					if (k == pColumna - 1)
					{
						cout << pTablero[i][k] << " ";
					}
					else
					{
						cout << pTablero[i][k] << "  ";
					}
				}
				else
				{
					/* DIBUJA EL RESTO DE NUMERO */
					SeColorNumeros((NUMEROS)pTablero[i][k]); // Le doy color al resto de numero, dependiendo cual sea.
					if (k == pColumna - 1)
					{
						cout << pTablero[i][k] << " ";
					}
					else
					{
						cout << pTablero[i][k] << "  ";
					}   

					SetColor(15); // Seteo el color en blanco.
				}
			}
		}

		cout << "\272";
		cout << " " << i; // Muestro el numero de fila, al final.

		if (activarCheat) // Si en el menu de configuracion de activa el cheat, entra al if.
		{
			ActivarCheat(pTablero, i); // Me muestra el tablero completo destapado.
		}

		cout << endl;
	}

	/* DIBUJA LA PARTE INFERIOR DEL TABLERO */
	cout << "\t\t";
	for (int i = 0; i < pColumna * 3 + 1; i++)
	{
		if (i == 0)
		{
			cout << "\310";
		}
		if (i == (pColumna * 3 + 1) - 1)
		{
			cout << "\274";
		}
		else
		{
			cout << "\315";
		}
	}
	cout << endl;

}

/* ME MUESTRA EL CHEAT EN PANTALLA*/
void ActivarCheat(int** pTablero, size_t indice)
{
	cout << "\t\t";
	for (size_t j(0); j < pColumna; j++)
	{
		cout << pTablero[indice][j] << "  ";
	}
}

/* ME MUESTRA LA INTERFAZ PARA JUGAR EN PANTALLA*/
char InterfazDeOpciones()
{
	char tecla = ' '; 
	int numero = 0;

	pColumna > 16 ? numero = 7 : numero = 12;

	for (int i = 0; i < pColumna * numero; i++)
	{
		SetColor(13);
		cout << "=";
		SetColor(15);
	}

	SetColor(9);
	cout << "\n[1]- DESCRUBIR";
	SetColor(15);

	string auxMarcar = "";
	StartedGame == false ? SetColor(8) : SetColor(9);
	StartedGame == false ? auxMarcar = "\t[2]- MARCAR (DESACTIVADO)" : auxMarcar = "\t[2]- MARCAR";	
	cout << auxMarcar;
	SetColor(15);

	string auxDesMarcar = "";
	StartedGame == false ? SetColor(8) : SetColor(9);
	StartedGame == false ? auxDesMarcar = "\t[3]- DESMARCAR (DESACTIVADO)" : auxDesMarcar = "\t[3]- DESMARCAR";
	cout << auxDesMarcar;
	SetColor(15);

	SetColor(9);
	string auxReglas = "";
	mostrarReglas == true ? auxReglas = "\t[4]- DESACTIVAR REGLAS" : auxReglas = "\t[4]- ACTIVAR REGLAS";
	cout << auxReglas;
	SetColor(15);

	SetColor(8);
	string auxCheat = "";
	activarCheat == true ? auxCheat = "\t[5]- DESACTIVAR CHEAT" : auxCheat = "\t[5]- ACTIVAR CHEAT";
	cout << auxCheat;
	SetColor(15);

	SetColor(10);
	cout << "\t[6]- SALIR" << endl;
	SetColor(15);

	return tecla = _getch();
}

/* PASO EL NUMERO CHAR QUE SE TECLEO A ENTERO */
int CharToInt(char pChar)
{
	switch (pChar)
	{
	case '1':
		return 1;
		break;
	case '2':
		return 2;
		break;
	case '3':
		return 3;
		break;
	case '4':
		return 4;
		break;
	case '5':
		return 5;
		break;
	case '6':
		return 6;
		break;
	case '7':
		return 7;
		break;
	case '8':
		return 8;
		break;
	case '9':
		return 9;
		break;
	default:
		/*MensajeDeError();	*/	// Uso el default que tenga el switch en donde se este invocando la funcion.
		break;
	}
}


/* VALIDO EL INPUT CON UN MAX Y MIN */
void ValidarInput(int& pOpcion, int pMin, int pMax)
{
	do
	{
		cout << "SELECCION : ";
		cin >> pOpcion;

	} while (pOpcion < pMin || pOpcion > pMax);
}

/* VALIDO EL INPUT PARA QUE DETECTE LA TECLA APRETADA */
void ValidarInputChar(int& pOpcion, int pMin, int pMax)
{
	char tecla = ' ';
	do
	{
		tecla = _getch(); // Guardo la tecla, presionada por el usuario.
		pOpcion = CharToInt(tecla); // Valido que la tecla sea un numero del teclado.		
	} while (pOpcion < pMin || pOpcion > pMax);
}

/* MENSAJE DE ERROR */
void MensajeDeError()
{
	SetColor(12);
	cout << "\nSe produjo un error..." << endl;
	SetColor(15);

	system("pause");
}

/* SETEO EL COLOR */
void SetColor(int numero)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), numero);
}

/* MUESTRA UN MENSAJE PERSONALIZADO */
void MostrarMensaje(string pTexto)
{
	cout << pTexto << endl;
}

/* CARGA EL TABLERO, DESPUES DE INGRESAR LA POSICION. PARA GENERAR EL TABLERO APARTIR DE AHI */
void CargarElTablero(int** pTablero, POSICION pPosicion)
{
	SetMina(pTablero, pPosicion); // Seto las minas
	CalcularMinas(pTablero); // Calculo las casillasa adyacentes a cada minas y le sumo + 1.
}

/* CUENTA LA CANTIDAD DE MINAS */
int ContarMinas(int** pTableroBanderas)
{
	int contador = 0;

	for (size_t i(0); i < pFilas; i++)
	{
		for (size_t k(0); k < pColumna; k++)
		{
			if (pTableroBanderas[i][k] == (int)NUMEROS::MENOSUNO)
			{
				contador++;
			}
		}
	}
	 
	return cantidadMinas - contador; // Calcula las posible cantidad de minas restantes.
}


void MarcarMina(int** pTableroBanderas, bool** pTableroVisible, POSICION pPosicion)
{
	if (!pTableroVisible[pPosicion.fila][pPosicion.columna])
	{
		pTableroBanderas[pPosicion.fila][pPosicion.columna] = (int)NUMEROS::MENOSUNO;

		SetColor(2); // Seteo el color de la palabra.
		cout << "\nCASILLA MARCADA..." << endl;
		SetColor(15); // Seteo el color de la palabra.
	}
	else
	{
		SetColor(12); // Seteo el color de la palabra.
		cout << "\nCASILLA NO MARCADA..." << endl;
		SetColor(15); // Seteo el color de la palabra.
	}

	system("pause");
}

void DesMarcarMina(int** pTableroBanderas, bool** pTableroVisible, POSICION pPosicion)
{
	if (pTableroBanderas[pPosicion.fila][pPosicion.columna] == (int)NUMEROS::MENOSUNO)
	{
		pTableroBanderas[pPosicion.fila][pPosicion.columna] = (int)NUMEROS::CERO;

		SetColor(2); // Seteo el color de la palabra.
		cout << "\nCASILLA DESMARCADA..." << endl;
		SetColor(15); // Seteo el color de la palabra.

	}
	else
	{
		SetColor(12); // Seteo el color de la palabra.
		cout << "\nCASILLA NO DESMARCADA..." << endl;
		SetColor(15); // Seteo el color de la palabra.
	}

	system("pause");
}

/* SETEO EL COLOR DE LOS NUMEROS DEL TABLERO */
void SeColorNumeros(NUMEROS pNumero)
{
	switch (pNumero)
	{
	case NUMEROS::CERO:
		SetColor(15);
		break;
	case NUMEROS::UNO:
		SetColor(1);
		break;
	case NUMEROS::DOS:
		SetColor(2);
		break;
	case NUMEROS::TRES:
		SetColor(13);
		break;
	case NUMEROS::CUATRO:
		SetColor(11);
		break;
	case NUMEROS::CINCO:
		SetColor(5);
		break;
	case NUMEROS::SEIS:
		SetColor(6);
		break;
	case NUMEROS::SIETE:
		SetColor(7);
		break;
	case NUMEROS::OCHO:
		SetColor(9);
		break;
	case NUMEROS::NUEVE:
		SetColor(12);
		break;
	default:
		MensajeDeError();
		system("pause");
		break;
	}
}

/* by : SUAREZ BRIAN ALAN 2020 */
