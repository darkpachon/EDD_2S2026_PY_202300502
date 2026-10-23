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
#include "../Estructuras/GrafoSedes.h"
#include "../Estructuras/Blockchain.h"
#include "../Modelos/Json.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    // ----- Estructuras -----
    ArbolPeliculas* arbol;
    ArbolAVL* avlFunciones;
    ArbolB* arbolClientes;
    TablaHash* tablaReservas;
    MatrizAsientos* matriz;
    ListaCircularDoble* listaSolicitudes;
    ListaDeListas* listaPromociones;

    // ----- Fase 3 -----
    GrafoSedes* grafo;
    Blockchain* blockchain;
    std::vector<JsonValue> transaccionesPendientes;

    Cliente* clienteActual;
    int siguienteNumeroSolicitud;

    QStackedWidget* stack;

    // Login
    QLineEdit *txtUsuario, *txtPassword;
    QLabel* lblMensajeLogin;
    QPushButton *btnIngresar, *btnRegistrarse;

    // Admin Tab 1
    QLineEdit *txtCodigo, *txtTitulo, *txtGenero, *txtDuracion,
              *txtClasificacion, *txtIdioma, *txtFechaEstreno, *txtFechaFin;
    QComboBox* cmbOrdenPeliculas;
    QTableWidget* tblPeliculasAdmin;
    QLabel* lblMensajeAdmin;

    // Admin Tab 2
    QComboBox* cmbPeliculaFuncion;
    QLineEdit *txtHorarioFuncion, *txtSalaFuncion;
    QComboBox* cmbSedeFuncion;
    QSpinBox *spnFilasFuncion, *spnColumnasFuncion;
    QComboBox* cmbOrdenFunciones;
    QTableWidget* tblFuncionesAdmin;
    QLabel* lblMensajeFuncion;

    // Admin Tab 3
    QLabel* lblFuncionActivaAdmin;
    QTableWidget* tblAsientosAdmin;

    // Admin Tab 4
    QTableWidget* tblClientesAdmin;
    QLabel* lblMensajeClienteAdmin;

    // Admin Tab 5
    QTableWidget* tblReservasAdmin;
    QLabel* lblMensajeReservasAdmin;

    // Admin Tab 6
    QTextEdit* txtLogCarga;

    // Admin Tab 7
    QLabel* lblReporteAdmin;

    // Admin Tab 8 (Fase 3 Grafo)
    QLineEdit *txtCodSede, *txtNombreSede, *txtDirSede, *txtTelSede;
    QSpinBox* spnSalasSede;
    QPushButton* btnRegistrarSede;
    QTableWidget* tblSedes;
    QComboBox *cmbOrigenBFS, *cmbDestinoBFS;
    QPushButton *btnBFS, *btnRanking;
    QLabel* lblResultGrafo;
    QComboBox* cmbSedeVerPeliculas;
    QPushButton* btnVerPeliculasSede;
    QTableWidget* tblPeliculasSede;

    // Admin Tab 9 (Fase 3 Blockchain)
    QPushButton *btnMinar, *btnVerCadena, *btnVerMerkle, *btnVentasTotales;
    QTableWidget* tblBlockchain;
    QSpinBox* spnIndiceMerkle;
    QLabel* lblMerkle;
    QLabel* lblVentas;

    // Cliente Tab 1
    QLabel* lblBienvenidoCliente;
    QTableWidget* tblCarteleraCliente;
    QComboBox *cmbPeliculaCliente, *cmbFuncionCliente;
    QLabel* lblFuncionClienteActiva;
    QTableWidget* tblMapaAsientosCliente;
    QLabel* lblMensajeCliente;

    // Cliente Tab 2
    QTableWidget* tblMisReservas;

    // Cliente Tab 3
    QTextEdit* txtPromocionesCliente;

    // Cliente Tab 4
    QTextEdit* txtSolicitudesCliente;

    // Cliente Tab 5 (Fase 3)
    QComboBox* cmbPeliculaF3;
    QPushButton* btnSedesPorPelicula;
    QTableWidget* tblSedesPorPelicula;
    QComboBox* cmbSedeSimilar;
    QPushButton* btnVerSimilares;
    QTableWidget* tblSimilares;
    QPushButton* btnMiHistorial;
    QTableWidget* tblHistorial;
    QLineEdit* txtCodTransaccion;
    QPushButton* btnComprobante;

    // ----- Helpers de construcción -----
    void construirUI();
    QWidget* construirPaginaLogin();
    QWidget* construirPaginaAdmin();
    QWidget* construirPaginaCliente();

    // ----- Helpers de poblado -----
    void poblarCarteleraAdmin();
    void poblarComboPeliculasFuncion();
    void poblarComboSedesFuncion();
    void poblarFuncionesAdmin();
    void poblarTablaAsientos(QTableWidget* tabla, MatrizAsientos* m);
    void poblarClientesAdmin();
    void poblarReservasAdmin();
    void poblarCarteleraCliente();
    void poblarMisReservas();

    void poblarSedesAdmin();
    void poblarCombosGrafo();
    void poblarBlockchainAdmin();
    void poblarCombosClienteF3();

    void limpiarFormularioPelicula();
    void limpiarFormularioFuncion();

    void mostrarReporte(const QString& nombrePng);

    void guardarTodosLosDatos();
    void cargarDatosIniciales();

    void reconstruirTablaHashDesdeBlockchain();
    void reconstruirGrafoDesdeFunciones();

private slots:
    // Login
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

    // Reportes Fase 2
    void onReporteBST();
    void onReporteAVL();

    // Reportes Fase 3
    void onReporteGrafoSedes();
    void onReporteListaAdyacencia();
    void onReporteBlockchain();
    void onReporteMerkle();

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

    // Fase 3 admin: Grafo
    void onRegistrarSede();
    void onEjecutarBFS();
    void onVerRanking();
    void onVerPeliculasSede();

    // Fase 3 admin: Blockchain
    void onMinarBloque();
    void onVerCadena();
    void onVerMerkle();
    void onVerVentasTotales();

    // Fase 3 cliente
    void onSedesPorPelicula();
    void onVerSimilares();
    void onMiHistorial();
    void onVerComprobante();
};

#endif