#include "mainwindow.h"
#include "PerfilDialog.h"
#include "../Reportes/GeneradorDot.h"
#include "../Persistencia/Persistencia.h"
#include "../Utilidades/Validaciones.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QDate>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QDebug>
#include <functional>
#include <algorithm>
#include <map>
#include <set>

namespace {
const QString CARPETA_DATOS = "datos";

void asegurarCarpetaDatos() {
    QDir d;
    if (!d.exists(CARPETA_DATOS)) d.mkpath(CARPETA_DATOS);
    if (!d.exists("blocks")) d.mkpath("blocks");
    if (!d.exists("reportes_generados")) d.mkpath("reportes_generados");
}

QString rutaDatos(const QString& nombre) {
    return CARPETA_DATOS + "/" + nombre;
}

bool parsearCodigoPelicula(const QString& entrada, int& numero, QString& original) {
    QString s = entrada.trimmed();
    if (s.isEmpty()) return false;
    if (s.startsWith('P', Qt::CaseInsensitive)) s = s.mid(1);
    bool ok = false;
    numero = s.toInt(&ok);
    if (!ok || numero <= 0) return false;
    original = QString("P%1").arg(numero, 3, 10, QChar('0'));
    return true;
}

QString estadoPelicula(const Pelicula& p) {
    QDate hoy = QDate::currentDate();
    QDate est = QDate::fromString(QString::fromStdString(p.fecha_estreno), "yyyy-MM-dd");
    QDate fin = QDate::fromString(QString::fromStdString(p.fecha_fin), "yyyy-MM-dd");
    if (!est.isValid() || !fin.isValid()) return "Fecha inválida";
    if (hoy < est) return "Próximo estreno";
    if (hoy > fin) return "Fuera de cartelera";
    if (hoy.daysTo(fin) < 7) return "Próximo a retirar";
    return "En cartelera";
}
}

// =========================================================================
//  Constructor / Destructor
// =========================================================================

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    arbol              = new ArbolPeliculas();
    avlFunciones       = new ArbolAVL();
    arbolClientes      = new ArbolB();
    tablaReservas      = new TablaHash();
    matriz             = new MatrizAsientos();
    listaSolicitudes   = new ListaCircularDoble();
    listaPromociones   = new ListaDeListas();
    grafo              = new GrafoSedes();
    blockchain         = new Blockchain("blocks");

    clienteActual      = nullptr;
    siguienteNumeroSolicitud = 1;

    asegurarCarpetaDatos();
    construirUI();
    cargarDatosIniciales();

    setWindowTitle("CinemaUSAC - Fase 3");
    resize(1280, 850);
}

MainWindow::~MainWindow() {
    delete arbol;
    delete avlFunciones;
    delete arbolClientes;
    delete tablaReservas;
    delete matriz;
    delete listaSolicitudes;
    delete listaPromociones;
    delete grafo;
    delete blockchain;
}

// =========================================================================
//  Construcción de UI
// =========================================================================

void MainWindow::construirUI() {
    stack = new QStackedWidget(this);
    setCentralWidget(stack);
    stack->addWidget(construirPaginaLogin());
    stack->addWidget(construirPaginaAdmin());
    stack->addWidget(construirPaginaCliente());
    stack->setCurrentIndex(0);
}

QWidget* MainWindow::construirPaginaLogin() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    auto* titulo = new QLabel("CinemaUSAC - Iniciar Sesión");
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size:22px; font-weight:bold;");
    layout->addWidget(titulo);

    txtUsuario  = new QLineEdit(); txtUsuario->setPlaceholderText("Usuario / Correo");
    txtPassword = new QLineEdit(); txtPassword->setPlaceholderText("Contraseña");
    txtPassword->setEchoMode(QLineEdit::Password);
    layout->addWidget(txtUsuario);
    layout->addWidget(txtPassword);

    auto* botones = new QHBoxLayout();
    btnIngresar    = new QPushButton("Ingresar");
    btnRegistrarse = new QPushButton("Registrarse (Cliente)");
    botones->addWidget(btnIngresar);
    botones->addWidget(btnRegistrarse);
    layout->addLayout(botones);

    lblMensajeLogin = new QLabel();
    lblMensajeLogin->setStyleSheet("color:red; font-weight:bold;");
    lblMensajeLogin->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblMensajeLogin);

    connect(btnIngresar, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(btnRegistrarse, &QPushButton::clicked, this, &MainWindow::onRegistrarseClicked);
    connect(txtPassword, &QLineEdit::returnPressed, this, &MainWindow::onLoginClicked);
    return page;
}

