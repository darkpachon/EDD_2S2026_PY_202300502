#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QCloseEvent>

#include "../Estructuras/ArbolPeliculas.h"
#include "../Estructuras/ArbolAVL.h"
#include "../Estructuras/ArbolB.h"
#include "../Estructuras/TablaHash.h"
#include "../Estructuras/MatrizAsientos.h"
#include "../Estructuras/ListaCircularDoble.h"
#include "../Estructuras/ListaDeListas.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    // -------------------- Estructuras --------------------
    ArbolPeliculas* arbol;
    ArbolAVL* avlFunciones;
    ArbolB* arbolClientes;
    TablaHash* tablaReservas;
    MatrizAsientos* matriz;
    ListaCircularDoble* listaSolicitudes;
    ListaDeListas* listaPromociones;

    Cliente* clienteActual;         // nullptr si no hay sesión de cliente
    int siguienteNumeroSolicitud;

    // -------------------- UI raíz --------------------
    QStackedWidget* stack;

    // Login
    QLineEdit* txtUsuario;
    QLineEdit* txtPassword;
    QLabel* lblMensajeLogin;
    QPushButton* btnIngresar;
    QPushButton* btnRegistrarse;

    // ------- Admin -------
    // Tab Películas
    QLineEdit *txtCodigo, *txtTitulo, *txtGenero, *txtDuracion,
              *txtClasificacion, *txtIdioma, *txtFechaEstreno, *txtFechaFin;
    QComboBox* cmbOrdenPeliculas;
    QTableWidget* tblPeliculasAdmin;
    QLabel* lblMensajeAdmin;

    // Tab Funciones
    QComboBox* cmbPeliculaFuncion;
    QLineEdit *txtHorarioFuncion, *txtSalaFuncion;
    QSpinBox  *spnFilasFuncion, *spnColumnasFuncion;
    QComboBox* cmbOrdenFunciones;
    QTableWidget* tblFuncionesAdmin;
    QLabel* lblMensajeFuncion;

    // Tab Asientos
    QLabel* lblFuncionActivaAdmin;
    QTableWidget* tblAsientosAdmin;

    // Tab Clientes
    QTableWidget* tblClientesAdmin;
    QLabel* lblMensajeClienteAdmin;

    // Tab Reservas
    QTableWidget* tblReservasAdmin;
    QLabel* lblMensajeReservasAdmin;

    // Tab Carga Masiva
    QTextEdit* txtLogCarga;

    // Tab Reportes
    QLabel* lblReporteAdmin;

    // ------- Cliente -------
    QLabel* lblBienvenidoCliente;
    QTableWidget* tblCarteleraCliente;
    QComboBox* cmbPeliculaCliente;
    QComboBox* cmbFuncionCliente;
    QLabel* lblFuncionClienteActiva;
    QTableWidget* tblMapaAsientosCliente;
    QLabel* lblMensajeCliente;
    QTableWidget* tblMisReservas;
    QTextEdit* txtPromocionesCliente;
    QTextEdit* txtSolicitudesCliente;

    // -------------------- Helpers de construcción --------------------
    void construirUI();
    QWidget* construirPaginaLogin();
    QWidget* construirPaginaAdmin();
    QWidget* construirPaginaCliente();

    // -------------------- Helpers de poblado --------------------
    void poblarCarteleraAdmin();
    void poblarComboPeliculasFuncion();
    void poblarFuncionesAdmin();
    void poblarTablaAsientos(QTableWidget* tabla, MatrizAsientos* m);
    void poblarClientesAdmin();
    void poblarReservasAdmin();
    void poblarCarteleraCliente();
    void poblarFuncionesCliente();
    void poblarMisReservas();
    void poblarPromociones();
    void poblarSolicitudesCliente();

    void limpiarFormularioPelicula();
    void limpiarFormularioFuncion();

    void mostrarReporte(const QString& nombrePng);

    // Persistencia
    void guardarTodosLosDatos();
    void cargarDatosIniciales();

private slots:
    // Login / logout
    void onLoginClicked();
    void onRegistrarseClicked();
    void onCerrarSesionAdmin();
    void onCerrarSesionCliente();

    // Películas
    void onInsertarPelicula();
    void onEditarPelicula();
    void onEliminarPelicula();
    void onBuscarPeliculaAdmin();
    void onVerCarteleraAdmin();

    // Funciones
    void onCrearFuncion();
    void onEditarFuncion();
    void onEliminarFuncion();
    void onBuscarFuncionAdmin();
    void onListarFunciones();
    void onCargarFuncionEnMapa();

    // Asientos admin
    void onLiberarAsientoAdmin();
    void onReporteMatrizAdmin();

    // Clientes admin
    void onBuscarClienteAdmin();
    void onEliminarClienteAdmin();
    void onVerReservasClienteAdmin();
    void onReporteArbolB();

    // Reservas admin
    void onBuscarReservaAdmin();
    void onEliminarReservaAdmin();
    void onReporteHash();

    // Carga masiva
    void onCargarPeliculasFunciones();
    void onCargarClientesReservas();
    void onGuardarTodoManual();

    // Reportes
    void onReporteBST();
    void onReporteAVL();

    // Cliente
    void onEditarPerfil();
    void onPeliculaClienteCambiada(int idx);
    void onFuncionClienteCambiada(int idx);
    void onReservarAsientoCliente();
    void onActualizarMisReservas();
    void onCancelarMiReserva();
    void onConsultarPromociones();
    void onNuevaSolicitud();
    void onConsultarSolicitudes();
};

#endif