QWidget* MainWindow::construirPaginaAdmin() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);

    auto* top = new QHBoxLayout();
    auto* titulo = new QLabel("Panel de Administrador");
    titulo->setStyleSheet("font-size:18px; font-weight:bold;");
    auto* btnGuardar = new QPushButton("Guardar cambios");
    auto* btnCerrar  = new QPushButton("Cerrar sesión");
    top->addWidget(titulo);
    top->addStretch();
    top->addWidget(btnGuardar);
    top->addWidget(btnCerrar);
    layout->addLayout(top);
    connect(btnGuardar, &QPushButton::clicked, this, &MainWindow::onGuardarTodoManual);
    connect(btnCerrar,  &QPushButton::clicked, this, &MainWindow::onCerrarSesionAdmin);

    auto* tabs = new QTabWidget();

    // ---- TAB 1: PELÍCULAS ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        auto* form = new QFormLayout();
        txtCodigo = new QLineEdit(); txtCodigo->setPlaceholderText("Ej: P001");
        txtTitulo = new QLineEdit();
        txtGenero = new QLineEdit();
        txtDuracion = new QLineEdit(); txtDuracion->setPlaceholderText("min");
        txtClasificacion = new QLineEdit();
        txtIdioma = new QLineEdit();
        txtFechaEstreno = new QLineEdit(); txtFechaEstreno->setPlaceholderText("AAAA-MM-DD");
        txtFechaFin = new QLineEdit(); txtFechaFin->setPlaceholderText("AAAA-MM-DD");
        form->addRow("Código:", txtCodigo);
        form->addRow("Título:", txtTitulo);
        form->addRow("Género:", txtGenero);
        form->addRow("Duración:", txtDuracion);
        form->addRow("Clasificación:", txtClasificacion);
        form->addRow("Idioma:", txtIdioma);
        form->addRow("Fecha estreno:", txtFechaEstreno);
        form->addRow("Fecha fin:", txtFechaFin);
        lay->addLayout(form);

        auto* bts = new QHBoxLayout();
        auto* bIns = new QPushButton("Insertar");
        auto* bEdit= new QPushButton("Editar");
        auto* bElim= new QPushButton("Eliminar");
        auto* bBus = new QPushButton("Buscar");
        auto* bVer = new QPushButton("Ver cartelera");
        cmbOrdenPeliculas = new QComboBox();
        cmbOrdenPeliculas->addItems({"Inorden","Preorden","Postorden"});
        bts->addWidget(bIns); bts->addWidget(bEdit); bts->addWidget(bElim);
        bts->addWidget(bBus); bts->addWidget(bVer);
        bts->addWidget(new QLabel("Orden:")); bts->addWidget(cmbOrdenPeliculas);
        lay->addLayout(bts);

        tblPeliculasAdmin = new QTableWidget();
        tblPeliculasAdmin->setColumnCount(9);
        tblPeliculasAdmin->setHorizontalHeaderLabels(
            {"Código","Título","Género","Duración","Clasif.","Idioma","Estreno","Fin","Estado"});
        tblPeliculasAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblPeliculasAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
        lay->addWidget(tblPeliculasAdmin);

        lblMensajeAdmin = new QLabel();
        lblMensajeAdmin->setStyleSheet("color:green; font-weight:bold;");
        lay->addWidget(lblMensajeAdmin);

        connect(bIns,  &QPushButton::clicked, this, &MainWindow::onInsertarPelicula);
        connect(bEdit, &QPushButton::clicked, this, &MainWindow::onEditarPelicula);
        connect(bElim, &QPushButton::clicked, this, &MainWindow::onEliminarPelicula);
        connect(bBus,  &QPushButton::clicked, this, &MainWindow::onBuscarPeliculaAdmin);
        connect(bVer,  &QPushButton::clicked, this, &MainWindow::onVerCarteleraAdmin);
        connect(cmbOrdenPeliculas, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int){ poblarCarteleraAdmin(); });
        tabs->addTab(tab, "1. Películas");
    }

    // ---- TAB 2: FUNCIONES ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        auto* form = new QFormLayout();
        cmbPeliculaFuncion = new QComboBox();
        cmbSedeFuncion     = new QComboBox();
        txtHorarioFuncion  = new QLineEdit(); txtHorarioFuncion->setPlaceholderText("HH:MM");
        txtSalaFuncion     = new QLineEdit();
        spnFilasFuncion    = new QSpinBox(); spnFilasFuncion->setRange(1,200); spnFilasFuncion->setValue(10);
        spnColumnasFuncion = new QSpinBox(); spnColumnasFuncion->setRange(1,200); spnColumnasFuncion->setValue(20);
        form->addRow("Película:", cmbPeliculaFuncion);
        form->addRow("Sede:",     cmbSedeFuncion);
        form->addRow("Horario:",  txtHorarioFuncion);
        form->addRow("Sala:",     txtSalaFuncion);
        form->addRow("Filas:",    spnFilasFuncion);
        form->addRow("Columnas:", spnColumnasFuncion);
        lay->addLayout(form);

        auto* bts = new QHBoxLayout();
        auto* bCrear = new QPushButton("Crear función");
        auto* bEdit  = new QPushButton("Editar");
        auto* bElim  = new QPushButton("Eliminar");
        auto* bBus   = new QPushButton("Buscar");
        auto* bList  = new QPushButton("Listar");
        auto* bMapa  = new QPushButton("Mapa asientos");
        cmbOrdenFunciones = new QComboBox();
        cmbOrdenFunciones->addItems({"Inorden","Preorden","Postorden"});
        bts->addWidget(bCrear); bts->addWidget(bEdit); bts->addWidget(bElim);
        bts->addWidget(bBus);   bts->addWidget(bList); bts->addWidget(bMapa);
        bts->addWidget(new QLabel("Orden:")); bts->addWidget(cmbOrdenFunciones);
        lay->addLayout(bts);

        tblFuncionesAdmin = new QTableWidget();
        tblFuncionesAdmin->setColumnCount(8);
        tblFuncionesAdmin->setHorizontalHeaderLabels(
            {"Código","Película","Sede","Horario","Sala","Filas","Columnas","Archivo"});
        tblFuncionesAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblFuncionesAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
        lay->addWidget(tblFuncionesAdmin);

        lblMensajeFuncion = new QLabel();
        lblMensajeFuncion->setStyleSheet("color:green; font-weight:bold;");
        lay->addWidget(lblMensajeFuncion);

        connect(bCrear, &QPushButton::clicked, this, &MainWindow::onCrearFuncion);
        connect(bEdit,  &QPushButton::clicked, this, &MainWindow::onEditarFuncion);
        connect(bElim,  &QPushButton::clicked, this, &MainWindow::onEliminarFuncion);
        connect(bBus,   &QPushButton::clicked, this, &MainWindow::onBuscarFuncionAdmin);
        connect(bList,  &QPushButton::clicked, this, &MainWindow::onListarFunciones);
        connect(bMapa,  &QPushButton::clicked, this, &MainWindow::onCargarFuncionEnMapa);
        connect(cmbOrdenFunciones, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int){ poblarFuncionesAdmin(); });
        tabs->addTab(tab, "2. Funciones");
    }

    // ---- TAB 3: ASIENTOS ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        lblFuncionActivaAdmin = new QLabel("Función activa: (ninguna)");
        lblFuncionActivaAdmin->setStyleSheet("font-weight:bold;");
        lay->addWidget(lblFuncionActivaAdmin);
        tblAsientosAdmin = new QTableWidget();
        tblAsientosAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
        lay->addWidget(tblAsientosAdmin);
        auto* bts = new QHBoxLayout();
        auto* bLib = new QPushButton("Liberar asiento");
        auto* bRep = new QPushButton("Reporte matriz");
        bts->addWidget(bLib); bts->addWidget(bRep);
        lay->addLayout(bts);
        connect(bLib, &QPushButton::clicked, this, &MainWindow::onLiberarAsientoAdmin);
        connect(bRep, &QPushButton::clicked, this, &MainWindow::onReporteMatrizAdmin);
        tabs->addTab(tab, "3. Asientos");
    }

    // ---- TAB 4: CLIENTES ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        tblClientesAdmin = new QTableWidget();
        tblClientesAdmin->setColumnCount(6);
        tblClientesAdmin->setHorizontalHeaderLabels(
            {"ID","Nombre","Correo","Teléfono","Tipo","# Reservas"});
        tblClientesAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblClientesAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
        lay->addWidget(tblClientesAdmin);
        auto* bts = new QHBoxLayout();
        auto* bBus = new QPushButton("Buscar");
        auto* bEli = new QPushButton("Eliminar");
        auto* bVer = new QPushButton("Ver reservas");
        auto* bRep = new QPushButton("Reporte B");
        bts->addWidget(bBus); bts->addWidget(bEli); bts->addWidget(bVer); bts->addWidget(bRep);
        lay->addLayout(bts);
        lblMensajeClienteAdmin = new QLabel();
        lblMensajeClienteAdmin->setStyleSheet("color:green; font-weight:bold;");
        lay->addWidget(lblMensajeClienteAdmin);
        connect(bBus, &QPushButton::clicked, this, &MainWindow::onBuscarClienteAdmin);
        connect(bEli, &QPushButton::clicked, this, &MainWindow::onEliminarClienteAdmin);
        connect(bVer, &QPushButton::clicked, this, &MainWindow::onVerReservasClienteAdmin);
        connect(bRep, &QPushButton::clicked, this, &MainWindow::onReporteArbolB);
        tabs->addTab(tab, "4. Clientes");
    }

    // ---- TAB 5: RESERVAS ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        tblReservasAdmin = new QTableWidget();
        tblReservasAdmin->setColumnCount(6);
        tblReservasAdmin->setHorizontalHeaderLabels(
            {"Código","Función","Cliente","Fila","Columna","Fecha"});
        tblReservasAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblReservasAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
        lay->addWidget(tblReservasAdmin);
        auto* bts = new QHBoxLayout();
        auto* bBus = new QPushButton("Buscar");
        auto* bEli = new QPushButton("Eliminar");
        auto* bRep = new QPushButton("Reporte Hash");
        bts->addWidget(bBus); bts->addWidget(bEli); bts->addWidget(bRep);
        lay->addLayout(bts);
        lblMensajeReservasAdmin = new QLabel();
        lblMensajeReservasAdmin->setStyleSheet("color:green; font-weight:bold;");
        lay->addWidget(lblMensajeReservasAdmin);
        connect(bBus, &QPushButton::clicked, this, &MainWindow::onBuscarReservaAdmin);
        connect(bEli, &QPushButton::clicked, this, &MainWindow::onEliminarReservaAdmin);
        connect(bRep, &QPushButton::clicked, this, &MainWindow::onReporteHash);
        tabs->addTab(tab, "5. Reservas");
    }

    // ---- TAB 6: CARGA MASIVA ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        auto* bPel = new QPushButton("Cargar Películas + Sedes + Funciones (JSON)");
        auto* bCli = new QPushButton("Cargar Clientes + Reservas (JSON)");
        lay->addWidget(bPel); lay->addWidget(bCli);
        txtLogCarga = new QTextEdit(); txtLogCarga->setReadOnly(true);
        lay->addWidget(txtLogCarga);
        connect(bPel, &QPushButton::clicked, this, &MainWindow::onCargarPeliculasFunciones);
        connect(bCli, &QPushButton::clicked, this, &MainWindow::onCargarClientesReservas);
        tabs->addTab(tab, "6. Carga masiva");
    }

    // ---- TAB 7: REPORTES ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        auto* bts1 = new QHBoxLayout();
        auto* bBST = new QPushButton("BST Películas");
        auto* bAVL = new QPushButton("AVL Funciones");
        auto* bB   = new QPushButton("Árbol B");
        auto* bH   = new QPushButton("Tabla Hash");
        auto* bM   = new QPushButton("Matriz activa");
        bts1->addWidget(bBST); bts1->addWidget(bAVL); bts1->addWidget(bB);
        bts1->addWidget(bH); bts1->addWidget(bM);
        lay->addLayout(bts1);

        auto* bts2 = new QHBoxLayout();
        auto* bGS = new QPushButton("Grafo Sedes");
        auto* bLA = new QPushButton("Lista Adyacencia");
        auto* bBC = new QPushButton("Blockchain");
        auto* bMK = new QPushButton("Merkle");
        bts2->addWidget(bGS); bts2->addWidget(bLA); bts2->addWidget(bBC); bts2->addWidget(bMK);
        lay->addLayout(bts2);

        lblReporteAdmin = new QLabel("El reporte aparecerá aquí");
        lblReporteAdmin->setMinimumHeight(400);
        lblReporteAdmin->setAlignment(Qt::AlignCenter);
        lblReporteAdmin->setFrameShape(QFrame::Box);
        lay->addWidget(lblReporteAdmin);

        connect(bBST, &QPushButton::clicked, this, &MainWindow::onReporteBST);
        connect(bAVL, &QPushButton::clicked, this, &MainWindow::onReporteAVL);
        connect(bB,   &QPushButton::clicked, this, &MainWindow::onReporteArbolB);
        connect(bH,   &QPushButton::clicked, this, &MainWindow::onReporteHash);
        connect(bM,   &QPushButton::clicked, this, &MainWindow::onReporteMatrizAdmin);
        connect(bGS,  &QPushButton::clicked, this, &MainWindow::onReporteGrafoSedes);
        connect(bLA,  &QPushButton::clicked, this, &MainWindow::onReporteListaAdyacencia);
        connect(bBC,  &QPushButton::clicked, this, &MainWindow::onReporteBlockchain);
        connect(bMK,  &QPushButton::clicked, this, &MainWindow::onReporteMerkle);
        tabs->addTab(tab, "7. Reportes");
    }

    // ---- TAB 8: GRAFO (Fase 3) ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* gbSede = new QGroupBox("Registrar Sede");
        auto* formSede = new QFormLayout(gbSede);
        txtCodSede    = new QLineEdit(); txtCodSede->setPlaceholderText("s001");
        txtNombreSede = new QLineEdit();
        txtDirSede    = new QLineEdit();
        txtTelSede    = new QLineEdit();
        spnSalasSede  = new QSpinBox(); spnSalasSede->setRange(1,50); spnSalasSede->setValue(3);
        formSede->addRow("Código:", txtCodSede);
        formSede->addRow("Nombre:", txtNombreSede);
        formSede->addRow("Dirección:", txtDirSede);
        formSede->addRow("Salas:", spnSalasSede);
        formSede->addRow("Teléfono:", txtTelSede);
        btnRegistrarSede = new QPushButton("Registrar sede");
        formSede->addRow(btnRegistrarSede);
        lay->addWidget(gbSede);

        tblSedes = new QTableWidget();
        tblSedes->setColumnCount(5);
        tblSedes->setHorizontalHeaderLabels({"Código","Nombre","Dirección","Salas","Teléfono"});
        tblSedes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        lay->addWidget(tblSedes);

        auto* gbBFS = new QGroupBox("Camino más corto (BFS)");
        auto* hBFS = new QHBoxLayout(gbBFS);
        cmbOrigenBFS  = new QComboBox();
        cmbDestinoBFS = new QComboBox();
        btnBFS = new QPushButton("Buscar camino");
        hBFS->addWidget(new QLabel("Origen:"));  hBFS->addWidget(cmbOrigenBFS);
        hBFS->addWidget(new QLabel("Destino:")); hBFS->addWidget(cmbDestinoBFS);
        hBFS->addWidget(btnBFS);
        lay->addWidget(gbBFS);

        auto* hRank = new QHBoxLayout();
        btnRanking = new QPushButton("Ver ranking de aristas");
        hRank->addWidget(btnRanking);
        lay->addLayout(hRank);

        lblResultGrafo = new QLabel();
        lblResultGrafo->setWordWrap(true);
        lblResultGrafo->setFrameShape(QFrame::Box);
        lblResultGrafo->setMinimumHeight(80);
        lay->addWidget(lblResultGrafo);

        auto* gbPelis = new QGroupBox("Películas por sede");
        auto* hPelis = new QHBoxLayout(gbPelis);
        cmbSedeVerPeliculas = new QComboBox();
        btnVerPeliculasSede = new QPushButton("Ver películas");
        hPelis->addWidget(cmbSedeVerPeliculas);
        hPelis->addWidget(btnVerPeliculasSede);
        lay->addWidget(gbPelis);

        tblPeliculasSede = new QTableWidget();
        tblPeliculasSede->setColumnCount(4);
        tblPeliculasSede->setHorizontalHeaderLabels({"Película","Función","Horario","Sala"});
        tblPeliculasSede->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        lay->addWidget(tblPeliculasSede);

        connect(btnRegistrarSede, &QPushButton::clicked, this, &MainWindow::onRegistrarSede);
        connect(btnBFS,           &QPushButton::clicked, this, &MainWindow::onEjecutarBFS);
        connect(btnRanking,       &QPushButton::clicked, this, &MainWindow::onVerRanking);
        connect(btnVerPeliculasSede, &QPushButton::clicked, this, &MainWindow::onVerPeliculasSede);
        tabs->addTab(tab, "8. Grafo de Sedes (F3)");
    }

    // ---- TAB 9: BLOCKCHAIN (Fase 3) ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* hTop = new QHBoxLayout();
        btnMinar = new QPushButton("Minar bloque con pendientes");
        btnVentasTotales = new QPushButton("Ver ventas totales");
        hTop->addWidget(btnMinar);
        hTop->addWidget(btnVentasTotales);
        lay->addLayout(hTop);

        btnVerCadena = new QPushButton("Ver cadena de bloques");
        lay->addWidget(btnVerCadena);

        tblBlockchain = new QTableWidget();
        tblBlockchain->setColumnCount(5);
        tblBlockchain->setHorizontalHeaderLabels({"Index","Nonce","Hash","Prev","Integridad"});
        tblBlockchain->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        lay->addWidget(tblBlockchain);

        auto* gbMerkle = new QGroupBox("Árbol de Merkle");
        auto* hMer = new QHBoxLayout(gbMerkle);
        spnIndiceMerkle = new QSpinBox(); spnIndiceMerkle->setRange(0, 999); spnIndiceMerkle->setValue(0);
        btnVerMerkle = new QPushButton("Ver árbol del bloque #");
        hMer->addWidget(new QLabel("Índice:")); hMer->addWidget(spnIndiceMerkle);
        hMer->addWidget(btnVerMerkle);
        lay->addWidget(gbMerkle);

        lblMerkle = new QLabel();
        lblMerkle->setMinimumHeight(250);
        lblMerkle->setAlignment(Qt::AlignCenter);
        lblMerkle->setFrameShape(QFrame::Box);
        lay->addWidget(lblMerkle);

        lblVentas = new QLabel();
        lblVentas->setWordWrap(true);
        lblVentas->setFrameShape(QFrame::Box);
        lblVentas->setMinimumHeight(60);
        lay->addWidget(lblVentas);

        connect(btnMinar, &QPushButton::clicked, this, &MainWindow::onMinarBloque);
        connect(btnVerCadena, &QPushButton::clicked, this, &MainWindow::onVerCadena);
        connect(btnVerMerkle, &QPushButton::clicked, this, &MainWindow::onVerMerkle);
        connect(btnVentasTotales, &QPushButton::clicked, this, &MainWindow::onVerVentasTotales);

        tabs->addTab(tab, "9. Blockchain (F3)");
    }

    layout->addWidget(tabs);
    return page;
}

QWidget* MainWindow::construirPaginaCliente() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);

    auto* top = new QHBoxLayout();
    lblBienvenidoCliente = new QLabel("Bienvenido");
    lblBienvenidoCliente->setStyleSheet("font-size:16px; font-weight:bold;");
    auto* btnPerfil = new QPushButton("Editar perfil");
    auto* btnCerrar = new QPushButton("Cerrar sesión");
    top->addWidget(lblBienvenidoCliente);
    top->addStretch();
    top->addWidget(btnPerfil);
    top->addWidget(btnCerrar);
    layout->addLayout(top);
    connect(btnPerfil, &QPushButton::clicked, this, &MainWindow::onEditarPerfil);
    connect(btnCerrar, &QPushButton::clicked, this, &MainWindow::onCerrarSesionCliente);

    auto* tabs = new QTabWidget();

    // ---- TAB 1: CARTELERA Y RESERVA ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        tblCarteleraCliente = new QTableWidget();
        tblCarteleraCliente->setColumnCount(4);
        tblCarteleraCliente->setHorizontalHeaderLabels({"Código","Título","Género","Estado"});
        tblCarteleraCliente->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblCarteleraCliente->setMaximumHeight(180);
        lay->addWidget(new QLabel("Cartelera:"));
        lay->addWidget(tblCarteleraCliente);

        auto* form = new QFormLayout();
        cmbPeliculaCliente = new QComboBox();
        cmbFuncionCliente  = new QComboBox();
        form->addRow("Película:", cmbPeliculaCliente);
        form->addRow("Función:",  cmbFuncionCliente);
        lay->addLayout(form);

        lblFuncionClienteActiva = new QLabel("Función activa: (ninguna)");
        lblFuncionClienteActiva->setStyleSheet("font-weight:bold;");
        lay->addWidget(lblFuncionClienteActiva);

        tblMapaAsientosCliente = new QTableWidget();
        tblMapaAsientosCliente->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblMapaAsientosCliente->setSelectionMode(QAbstractItemView::SingleSelection);
        tblMapaAsientosCliente->setSelectionBehavior(QAbstractItemView::SelectItems);
        lay->addWidget(new QLabel("Mapa de asientos:"));
        lay->addWidget(tblMapaAsientosCliente);

        auto* bRes = new QPushButton("Reservar y Pagar");
        lay->addWidget(bRes);
        lblMensajeCliente = new QLabel();
        lblMensajeCliente->setStyleSheet("color:green; font-weight:bold;");
        lay->addWidget(lblMensajeCliente);

        connect(cmbPeliculaCliente, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onPeliculaClienteCambiada);
        connect(cmbFuncionCliente, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onFuncionClienteCambiada);
        connect(bRes, &QPushButton::clicked, this, &MainWindow::onReservarAsientoCliente);
        tabs->addTab(tab, "1. Cartelera y Reservas");
    }

    // ---- TAB 2: MIS RESERVAS ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        tblMisReservas = new QTableWidget();
        tblMisReservas->setColumnCount(6);
        tblMisReservas->setHorizontalHeaderLabels(
            {"Código","Función","Fecha","Fila","Columna","Estado"});
        tblMisReservas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblMisReservas->setSelectionBehavior(QAbstractItemView::SelectRows);
        lay->addWidget(tblMisReservas);
        auto* bts = new QHBoxLayout();
        auto* bAct = new QPushButton("Actualizar");
        auto* bCan = new QPushButton("Cancelar reserva");
        bts->addWidget(bAct); bts->addWidget(bCan);
        lay->addLayout(bts);
        connect(bAct, &QPushButton::clicked, this, &MainWindow::onActualizarMisReservas);
        connect(bCan, &QPushButton::clicked, this, &MainWindow::onCancelarMiReserva);
        tabs->addTab(tab, "2. Mis Reservas");
    }

    // ---- TAB 3: PROMOCIONES ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        auto* bVer = new QPushButton("Ver promociones activas");
        txtPromocionesCliente = new QTextEdit(); txtPromocionesCliente->setReadOnly(true);
        lay->addWidget(bVer); lay->addWidget(txtPromocionesCliente);
        connect(bVer, &QPushButton::clicked, this, &MainWindow::onConsultarPromociones);
        tabs->addTab(tab, "3. Promociones");
    }

    // ---- TAB 4: SOLICITUDES ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);
        auto* bts = new QHBoxLayout();
        auto* bNew = new QPushButton("Nueva solicitud");
        auto* bVer = new QPushButton("Consultar mis solicitudes");
        bts->addWidget(bNew); bts->addWidget(bVer);
        lay->addLayout(bts);
        txtSolicitudesCliente = new QTextEdit(); txtSolicitudesCliente->setReadOnly(true);
        lay->addWidget(txtSolicitudesCliente);
        connect(bNew, &QPushButton::clicked, this, &MainWindow::onNuevaSolicitud);
        connect(bVer, &QPushButton::clicked, this, &MainWindow::onConsultarSolicitudes);
        tabs->addTab(tab, "4. Solicitudes");
    }

    // ---- TAB 5: RECOMENDACIONES Y COMPROBANTES (Fase 3) ----
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* gbPeli = new QGroupBox("Sedes donde está disponible una película");
        auto* hPeli = new QHBoxLayout(gbPeli);
        cmbPeliculaF3 = new QComboBox();
        btnSedesPorPelicula = new QPushButton("Buscar sedes");
        hPeli->addWidget(cmbPeliculaF3);
        hPeli->addWidget(btnSedesPorPelicula);
        lay->addWidget(gbPeli);

        tblSedesPorPelicula = new QTableWidget();
        tblSedesPorPelicula->setColumnCount(4);
        tblSedesPorPelicula->setHorizontalHeaderLabels({"Sede","Función","Horario","Sala"});
        tblSedesPorPelicula->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblSedesPorPelicula->setMaximumHeight(150);
        lay->addWidget(tblSedesPorPelicula);

        auto* gbSim = new QGroupBox("Sedes similares");
        auto* hSim = new QHBoxLayout(gbSim);
        cmbSedeSimilar = new QComboBox();
        btnVerSimilares = new QPushButton("Ver similares");
        hSim->addWidget(cmbSedeSimilar);
        hSim->addWidget(btnVerSimilares);
        lay->addWidget(gbSim);

        tblSimilares = new QTableWidget();
        tblSimilares->setColumnCount(2);
        tblSimilares->setHorizontalHeaderLabels({"Sede","Peso (películas en común)"});
        tblSimilares->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblSimilares->setMaximumHeight(120);
        lay->addWidget(tblSimilares);

        auto* gbHist = new QGroupBox("Mi historial (desde la blockchain)");
        auto* hHist = new QHBoxLayout(gbHist);
        btnMiHistorial = new QPushButton("Ver mi historial completo");
        hHist->addWidget(btnMiHistorial);
        lay->addWidget(gbHist);

        tblHistorial = new QTableWidget();
        tblHistorial->setColumnCount(5);
        tblHistorial->setHorizontalHeaderLabels({"Transacción","Reserva","Función","Monto","Bloque#"});
        tblHistorial->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblHistorial->setMaximumHeight(150);
        lay->addWidget(tblHistorial);

        auto* gbCom = new QGroupBox("Comprobante de transacción");
        auto* hCom = new QHBoxLayout(gbCom);
        txtCodTransaccion = new QLineEdit(); txtCodTransaccion->setPlaceholderText("T001");
        btnComprobante = new QPushButton("Ver comprobante");
        hCom->addWidget(txtCodTransaccion);
        hCom->addWidget(btnComprobante);
        lay->addWidget(gbCom);

        connect(btnSedesPorPelicula, &QPushButton::clicked, this, &MainWindow::onSedesPorPelicula);
        connect(btnVerSimilares,     &QPushButton::clicked, this, &MainWindow::onVerSimilares);
        connect(btnMiHistorial,      &QPushButton::clicked, this, &MainWindow::onMiHistorial);
        connect(btnComprobante,      &QPushButton::clicked, this, &MainWindow::onVerComprobante);
        tabs->addTab(tab, "5. Recomendaciones (F3)");
    }

    layout->addWidget(tabs);
    return page;
}

// =========================================================================
//  Carga / guardado
// =========================================================================

void MainWindow::cargarDatosIniciales() {
    bool eddOk = false;
    if (QFileInfo::exists("datos/sedes.edd") &&
        QFileInfo::exists("datos/peliculas.edd") &&
        QFileInfo::exists("datos/funciones.edd")) {
        eddOk = Persistencia::cargarSedesEDD("datos/sedes.edd", grafo);
        eddOk &= Persistencia::cargarPeliculasEDD("datos/peliculas.edd", arbol);
        eddOk &= Persistencia::cargarFuncionesEDD("datos/funciones.edd", avlFunciones);
        if (eddOk) qDebug() << "Datos cargados desde .edd";
    }

    if (!eddOk) {
        QString rutaCart = rutaDatos("cartelera.json");
        if (QFileInfo::exists(rutaCart)) {
            std::vector<std::string> err;
            Persistencia::cargarPeliculasYFunciones(rutaCart.toStdString(),
                                                    arbol, avlFunciones, grafo, err);
        }
    }

    QString rutaCli = rutaDatos("clientes.json");
    if (QFileInfo::exists(rutaCli)) {
        std::vector<std::string> err;
        Persistencia::cargarClientesYReservas(rutaCli.toStdString(), arbolClientes,
                                              tablaReservas, avlFunciones, err);
    }

    // Fase 3: reconstruir tabla hash desde la blockchain
    blockchain->cargarDesdeDisco();
    reconstruirTablaHashDesdeBlockchain();
    reconstruirGrafoDesdeFunciones();

    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
    poblarComboSedesFuncion();
    poblarFuncionesAdmin();
    poblarClientesAdmin();
    poblarReservasAdmin();
    poblarCarteleraCliente();
    poblarSedesAdmin();
    poblarCombosGrafo();
    poblarBlockchainAdmin();
    poblarCombosClienteF3();
}

void MainWindow::reconstruirTablaHashDesdeBlockchain() {
    tablaReservas->limpiar();
    auto txs = blockchain->todasTransaccionesValidas();
    for (const auto& tx : txs) {
        string codRes = tx.getTexto("codigo_reserva", "");
        if (codRes.empty()) continue;
        Reserva r(codRes,
                  tx.getTexto("codigo_funcion", ""),
                  tx.getTexto("id_cliente", ""),
                  tx.getEntero("fila", 0),
                  tx.getEntero("columna", 0),
                  tx.getTexto("fecha_pago", ""));
        tablaReservas->insertar(r);
    }
}

void MainWindow::reconstruirGrafoDesdeFunciones() {
    std::map<std::string, std::set<std::string>> catalogo;
    for (const auto& f : avlFunciones->inorden()) {
        if (!f.sede.empty()) catalogo[f.sede].insert(f.codigoPelicula);
    }
    grafo->reconstruirDesdeCatalogo(catalogo);
}

void MainWindow::guardarTodosLosDatos() {
    asegurarCarpetaDatos();
    Persistencia::guardarCartelera(rutaDatos("cartelera.json").toStdString(), arbol, avlFunciones);
    Persistencia::guardarClientes(rutaDatos("clientes.json").toStdString(), arbolClientes, tablaReservas);
    Persistencia::guardarReservas(rutaDatos("reservas.json").toStdString(), tablaReservas);
    if (!matriz->getCodigoFuncion().empty()) {
        Persistencia::guardarAsientosFuncion(
            rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
            matriz);
    }
    // Fase 3: guardar .edd
    Persistencia::guardarSedesEDD("datos/sedes.edd", grafo);
    Persistencia::guardarPeliculasEDD("datos/peliculas.edd", arbol);
    Persistencia::guardarFuncionesEDD("datos/funciones.edd", avlFunciones);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    guardarTodosLosDatos();
    event->accept();
}

// =========================================================================
//  Login / logout
// =========================================================================

void MainWindow::onLoginClicked() {
    QString u = txtUsuario->text().trimmed();
    QString p = txtPassword->text();
    if ((u == "admin" && p == "admin123") ||
        (u == "AdminCine@gmail.com" && p == "admin123Pass")) {
        stack->setCurrentIndex(1);
        txtUsuario->clear(); txtPassword->clear();
        lblMensajeLogin->clear();
        poblarCarteleraAdmin();
        poblarComboPeliculasFuncion();
        poblarComboSedesFuncion();
        poblarFuncionesAdmin();
        poblarClientesAdmin();
        poblarReservasAdmin();
        poblarSedesAdmin();
        poblarCombosGrafo();
        poblarBlockchainAdmin();
        return;
    }
    Cliente* c = arbolClientes->buscar(u.toStdString());
    if (c == nullptr) {
        for (auto& cli : arbolClientes->listar()) {
            if (cli.correo == u.toStdString()) { c = arbolClientes->buscar(cli.id); break; }
        }
    }
    if (c != nullptr && c->password == p.toStdString()) {
        clienteActual = c;
        lblBienvenidoCliente->setText(QString("Bienvenido, %1 (%2)")
            .arg(QString::fromStdString(c->nombre))
            .arg(QString::fromStdString(c->id)));
        stack->setCurrentIndex(2);
        txtUsuario->clear(); txtPassword->clear();
        lblMensajeLogin->clear();
        poblarCarteleraCliente();
        poblarMisReservas();
        poblarCombosClienteF3();
        return;
    }
    lblMensajeLogin->setText("Credenciales inválidas");
}

void MainWindow::onRegistrarseClicked() {
    bool ok;
    QString id = QInputDialog::getText(this, "Registro", "ID de cliente:", QLineEdit::Normal, "", &ok);
    if (!ok || id.trimmed().isEmpty()) return;
    QString nombre = QInputDialog::getText(this, "Registro", "Nombre:", QLineEdit::Normal, "", &ok);
    if (!ok || nombre.trimmed().isEmpty()) return;
    QString correo = QInputDialog::getText(this, "Registro", "Correo:", QLineEdit::Normal, "", &ok);
    if (!ok || correo.trimmed().isEmpty()) return;
    QString tel = QInputDialog::getText(this, "Registro", "Teléfono:", QLineEdit::Normal, "", &ok);
    if (!ok || tel.trimmed().isEmpty()) return;
    QString pass = QInputDialog::getText(this, "Registro", "Contraseña:", QLineEdit::Password, "", &ok);
    if (!ok || pass.isEmpty()) return;

    if (!Val::esCorreoValido(correo.toStdString())) { QMessageBox::warning(this, "Registro", "Correo inválido"); return; }
    if (!Val::esTelefonoValido(tel.toStdString()))   { QMessageBox::warning(this, "Registro", "Teléfono inválido"); return; }
    if (arbolClientes->buscar(id.trimmed().toStdString()) != nullptr) {
        QMessageBox::warning(this, "Registro", "ID ya existe"); return;
    }
    Cliente nuevo(id.trimmed().toStdString(), nombre.toStdString(), correo.toStdString(),
                  tel.toStdString(), pass.toStdString(), "cliente");
    arbolClientes->insertar(nuevo);
    QMessageBox::information(this, "Registro", "Cliente registrado.");
    poblarClientesAdmin();
}

void MainWindow::onCerrarSesionAdmin() {
    guardarTodosLosDatos();
    clienteActual = nullptr;
    stack->setCurrentIndex(0);
}

void MainWindow::onCerrarSesionCliente() {
    clienteActual = nullptr;
    stack->setCurrentIndex(0);
}

// =========================================================================
//  Películas
// =========================================================================

void MainWindow::limpiarFormularioPelicula() {
    txtCodigo->clear(); txtTitulo->clear(); txtGenero->clear();
    txtDuracion->clear(); txtClasificacion->clear(); txtIdioma->clear();
    txtFechaEstreno->clear(); txtFechaFin->clear();
}

void MainWindow::onInsertarPelicula() {
    int cod; QString orig;
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) { lblMensajeAdmin->setText("Código inválido"); return; }
    if (arbol->buscar(cod) != nullptr) { lblMensajeAdmin->setText("Ya existe"); return; }
    QString titulo = txtTitulo->text().trimmed();
    QString genero = txtGenero->text().trimmed();
    QString durStr = txtDuracion->text().trimmed();
    QString clasif = txtClasificacion->text().trimmed();
    QString idioma = txtIdioma->text().trimmed();
    QString fEst   = txtFechaEstreno->text().trimmed();
    QString fFin   = txtFechaFin->text().trimmed();
    if (titulo.isEmpty() || genero.isEmpty() || clasif.isEmpty() || idioma.isEmpty() || durStr.isEmpty()) {
        lblMensajeAdmin->setText("Complete todos los campos"); return;
    }
    if (!Val::esEnteroPositivo(durStr.toStdString())) { lblMensajeAdmin->setText("Duración inválida"); return; }
    if (!Val::esFechaValida(fEst.toStdString()) || !Val::esFechaValida(fFin.toStdString())) {
        lblMensajeAdmin->setText("Fechas inválidas"); return;
    }
    Pelicula p(cod, orig.toStdString(), titulo.toStdString(), genero.toStdString(),
               durStr.toInt(), clasif.toStdString(), idioma.toStdString(),
               fEst.toStdString(), fFin.toStdString());
    arbol->insertar(p);
    limpiarFormularioPelicula();
    lblMensajeAdmin->setText("Película insertada");
    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
}

void MainWindow::onEditarPelicula() {
    int cod; QString orig;
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) return;
    Pelicula* p = arbol->buscar(cod);
    if (p == nullptr) return;
    if (!txtTitulo->text().trimmed().isEmpty())        p->titulo = txtTitulo->text().trimmed().toStdString();
    if (!txtGenero->text().trimmed().isEmpty())        p->genero = txtGenero->text().trimmed().toStdString();
    if (Val::esEnteroPositivo(txtDuracion->text().trimmed().toStdString()))
        p->duracion = txtDuracion->text().trimmed().toInt();
    if (!txtClasificacion->text().trimmed().isEmpty()) p->clasificacion = txtClasificacion->text().trimmed().toStdString();
    if (!txtIdioma->text().trimmed().isEmpty())        p->idioma = txtIdioma->text().trimmed().toStdString();
    if (Val::esFechaValida(txtFechaEstreno->text().trimmed().toStdString()))
        p->fecha_estreno = txtFechaEstreno->text().trimmed().toStdString();
    if (Val::esFechaValida(txtFechaFin->text().trimmed().toStdString()))
        p->fecha_fin = txtFechaFin->text().trimmed().toStdString();
    lblMensajeAdmin->setText("Actualizada");
    poblarCarteleraAdmin();
}

void MainWindow::onEliminarPelicula() {
    int cod; QString orig;
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) return;
    if (arbol->buscar(cod) == nullptr) return;
    arbol->eliminar(cod);
    limpiarFormularioPelicula();
    lblMensajeAdmin->setText("Eliminada");
    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
}

void MainWindow::onBuscarPeliculaAdmin() {
    int cod; QString orig;
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) return;
    Pelicula* p = arbol->buscar(cod);
    if (p == nullptr) return;
    txtTitulo->setText(QString::fromStdString(p->titulo));
    txtGenero->setText(QString::fromStdString(p->genero));
    txtDuracion->setText(QString::number(p->duracion));
    txtClasificacion->setText(QString::fromStdString(p->clasificacion));
    txtIdioma->setText(QString::fromStdString(p->idioma));
    txtFechaEstreno->setText(QString::fromStdString(p->fecha_estreno));
    txtFechaFin->setText(QString::fromStdString(p->fecha_fin));
}

void MainWindow::poblarCarteleraAdmin() {
    tblPeliculasAdmin->setRowCount(0);
    int orden = cmbOrdenPeliculas ? cmbOrdenPeliculas->currentIndex() : 0;
    std::vector<NodoArbol*> nodos;
    std::function<void(NodoArbol*)> pre = [&](NodoArbol* n){ if(!n)return; nodos.push_back(n); pre(n->izquierdo); pre(n->derecho); };
    std::function<void(NodoArbol*)> in  = [&](NodoArbol* n){ if(!n)return; in(n->izquierdo); nodos.push_back(n); in(n->derecho); };
    std::function<void(NodoArbol*)> post= [&](NodoArbol* n){ if(!n)return; post(n->izquierdo); post(n->derecho); nodos.push_back(n); };
    if (orden == 0) in(arbol->getRaiz()); else if (orden == 1) pre(arbol->getRaiz()); else post(arbol->getRaiz());
    for (auto* n : nodos) {
        int r = tblPeliculasAdmin->rowCount();
        tblPeliculasAdmin->insertRow(r);
        tblPeliculasAdmin->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(n->pelicula.codigoOriginal)));
        tblPeliculasAdmin->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(n->pelicula.titulo)));
        tblPeliculasAdmin->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(n->pelicula.genero)));
        tblPeliculasAdmin->setItem(r, 3, new QTableWidgetItem(QString::number(n->pelicula.duracion)));
        tblPeliculasAdmin->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(n->pelicula.clasificacion)));
        tblPeliculasAdmin->setItem(r, 5, new QTableWidgetItem(QString::fromStdString(n->pelicula.idioma)));
        tblPeliculasAdmin->setItem(r, 6, new QTableWidgetItem(QString::fromStdString(n->pelicula.fecha_estreno)));
        tblPeliculasAdmin->setItem(r, 7, new QTableWidgetItem(QString::fromStdString(n->pelicula.fecha_fin)));
        tblPeliculasAdmin->setItem(r, 8, new QTableWidgetItem(estadoPelicula(n->pelicula)));
    }
}

void MainWindow::onVerCarteleraAdmin() { poblarCarteleraAdmin(); }

// =========================================================================
//  Funciones
// =========================================================================

void MainWindow::limpiarFormularioFuncion() {
    txtHorarioFuncion->clear();
    txtSalaFuncion->clear();
    spnFilasFuncion->setValue(10);
    spnColumnasFuncion->setValue(20);
}

void MainWindow::poblarComboPeliculasFuncion() {
    cmbPeliculaFuncion->clear();
    std::function<void(NodoArbol*)> in = [&](NodoArbol* n) {
        if (!n) return; in(n->izquierdo);
        cmbPeliculaFuncion->addItem(QString("%1 - %2")
            .arg(QString::fromStdString(n->pelicula.codigoOriginal),
                 QString::fromStdString(n->pelicula.titulo)),
            QString::fromStdString(n->pelicula.codigoOriginal));
        in(n->derecho);
    };
    in(arbol->getRaiz());
}

void MainWindow::poblarComboSedesFuncion() {
    cmbSedeFuncion->clear();
    for (int i = 0; i < grafo->cantidadSedes(); i++) {
        const Sede& s = grafo->getSede(i);
        cmbSedeFuncion->addItem(QString("%1 - %2").arg(QString::fromStdString(s.codigo),
                                                        QString::fromStdString(s.nombre)),
                                 QString::fromStdString(s.codigo));
    }
}

void MainWindow::onCrearFuncion() {
    if (cmbPeliculaFuncion->count() == 0) { lblMensajeFuncion->setText("No hay películas"); return; }
    if (cmbSedeFuncion->count() == 0)     { lblMensajeFuncion->setText("No hay sedes registradas"); return; }
    QString codPel  = cmbPeliculaFuncion->currentData().toString();
    QString codSede = cmbSedeFuncion->currentData().toString();
    QString horario = txtHorarioFuncion->text().trimmed();
    QString sala    = txtSalaFuncion->text().trimmed();
    int filas = spnFilasFuncion->value();
    int cols  = spnColumnasFuncion->value();
    if (!Val::esHorarioValido(horario.toStdString())) { lblMensajeFuncion->setText("Horario inválido"); return; }
    if (sala.isEmpty()) { lblMensajeFuncion->setText("Sala vacía"); return; }

    int cod; QString orig;
    if (!parsearCodigoPelicula(codPel, cod, orig)) return;
    Pelicula* pel = arbol->buscar(cod);
    if (pel == nullptr) return;

    std::string codFun = Persistencia::siguienteCodigoFuncion(avlFunciones);
    QString rutaAsientos = rutaDatos(QString::fromStdString(codFun) + "_funcion.json");
    Persistencia::crearArchivoAsientosVacio(rutaAsientos.toStdString(), filas, cols);

    Funcion f(codFun, pel->codigoOriginal, pel->titulo, horario.toStdString(),
              sala.toStdString(), filas, cols, rutaAsientos.toStdString(),
              codSede.toStdString());

    avlFunciones->insertar(f);
    pel->funciones.push_back(codFun);
    reconstruirGrafoDesdeFunciones();

    limpiarFormularioFuncion();
    lblMensajeFuncion->setText(QString("Función %1 creada").arg(QString::fromStdString(codFun)));
    poblarFuncionesAdmin();
    poblarSedesAdmin();
}

void MainWindow::onEditarFuncion() {
    bool ok;
    QString codFun = QInputDialog::getText(this, "Editar", "Código función:", QLineEdit::Normal, "", &ok);
    if (!ok || codFun.trimmed().isEmpty()) return;
    Funcion* f = avlFunciones->buscar(codFun.trimmed().toStdString());
    if (f == nullptr) return;

    QString nuevoHor = QInputDialog::getText(this, "Editar", "Nuevo horario:", QLineEdit::Normal,
                                              QString::fromStdString(f->horario), &ok);
    if (!ok || !Val::esHorarioValido(nuevoHor.toStdString())) return;
    QString nuevaSala = QInputDialog::getText(this, "Editar", "Nueva sala:", QLineEdit::Normal,
                                               QString::fromStdString(f->sala), &ok);
    if (!ok || nuevaSala.trimmed().isEmpty()) return;
    f->horario = nuevoHor.toStdString();
    f->sala    = nuevaSala.trimmed().toStdString();
    lblMensajeFuncion->setText("Actualizada");
    poblarFuncionesAdmin();
}

void MainWindow::onEliminarFuncion() {
    bool ok;
    QString codFun = QInputDialog::getText(this, "Eliminar", "Código función:", QLineEdit::Normal, "", &ok);
    if (!ok || codFun.trimmed().isEmpty()) return;
    Funcion* f = avlFunciones->buscar(codFun.trimmed().toStdString());
    if (f == nullptr) return;
    QString rutaArchivo = QString::fromStdString(f->archivoAsientos);
    if (avlFunciones->eliminar(codFun.trimmed().toStdString())) {
        QFile::remove(rutaArchivo);
        reconstruirGrafoDesdeFunciones();
        lblMensajeFuncion->setText("Eliminada");
        poblarFuncionesAdmin();
        poblarSedesAdmin();
    }
}

void MainWindow::onBuscarFuncionAdmin() {
    bool ok;
    QString codFun = QInputDialog::getText(this, "Buscar", "Código función:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Funcion* f = avlFunciones->buscar(codFun.trimmed().toStdString());
    if (f == nullptr) return;
    QMessageBox::information(this, "Función",
        QString("Código: %1\nPelícula: %2\nSede: %3\nHorario: %4\nSala: %5\nDim: %6x%7")
        .arg(QString::fromStdString(f->codigoFuncion))
        .arg(QString::fromStdString(f->tituloPelicula))
        .arg(QString::fromStdString(f->sede))
        .arg(QString::fromStdString(f->horario))
        .arg(QString::fromStdString(f->sala))
        .arg(f->filas).arg(f->columnas));
}

void MainWindow::onListarFunciones() { poblarFuncionesAdmin(); }

void MainWindow::poblarFuncionesAdmin() {
    tblFuncionesAdmin->setRowCount(0);
    std::vector<Funcion> lista;
    int orden = cmbOrdenFunciones ? cmbOrdenFunciones->currentIndex() : 0;
    if (orden == 0) lista = avlFunciones->inorden();
    else if (orden == 1) lista = avlFunciones->preorden();
    else lista = avlFunciones->postorden();
    for (auto& f : lista) {
        int r = tblFuncionesAdmin->rowCount();
        tblFuncionesAdmin->insertRow(r);
        tblFuncionesAdmin->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(f.codigoFuncion)));
        tblFuncionesAdmin->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(f.tituloPelicula)));
        tblFuncionesAdmin->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(f.sede)));
        tblFuncionesAdmin->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(f.horario)));
        tblFuncionesAdmin->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(f.sala)));
        tblFuncionesAdmin->setItem(r, 5, new QTableWidgetItem(QString::number(f.filas)));
        tblFuncionesAdmin->setItem(r, 6, new QTableWidgetItem(QString::number(f.columnas)));
        tblFuncionesAdmin->setItem(r, 7, new QTableWidgetItem(QString::fromStdString(f.archivoAsientos)));
    }
}

void MainWindow::onCargarFuncionEnMapa() {
    int fila = tblFuncionesAdmin->currentRow();
    if (fila < 0) return;
    QString codFun = tblFuncionesAdmin->item(fila, 0)->text();
    Funcion* f = avlFunciones->buscar(codFun.toStdString());
    if (f == nullptr) return;
    if (!matriz->getCodigoFuncion().empty()) {
        Persistencia::guardarAsientosFuncion(
            rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
            matriz);
    }
    matriz->vaciar();
    matriz->setCodigoFuncion(f->codigoFuncion);
    matriz->configurarFuncion(f->tituloPelicula, f->horario, f->sala);
    Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), matriz);
    lblFuncionActivaAdmin->setText(QString("Función activa: %1 - %2 - %3 (%4x%5)")
        .arg(QString::fromStdString(f->codigoFuncion))
        .arg(QString::fromStdString(f->tituloPelicula))
        .arg(QString::fromStdString(f->horario))
        .arg(f->filas).arg(f->columnas));
    poblarTablaAsientos(tblAsientosAdmin, matriz);
}

// =========================================================================
//  Asientos
// =========================================================================

void MainWindow::poblarTablaAsientos(QTableWidget* tabla, MatrizAsientos* m) {
    tabla->clear();
    if (m == nullptr || m->getFilas() == 0) { tabla->setRowCount(0); tabla->setColumnCount(0); return; }
    int filas = m->getFilas(), cols = m->getColumnas();
    tabla->setRowCount(filas);
    tabla->setColumnCount(cols + 1);
    QStringList headers;
    headers << "Fila";
    for (int c = 1; c <= cols; c++) headers << QString("C%1").arg(c);
    tabla->setHorizontalHeaderLabels(headers);
    for (int f = 1; f <= filas; f++) {
        tabla->setItem(f-1, 0, new QTableWidgetItem(QString::number(f)));
        for (int c = 1; c <= cols; c++) {
            NodoMatriz* a = m->buscarAsiento(f, c);
            QTableWidgetItem* item;
            if (a != nullptr) {
                item = new QTableWidgetItem(QString::fromStdString(a->codigoReserva));
                item->setBackground(Qt::red); item->setForeground(Qt::white);
            } else {
                item = new QTableWidgetItem("Libre");
                item->setBackground(Qt::green);
            }
            tabla->setItem(f-1, c, item);
        }
    }
    tabla->resizeColumnsToContents();
}

void MainWindow::onLiberarAsientoAdmin() {
    if (matriz->getFilas() == 0) return;
    bool ok;
    QString codRes = QInputDialog::getText(this, "Liberar", "Código reserva:", QLineEdit::Normal, "", &ok);
    if (!ok || codRes.trimmed().isEmpty()) return;
    Reserva* r = tablaReservas->buscar(codRes.trimmed().toStdString());
    if (r == nullptr) return;

    if (r->codigoFuncion != matriz->getCodigoFuncion()) {
        Persistencia::guardarAsientosFuncion(
            rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
            matriz);
        Funcion* f = avlFunciones->buscar(r->codigoFuncion);
        if (f == nullptr) return;
        matriz->vaciar();
        matriz->setCodigoFuncion(f->codigoFuncion);
        matriz->configurarFuncion(f->tituloPelicula, f->horario, f->sala);
        Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), matriz);
    }
    matriz->eliminarAsiento(r->fila, r->columna);
    Persistencia::guardarAsientosFuncion(
        rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
        matriz);
    tablaReservas->eliminar(r->codigoReserva);
    Cliente* cli = arbolClientes->buscar(r->idCliente);
    if (cli != nullptr) {
        auto& v = cli->codigosReserva;
        v.erase(std::remove(v.begin(), v.end(), r->codigoReserva), v.end());
    }
    poblarTablaAsientos(tblAsientosAdmin, matriz);
    poblarReservasAdmin();
}

void MainWindow::onReporteMatrizAdmin() {
    if (matriz->getFilas() == 0) return;
    GeneradorDot g;
    g.graficarMatriz(matriz, "reporte_matriz.png");
    mostrarReporte("reporte_matriz.png");
}

// =========================================================================
//  Clientes
// =========================================================================

void MainWindow::poblarClientesAdmin() {
    tblClientesAdmin->setRowCount(0);
    for (auto& c : arbolClientes->listar()) {
        int r = tblClientesAdmin->rowCount();
        tblClientesAdmin->insertRow(r);
        tblClientesAdmin->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(c.id)));
        tblClientesAdmin->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(c.nombre)));
        tblClientesAdmin->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(c.correo)));
        tblClientesAdmin->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(c.telefono)));
        tblClientesAdmin->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(c.tipo)));
        tblClientesAdmin->setItem(r, 5, new QTableWidgetItem(QString::number(c.codigosReserva.size())));
    }
}

void MainWindow::onBuscarClienteAdmin() {
    bool ok;
    QString id = QInputDialog::getText(this, "Buscar", "ID:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Cliente* c = arbolClientes->buscar(id.trimmed().toStdString());
    if (c == nullptr) { lblMensajeClienteAdmin->setText("No existe"); return; }
    lblMensajeClienteAdmin->setText(QString("Cliente: %1 - %2 - %3")
        .arg(QString::fromStdString(c->id), QString::fromStdString(c->nombre),
             QString::fromStdString(c->correo)));
}

void MainWindow::onEliminarClienteAdmin() {
    int fila = tblClientesAdmin->currentRow();
    if (fila < 0) return;
    QString id = tblClientesAdmin->item(fila, 0)->text();
    Cliente* c = arbolClientes->buscar(id.toStdString());
    if (c == nullptr) return;
    for (auto& codRes : c->codigosReserva) {
        Reserva* r = tablaReservas->buscar(codRes);
        if (r != nullptr) {
            Funcion* f = avlFunciones->buscar(r->codigoFuncion);
            if (f != nullptr) {
                MatrizAsientos temp;
                Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);
                temp.eliminarAsiento(r->fila, r->columna);
                Persistencia::guardarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);
            }
            tablaReservas->eliminar(codRes);
        }
    }
    arbolClientes->eliminar(id.toStdString());
    poblarClientesAdmin();
    poblarReservasAdmin();
}

void MainWindow::onVerReservasClienteAdmin() {
    bool ok;
    QString id = QInputDialog::getText(this, "Reservas", "ID:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Cliente* c = arbolClientes->buscar(id.trimmed().toStdString());
    if (c == nullptr) return;
    QString txt;
    for (auto& codRes : c->codigosReserva) {
        Reserva* r = tablaReservas->buscar(codRes);
        if (r == nullptr) continue;
        txt += QString("%1 | Función %2 | Fila %3 | Columna %4\n")
            .arg(QString::fromStdString(r->codigoReserva))
            .arg(QString::fromStdString(r->codigoFuncion))
            .arg(r->fila).arg(r->columna);
    }
    QMessageBox::information(this, "Reservas", txt.isEmpty() ? "Sin reservas" : txt);
}

// =========================================================================
//  Reservas
// =========================================================================

void MainWindow::poblarReservasAdmin() {
    tblReservasAdmin->setRowCount(0);
    for (auto& r : tablaReservas->listar()) {
        int row = tblReservasAdmin->rowCount();
        tblReservasAdmin->insertRow(row);
        tblReservasAdmin->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r.codigoReserva)));
        tblReservasAdmin->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r.codigoFuncion)));
        tblReservasAdmin->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(r.idCliente)));
        tblReservasAdmin->setItem(row, 3, new QTableWidgetItem(QString::number(r.fila)));
        tblReservasAdmin->setItem(row, 4, new QTableWidgetItem(QString::number(r.columna)));
        tblReservasAdmin->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(r.fechaReserva)));
    }
}

void MainWindow::onBuscarReservaAdmin() {
    bool ok;
    QString cod = QInputDialog::getText(this, "Buscar", "Código reserva:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Reserva* r = tablaReservas->buscar(cod.trimmed().toStdString());
    if (r == nullptr) return;
    lblMensajeReservasAdmin->setText(QString("Reserva: %1 | Función %2 | Cliente %3 | %4,%5")
        .arg(QString::fromStdString(r->codigoReserva),
             QString::fromStdString(r->codigoFuncion),
             QString::fromStdString(r->idCliente))
        .arg(r->fila).arg(r->columna));
}

void MainWindow::onEliminarReservaAdmin() {
    int fila = tblReservasAdmin->currentRow();
    if (fila < 0) return;
    QString cod = tblReservasAdmin->item(fila, 0)->text();
    Reserva* r = tablaReservas->buscar(cod.toStdString());
    if (r == nullptr) return;
    Funcion* f = avlFunciones->buscar(r->codigoFuncion);
    if (f != nullptr) {
        MatrizAsientos temp;
        Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);
        temp.eliminarAsiento(r->fila, r->columna);
        Persistencia::guardarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);
        if (matriz->getCodigoFuncion() == f->codigoFuncion) {
            matriz->eliminarAsiento(r->fila, r->columna);
            poblarTablaAsientos(tblAsientosAdmin, matriz);
        }
    }
    Cliente* c = arbolClientes->buscar(r->idCliente);
    if (c != nullptr) {
        auto& v = c->codigosReserva;
        v.erase(std::remove(v.begin(), v.end(), r->codigoReserva), v.end());
    }
    tablaReservas->eliminar(cod.toStdString());
    poblarReservasAdmin();
}

// =========================================================================
//  Carga masiva
// =========================================================================

void MainWindow::onCargarPeliculasFunciones() {
    QString ruta = QFileDialog::getOpenFileName(this, "JSON de películas y sedes",
                                                CARPETA_DATOS, "JSON (*.json)");
    if (ruta.isEmpty()) return;
    QFile file(ruta);
    if (!file.open(QIODevice::ReadOnly)) {
        txtLogCarga->append("No se pudo abrir " + ruta); return;
    }
    QByteArray contenido = file.readAll();
    file.close();
    std::vector<std::string> errores;
    bool ok = Persistencia::cargarPeliculasYFuncionesDesdeTexto(
        contenido.toStdString(), arbol, avlFunciones, grafo, errores);
    txtLogCarga->append(QString("=== Carga: %1 ===").arg(ok ? "OK" : "FALLÓ"));
    for (auto& e : errores) txtLogCarga->append(QString::fromStdString(e));
    reconstruirGrafoDesdeFunciones();
    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
    poblarComboSedesFuncion();
    poblarFuncionesAdmin();
    poblarSedesAdmin();
    poblarCombosGrafo();
}

void MainWindow::onCargarClientesReservas() {
    QString ruta = QFileDialog::getOpenFileName(this, "JSON clientes",
                                                CARPETA_DATOS, "JSON (*.json)");
    if (ruta.isEmpty()) return;
    QFile file(ruta);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray contenido = file.readAll();
    file.close();
    std::vector<std::string> errores;
    bool ok = Persistencia::cargarClientesYReservasDesdeTexto(
        contenido.toStdString(), arbolClientes, tablaReservas, avlFunciones, errores);
    txtLogCarga->append(QString("=== Clientes: %1 ===").arg(ok ? "OK" : "FALLÓ"));
    for (auto& e : errores) txtLogCarga->append(QString::fromStdString(e));
    poblarClientesAdmin();
    poblarReservasAdmin();
}

void MainWindow::onGuardarTodoManual() {
    guardarTodosLosDatos();
    QMessageBox::information(this, "Guardado", "Datos guardados (JSON + .edd)");
}

// =========================================================================
//  Reportes
// =========================================================================

void MainWindow::onReporteBST()  { GeneradorDot g; g.graficarArbol(arbol, "reporte_bst.png"); mostrarReporte("reporte_bst.png"); }
void MainWindow::onReporteAVL()  { GeneradorDot g; g.graficarAVL(avlFunciones, "reporte_avl.png"); mostrarReporte("reporte_avl.png"); }
void MainWindow::onReporteArbolB(){ GeneradorDot g; g.graficarArbolB(arbolClientes, "reporte_b.png"); mostrarReporte("reporte_b.png"); }
void MainWindow::onReporteHash() { GeneradorDot g; g.graficarHash(tablaReservas, "reporte_hash.png"); mostrarReporte("reporte_hash.png"); }

void MainWindow::onReporteGrafoSedes() {
    GeneradorDot g; g.graficarGrafoSedes(grafo, "reporte_grafo.png");
    mostrarReporte("reporte_grafo.png");
}
void MainWindow::onReporteListaAdyacencia() {
    GeneradorDot g; g.graficarListaAdyacencia(grafo, "reporte_ady.png");
    mostrarReporte("reporte_ady.png");
}
void MainWindow::onReporteBlockchain() {
    GeneradorDot g; g.graficarBlockchain(blockchain, "reporte_bc.png");
    mostrarReporte("reporte_bc.png");
}
void MainWindow::onReporteMerkle() {
    int idx = blockchain->ultimoIndiceValido();
    if (idx < 0) { QMessageBox::information(this, "Reporte", "Sin bloques válidos"); return; }
    GeneradorDot g; g.graficarMerkle(blockchain, idx, "reporte_merkle.png");
    mostrarReporte("reporte_merkle.png");
}

void MainWindow::mostrarReporte(const QString& nombrePng) {
    QString ruta = QDir::current().filePath("reportes_generados/" + nombrePng);
    QFileInfo fi(ruta);
    if (!fi.exists()) { QMessageBox::warning(this, "Reporte", "No se generó el PNG"); return; }
    QPixmap pix(ruta);
    if (pix.isNull()) return;
    lblReporteAdmin->setText("");
    lblReporteAdmin->setPixmap(pix.scaled(lblReporteAdmin->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// =========================================================================
//  Cliente
// =========================================================================

void MainWindow::onEditarPerfil() {
    if (clienteActual == nullptr) return;
    PerfilDialog dlg(clienteActual, this);
    if (dlg.exec() == QDialog::Accepted) {
        lblBienvenidoCliente->setText(QString("Bienvenido, %1 (%2)")
            .arg(QString::fromStdString(clienteActual->nombre))
            .arg(QString::fromStdString(clienteActual->id)));
    }
}

void MainWindow::poblarCarteleraCliente() {
    tblCarteleraCliente->setRowCount(0);
    std::function<void(NodoArbol*)> in = [&](NodoArbol* n) {
        if (!n) return; in(n->izquierdo);
        QString estado = estadoPelicula(n->pelicula);
        if (estado == "En cartelera" || estado == "Próximo a retirar" || estado == "Próximo estreno") {
            int r = tblCarteleraCliente->rowCount();
            tblCarteleraCliente->insertRow(r);
            tblCarteleraCliente->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(n->pelicula.codigoOriginal)));
            tblCarteleraCliente->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(n->pelicula.titulo)));
            tblCarteleraCliente->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(n->pelicula.genero)));
            tblCarteleraCliente->setItem(r, 3, new QTableWidgetItem(estado));
        }
        in(n->derecho);
    };
    in(arbol->getRaiz());

    cmbPeliculaCliente->clear();
    for (int r = 0; r < tblCarteleraCliente->rowCount(); r++) {
        cmbPeliculaCliente->addItem(tblCarteleraCliente->item(r,0)->text() + " - " +
                                     tblCarteleraCliente->item(r,1)->text(),
                                     tblCarteleraCliente->item(r,0)->text());
    }
    onPeliculaClienteCambiada(cmbPeliculaCliente->currentIndex());
}

void MainWindow::onPeliculaClienteCambiada(int) {
    cmbFuncionCliente->clear();
    if (cmbPeliculaCliente->count() == 0) return;
    QString codPel = cmbPeliculaCliente->currentData().toString();
    int cod; QString orig;
    if (!parsearCodigoPelicula(codPel, cod, orig)) return;
    Pelicula* p = arbol->buscar(cod);
    if (p == nullptr) return;
    for (auto& codFun : p->funciones) {
        Funcion* f = avlFunciones->buscar(codFun);
        if (f == nullptr) continue;
        cmbFuncionCliente->addItem(QString("%1 - %2 - %3 - %4")
            .arg(QString::fromStdString(f->codigoFuncion),
                 QString::fromStdString(f->horario),
                 QString::fromStdString(f->sala),
                 QString::fromStdString(f->sede)),
            QString::fromStdString(f->codigoFuncion));
    }
    onFuncionClienteCambiada(cmbFuncionCliente->currentIndex());
}

void MainWindow::onFuncionClienteCambiada(int) {
    if (cmbFuncionCliente->count() == 0) {
        lblFuncionClienteActiva->setText("Función activa: (ninguna)");
        tblMapaAsientosCliente->setRowCount(0);
        tblMapaAsientosCliente->setColumnCount(0);
        return;
    }
    QString codFun = cmbFuncionCliente->currentData().toString();
    Funcion* f = avlFunciones->buscar(codFun.toStdString());
    if (f == nullptr) return;
    if (!matriz->getCodigoFuncion().empty() && matriz->getCodigoFuncion() != f->codigoFuncion) {
        Persistencia::guardarAsientosFuncion(
            rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
            matriz);
    }
    matriz->vaciar();
    matriz->setCodigoFuncion(f->codigoFuncion);
    matriz->configurarFuncion(f->tituloPelicula, f->horario, f->sala);
    Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), matriz);
    lblFuncionClienteActiva->setText(QString("Función activa: %1 - %2 - %3")
        .arg(QString::fromStdString(f->codigoFuncion),
             QString::fromStdString(f->horario),
             QString::fromStdString(f->sala)));
    poblarTablaAsientos(tblMapaAsientosCliente, matriz);
}

void MainWindow::onReservarAsientoCliente() {
    if (clienteActual == nullptr) return;
    if (matriz->getFilas() == 0) { lblMensajeCliente->setText("Selecciona una función"); return; }
    int f = tblMapaAsientosCliente->currentRow();
    int c = tblMapaAsientosCliente->currentColumn();
    if (f < 0 || c < 1) { lblMensajeCliente->setText("Selecciona un asiento"); return; }
    int fila = f + 1, columna = c;
    NodoMatriz* a = matriz->buscarAsiento(fila, columna);
    if (a != nullptr) { lblMensajeCliente->setText("Asiento ocupado"); return; }

    std::string codRes = Persistencia::siguienteCodigoReserva(tablaReservas);
    QString fechaHoy = QDate::currentDate().toString("yyyy-MM-dd");

    Reserva r(codRes, matriz->getCodigoFuncion(), clienteActual->id,
              fila, columna, fechaHoy.toStdString());
    tablaReservas->insertar(r);
    clienteActual->codigosReserva.push_back(codRes);
    matriz->insertarAsiento(fila, columna, codRes);
    Persistencia::guardarAsientosFuncion(
        rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
        matriz);

    // Fase 3: transacción + método de pago
    bool ok;
    QString metodo = QInputDialog::getItem(this, "Pago", "Método de pago:",
                                            {"Tarjeta", "Efectivo"}, 0, false, &ok);
    if (!ok) metodo = "Efectivo";
    double monto = 45.0;

    static int consecutivoTx = 1;
    QString codTx = QString("T%1").arg(consecutivoTx++, 3, 10, QChar('0'));

    JsonValue tx;
    tx["codigo_transaccion"] = codTx.toStdString();
    tx["codigo_reserva"]     = codRes;
    tx["id_cliente"]         = clienteActual->id;
    tx["codigo_funcion"]     = matriz->getCodigoFuncion();
    tx["fila"]               = fila;
    tx["columna"]            = columna;
    tx["monto"]              = monto;
    tx["metodo_pago"]        = metodo.toStdString();
    tx["fecha_pago"]         = fechaHoy.toStdString();
    transaccionesPendientes.push_back(tx);

    if (transaccionesPendientes.size() >= 4) {
        try {
            blockchain->minar(transaccionesPendientes);
            transaccionesPendientes.clear();
        } catch (...) {}
    }

    poblarTablaAsientos(tblMapaAsientosCliente, matriz);
    poblarBlockchainAdmin();
    lblMensajeCliente->setText(QString("Reserva %1 creada (transacción %2 encolada)")
        .arg(QString::fromStdString(codRes)).arg(codTx));
}

void MainWindow::poblarMisReservas() {
    tblMisReservas->setRowCount(0);
    if (clienteActual == nullptr) return;
    for (auto& cod : clienteActual->codigosReserva) {
        Reserva* r = tablaReservas->buscar(cod);
        if (r == nullptr) continue;
        int row = tblMisReservas->rowCount();
        tblMisReservas->insertRow(row);
        tblMisReservas->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r->codigoReserva)));
        tblMisReservas->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r->codigoFuncion)));
        tblMisReservas->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(r->fechaReserva)));
        tblMisReservas->setItem(row, 3, new QTableWidgetItem(QString::number(r->fila)));
        tblMisReservas->setItem(row, 4, new QTableWidgetItem(QString::number(r->columna)));
        tblMisReservas->setItem(row, 5, new QTableWidgetItem("VÁLIDA"));
    }
}

void MainWindow::onActualizarMisReservas() { poblarMisReservas(); }

void MainWindow::onCancelarMiReserva() {
    if (clienteActual == nullptr) return;
    int row = tblMisReservas->currentRow();
    if (row < 0) return;
    QString codRes = tblMisReservas->item(row, 0)->text();
    Reserva* r = tablaReservas->buscar(codRes.toStdString());
    if (r == nullptr) return;
    Funcion* f = avlFunciones->buscar(r->codigoFuncion);
    if (f == nullptr) return;
    MatrizAsientos temp;
    Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);
    temp.eliminarAsiento(r->fila, r->columna);
    Persistencia::guardarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);
    if (matriz->getCodigoFuncion() == f->codigoFuncion) {
        matriz->eliminarAsiento(r->fila, r->columna);
        poblarTablaAsientos(tblMapaAsientosCliente, matriz);
    }
    tablaReservas->eliminar(codRes.toStdString());
    auto& v = clienteActual->codigosReserva;
    v.erase(std::remove(v.begin(), v.end(), codRes.toStdString()), v.end());
    poblarMisReservas();
    QMessageBox::information(this, "Cancelada", "Reserva cancelada");
}

// =========================================================================
//  Promociones y solicitudes
// =========================================================================

void MainWindow::onConsultarPromociones() {
    if (listaPromociones->getPrimero() == nullptr) {
        txtPromocionesCliente->setText("No hay promociones.");
        return;
    }
    txtPromocionesCliente->setText("(sin promociones cargadas en esta versión)");
}

void MainWindow::onNuevaSolicitud() {
    if (clienteActual == nullptr) return;
    bool ok;
    QString tipo = QInputDialog::getText(this, "Solicitud", "Tipo:", QLineEdit::Normal, "", &ok);
    if (!ok || tipo.trimmed().isEmpty()) return;
    QString prio = QInputDialog::getItem(this, "Solicitud", "Prioridad:",
                                         {"Baja","Normal","Alta"}, 1, false, &ok);
    if (!ok) return;
    QString desc = QInputDialog::getText(this, "Solicitud", "Descripción:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Solicitud s(clienteActual->nombre, tipo.toStdString());
    s.numero = siguienteNumeroSolicitud++;
    s.prioridad = prio.toStdString();
    s.telefono = clienteActual->telefono;
    s.descripcion = desc.toStdString();
    s.fecha = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    listaSolicitudes->insertar(s);
    QMessageBox::information(this, "Solicitud", "Registrada");
}

void MainWindow::onConsultarSolicitudes() {
    txtSolicitudesCliente->clear();
    if (clienteActual == nullptr) return;
    NodoListaDoble* actual = listaSolicitudes->getPrimero();
    if (actual == nullptr) { txtSolicitudesCliente->setText("Sin solicitudes"); return; }
    QString txt;
    do {
        if (actual->solicitud.telefono == clienteActual->telefono) {
            txt += QString("#%1 %2\nEstado: %3\n%4\n\n")
                .arg(actual->solicitud.numero)
                .arg(QString::fromStdString(actual->solicitud.tipo))
                .arg(QString::fromStdString(actual->solicitud.estado))
                .arg(QString::fromStdString(actual->solicitud.descripcion));
        }
        actual = actual->siguiente;
    } while (actual != listaSolicitudes->getPrimero());
    txtSolicitudesCliente->setText(txt.isEmpty() ? "Sin solicitudes" : txt);
}

// =========================================================================
//  Fase 3: Grafo
// =========================================================================

void MainWindow::poblarSedesAdmin() {
    tblSedes->setRowCount(0);
    for (int i = 0; i < grafo->cantidadSedes(); i++) {
        const Sede& s = grafo->getSede(i);
        int r = tblSedes->rowCount();
        tblSedes->insertRow(r);
        tblSedes->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(s.codigo)));
        tblSedes->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(s.nombre)));
        tblSedes->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(s.direccion)));
        tblSedes->setItem(r, 3, new QTableWidgetItem(QString::number(s.cantidadSalas)));
        tblSedes->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(s.telefono)));
    }
}

void MainWindow::poblarCombosGrafo() {
    cmbOrigenBFS->clear(); cmbDestinoBFS->clear(); cmbSedeVerPeliculas->clear();
    for (int i = 0; i < grafo->cantidadSedes(); i++) {
        const Sede& s = grafo->getSede(i);
        QString lbl = QString("%1 - %2").arg(QString::fromStdString(s.codigo),
                                              QString::fromStdString(s.nombre));
        QString cod = QString::fromStdString(s.codigo);
        cmbOrigenBFS->addItem(lbl, cod);
        cmbDestinoBFS->addItem(lbl, cod);
        cmbSedeVerPeliculas->addItem(lbl, cod);
        cmbSedeSimilar->addItem(lbl, cod);
    }
}

void MainWindow::onRegistrarSede() {
    QString cod = txtCodSede->text().trimmed().toLower();
    if (cod.isEmpty()) { lblResultGrafo->setText("Código vacío"); return; }
    if (grafo->existeSede(cod.toStdString())) { lblResultGrafo->setText("Ya existe"); return; }
    Sede s(cod.toStdString(),
           txtNombreSede->text().trimmed().toStdString(),
           txtDirSede->text().trimmed().toStdString(),
           spnSalasSede->value(),
           txtTelSede->text().trimmed().toStdString());
    grafo->agregarSede(s);
    lblResultGrafo->setText("Sede registrada");
    poblarSedesAdmin();
    poblarCombosGrafo();
    poblarComboSedesFuncion();
}

void MainWindow::onEjecutarBFS() {
    if (cmbOrigenBFS->count() == 0) return;
    QString o = cmbOrigenBFS->currentData().toString();
    QString d = cmbDestinoBFS->currentData().toString();
    auto camino = grafo->caminoBFS(o.toStdString(), d.toStdString());
    if (camino.empty()) { lblResultGrafo->setText("No hay camino entre esas sedes"); return; }
    QString txt = "Camino: ";
    for (size_t i = 0; i < camino.size(); i++) {
        if (i) txt += " -> ";
        txt += QString::fromStdString(camino[i]);
    }
    lblResultGrafo->setText(txt);
}

void MainWindow::onVerRanking() {
    auto r = grafo->rankingAristas();
    QString txt = "Ranking de aristas (por peso):\n";
    for (auto& x : r) {
        txt += QString("%1 -- %2 : %3 películas\n")
            .arg(QString::fromStdString(x.sedeA),
                 QString::fromStdString(x.sedeB))
            .arg(x.peso);
    }
    if (r.empty()) txt = "No hay aristas registradas";
    lblResultGrafo->setText(txt);
}

void MainWindow::onVerPeliculasSede() {
    tblPeliculasSede->setRowCount(0);
    if (cmbSedeVerPeliculas->count() == 0) return;
    QString codSede = cmbSedeVerPeliculas->currentData().toString();
    for (const auto& f : avlFunciones->inorden()) {
        if (f.sede != codSede.toStdString()) continue;
        int r = tblPeliculasSede->rowCount();
        tblPeliculasSede->insertRow(r);
        tblPeliculasSede->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(f.tituloPelicula)));
        tblPeliculasSede->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(f.codigoFuncion)));
        tblPeliculasSede->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(f.horario)));
        tblPeliculasSede->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(f.sala)));
    }
}

// =========================================================================
//  Fase 3: Blockchain
// =========================================================================

void MainWindow::poblarBlockchainAdmin() {
    tblBlockchain->setRowCount(0);
    const auto& bloques = blockchain->getBloques();
    const auto& integ = blockchain->getIntegridad();
    for (size_t i = 0; i < bloques.size(); i++) {
        int r = tblBlockchain->rowCount();
        tblBlockchain->insertRow(r);
        tblBlockchain->setItem(r, 0, new QTableWidgetItem(QString::number(bloques[i].index)));
        tblBlockchain->setItem(r, 1, new QTableWidgetItem(QString::number(bloques[i].nonce)));
        tblBlockchain->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(bloques[i].hash)));
        tblBlockchain->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(bloques[i].previousHash)));
        QTableWidgetItem* it = new QTableWidgetItem(integ[i] ? "ÍNTEGRO" : "ALTERADO");
        it->setBackground(integ[i] ? Qt::green : Qt::red);
        it->setForeground(Qt::white);
        tblBlockchain->setItem(r, 4, it);
    }
    spnIndiceMerkle->setMaximum(std::max(0, (int)bloques.size() - 1));
}

void MainWindow::onMinarBloque() {
    if (transaccionesPendientes.empty()) {
        QMessageBox::information(this, "Minar", "No hay transacciones pendientes");
        return;
    }
    try {
        Bloque b = blockchain->minar(transaccionesPendientes);
        transaccionesPendientes.clear();
        QMessageBox::information(this, "Minar",
            QString("Bloque %1 minado\nHash: %2\nNonce: %3")
            .arg(b.index).arg(QString::fromStdString(b.hash)).arg((qint64)b.nonce));
        poblarBlockchainAdmin();
    } catch (std::exception& e) {
        QMessageBox::warning(this, "Minar", e.what());
    }
}

void MainWindow::onVerCadena() {
    blockchain->cargarDesdeDisco();
    poblarBlockchainAdmin();
}

void MainWindow::onVerMerkle() {
    int idx = spnIndiceMerkle->value();
    if (idx < 0 || idx >= (int)blockchain->getBloques().size()) {
        QMessageBox::warning(this, "Merkle", "Índice inválido"); return;
    }
    GeneradorDot g;
    g.graficarMerkle(blockchain, idx, "reporte_merkle.png");
    QString ruta = QDir::current().filePath("reportes_generados/reporte_merkle.png");
    if (!QFileInfo::exists(ruta)) return;
    QPixmap pix(ruta);
    lblMerkle->setText("");
    lblMerkle->setPixmap(pix.scaled(lblMerkle->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::onVerVentasTotales() {
    auto txs = blockchain->todasTransaccionesValidas();
    int boletos = (int)txs.size();
    double total = 0.0;
    for (const auto& tx : txs) total += tx.getDecimal("monto", 0.0);
    lblVentas->setText(QString("Boletos vendidos: %1\nTotal recaudado: Q%2")
        .arg(boletos).arg(total, 0, 'f', 2));
}

// =========================================================================
//  Fase 3: Cliente
// =========================================================================

void MainWindow::poblarCombosClienteF3() {
    cmbPeliculaF3->clear();
    std::function<void(NodoArbol*)> in = [&](NodoArbol* n) {
        if (!n) return; in(n->izquierdo);
        cmbPeliculaF3->addItem(QString("%1 - %2")
            .arg(QString::fromStdString(n->pelicula.codigoOriginal),
                 QString::fromStdString(n->pelicula.titulo)),
            QString::fromStdString(n->pelicula.codigoOriginal));
        in(n->derecho);
    };
    in(arbol->getRaiz());
    // los combos de sede ya se llenaron en poblarCombosGrafo
}

void MainWindow::onSedesPorPelicula() {
    tblSedesPorPelicula->setRowCount(0);
    if (cmbPeliculaF3->count() == 0) return;
    QString codPel = cmbPeliculaF3->currentData().toString();
    for (const auto& f : avlFunciones->inorden()) {
        if (f.codigoPelicula != codPel.toStdString()) continue;
        int r = tblSedesPorPelicula->rowCount();
        tblSedesPorPelicula->insertRow(r);
        tblSedesPorPelicula->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(f.sede)));
        tblSedesPorPelicula->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(f.codigoFuncion)));
        tblSedesPorPelicula->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(f.horario)));
        tblSedesPorPelicula->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(f.sala)));
    }
}

void MainWindow::onVerSimilares() {
    tblSimilares->setRowCount(0);
    if (cmbSedeSimilar->count() == 0) return;
    QString codSede = cmbSedeSimilar->currentData().toString();
    auto r = grafo->sedesSimilares(codSede.toStdString());
    for (auto& x : r) {
        int row = tblSimilares->rowCount();
        tblSimilares->insertRow(row);
        tblSimilares->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(x.sedeB)));
        tblSimilares->setItem(row, 1, new QTableWidgetItem(QString::number(x.peso)));
    }
}

void MainWindow::onMiHistorial() {
    tblHistorial->setRowCount(0);
    if (clienteActual == nullptr) return;
    auto txs = blockchain->todasTransaccionesValidas();
    int idx = 0;
    for (const auto& tx : txs) {
        idx++;
        if (tx.getTexto("id_cliente", "") != clienteActual->id) continue;
        int row = tblHistorial->rowCount();
        tblHistorial->insertRow(row);
        tblHistorial->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(tx.getTexto("codigo_transaccion", ""))));
        tblHistorial->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(tx.getTexto("codigo_reserva", ""))));
        tblHistorial->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(tx.getTexto("codigo_funcion", ""))));
        tblHistorial->setItem(row, 3, new QTableWidgetItem(QString::number(tx.getDecimal("monto", 0.0))));
        // buscar bloque
        int blk = -1;
        for (size_t b = 0; b < blockchain->getBloques().size(); b++) {
            for (const auto& t : blockchain->getBloques()[b].data) {
                if (t.getTexto("codigo_transaccion", "") == tx.getTexto("codigo_transaccion", "")) {
                    blk = (int)b; break;
                }
            }
            if (blk >= 0) break;
        }
        tblHistorial->setItem(row, 4, new QTableWidgetItem(QString::number(blk)));
    }
}

void MainWindow::onVerComprobante() {
    QString codTx = txtCodTransaccion->text().trimmed();
    if (codTx.isEmpty()) return;
    auto txs = blockchain->todasTransaccionesValidas();
    for (const auto& tx : txs) {
        if (tx.getTexto("codigo_transaccion", "") != codTx.toStdString()) continue;
        int blk = -1;
        std::string hashBlq;
        for (size_t b = 0; b < blockchain->getBloques().size(); b++) {
            for (const auto& t : blockchain->getBloques()[b].data) {
                if (t.getTexto("codigo_transaccion", "") == codTx.toStdString()) {
                    blk = (int)b; hashBlq = blockchain->getBloques()[b].hash; break;
                }
            }
            if (blk >= 0) break;
        }
        QMessageBox::information(this, "Comprobante",
            QString("Transacción: %1\nReserva: %2\nFunción: %3\nAsiento: F%4 C%5\n"
                    "Monto: Q%6\nMétodo: %7\nFecha: %8\n\nBloque #%9\nHash: %10")
            .arg(QString::fromStdString(tx.getTexto("codigo_transaccion","")))
            .arg(QString::fromStdString(tx.getTexto("codigo_reserva","")))
            .arg(QString::fromStdString(tx.getTexto("codigo_funcion","")))
            .arg(tx.getEntero("fila",0)).arg(tx.getEntero("columna",0))
            .arg(tx.getDecimal("monto",0.0))
            .arg(QString::fromStdString(tx.getTexto("metodo_pago","")))
            .arg(QString::fromStdString(tx.getTexto("fecha_pago","")))
            .arg(blk).arg(QString::fromStdString(hashBlq)));
        return;
    }
    QMessageBox::warning(this, "Comprobante", "Transacción no encontrada");
}