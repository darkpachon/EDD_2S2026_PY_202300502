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
#include <QPushButton>
#include <QDebug>

namespace {
const QString CARPETA_DATOS = "datos";

void asegurarCarpetaDatos() {
    QDir d;
    if (!d.exists(CARPETA_DATOS)) d.mkpath(CARPETA_DATOS);
}

QString rutaDatos(const QString& nombre) {
    return CARPETA_DATOS + "/" + nombre;
}

// "P034" -> 34
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

    clienteActual      = nullptr;
    siguienteNumeroSolicitud = 1;

    asegurarCarpetaDatos();
    construirUI();
    cargarDatosIniciales();

    setWindowTitle("CinemaUSAC - Fase 2");
    resize(1200, 800);
}

MainWindow::~MainWindow() {
    delete arbol;
    delete avlFunciones;
    delete arbolClientes;
    delete tablaReservas;
    delete matriz;
    delete listaSolicitudes;
    delete listaPromociones;
}

// =========================================================================
//  Construcción de UI
// =========================================================================

void MainWindow::construirUI() {
    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    stack->addWidget(construirPaginaLogin());   // índice 0
    stack->addWidget(construirPaginaAdmin());   // índice 1
    stack->addWidget(construirPaginaCliente()); // índice 2
    stack->setCurrentIndex(0);
}

// ---------- LOGIN ----------
QWidget* MainWindow::construirPaginaLogin() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    auto* titulo = new QLabel("CinemaUSAC - Iniciar Sesión");
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size:22px; font-weight:bold;");
    layout->addWidget(titulo);

    txtUsuario  = new QLineEdit();
    txtUsuario->setPlaceholderText("Usuario / Correo");
    txtPassword = new QLineEdit();
    txtPassword->setPlaceholderText("Contraseña");
    txtPassword->setEchoMode(QLineEdit::Password);

    layout->addWidget(txtUsuario);
    layout->addWidget(txtPassword);

    auto* botones = new QHBoxLayout();
    btnIngresar   = new QPushButton("Ingresar");
    btnRegistrarse = new QPushButton("Registrarse (Cliente)");
    botones->addWidget(btnIngresar);
    botones->addWidget(btnRegistrarse);
    layout->addLayout(botones);

    lblMensajeLogin = new QLabel();
    lblMensajeLogin->setStyleSheet("color:red; font-weight:bold;");
    lblMensajeLogin->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblMensajeLogin);

    connect(btnIngresar,    &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(btnRegistrarse, &QPushButton::clicked, this, &MainWindow::onRegistrarseClicked);
    connect(txtPassword, &QLineEdit::returnPressed, this, &MainWindow::onLoginClicked);

    return page;
}

// ---------- ADMIN ----------
QWidget* MainWindow::construirPaginaAdmin() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);

    // Barra superior
    auto* top = new QHBoxLayout();
    auto* titulo = new QLabel("Panel de Administrador");
    titulo->setStyleSheet("font-size:18px; font-weight:bold;");
    auto* btnGuardar = new QPushButton("Guardar cambios (JSON)");
    auto* btnCerrar  = new QPushButton("Cerrar sesión");
    top->addWidget(titulo);
    top->addStretch();
    top->addWidget(btnGuardar);
    top->addWidget(btnCerrar);
    layout->addLayout(top);

    connect(btnGuardar, &QPushButton::clicked, this, &MainWindow::onGuardarTodoManual);
    connect(btnCerrar,  &QPushButton::clicked, this, &MainWindow::onCerrarSesionAdmin);

    auto* tabs = new QTabWidget();

    // ======== TAB 1: PELÍCULAS ========
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* form = new QFormLayout();
        txtCodigo        = new QLineEdit(); txtCodigo->setPlaceholderText("Ej: P001");
        txtTitulo        = new QLineEdit();
        txtGenero        = new QLineEdit();
        txtDuracion      = new QLineEdit(); txtDuracion->setPlaceholderText("min");
        txtClasificacion = new QLineEdit();
        txtIdioma        = new QLineEdit();
        txtFechaEstreno  = new QLineEdit(); txtFechaEstreno->setPlaceholderText("AAAA-MM-DD");
        txtFechaFin      = new QLineEdit(); txtFechaFin->setPlaceholderText("AAAA-MM-DD");
        form->addRow("Código:",        txtCodigo);
        form->addRow("Título:",        txtTitulo);
        form->addRow("Género:",        txtGenero);
        form->addRow("Duración (min):",txtDuracion);
        form->addRow("Clasificación:", txtClasificacion);
        form->addRow("Idioma:",        txtIdioma);
        form->addRow("Fecha estreno:", txtFechaEstreno);
        form->addRow("Fecha fin:",     txtFechaFin);
        lay->addLayout(form);

        auto* bts = new QHBoxLayout();
        auto* bIns = new QPushButton("Insertar");
        auto* bEdit= new QPushButton("Editar");
        auto* bElim= new QPushButton("Eliminar");
        auto* bBus = new QPushButton("Buscar por código");
        auto* bVer = new QPushButton("Ver cartelera");
        cmbOrdenPeliculas = new QComboBox();
        cmbOrdenPeliculas->addItems({"Inorden", "Preorden", "Postorden"});
        bts->addWidget(bIns); bts->addWidget(bEdit); bts->addWidget(bElim);
        bts->addWidget(bBus); bts->addWidget(bVer);
        bts->addWidget(new QLabel("Orden:"));
        bts->addWidget(cmbOrdenPeliculas);
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

    // ======== TAB 2: FUNCIONES ========
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* form = new QFormLayout();
        cmbPeliculaFuncion   = new QComboBox();
        txtHorarioFuncion    = new QLineEdit(); txtHorarioFuncion->setPlaceholderText("HH:MM");
        txtSalaFuncion       = new QLineEdit(); txtSalaFuncion->setPlaceholderText("Sala 1");
        spnFilasFuncion      = new QSpinBox(); spnFilasFuncion->setRange(1, 200); spnFilasFuncion->setValue(10);
        spnColumnasFuncion   = new QSpinBox(); spnColumnasFuncion->setRange(1, 200); spnColumnasFuncion->setValue(20);
        form->addRow("Película:",      cmbPeliculaFuncion);
        form->addRow("Horario:",       txtHorarioFuncion);
        form->addRow("Sala:",          txtSalaFuncion);
        form->addRow("Filas:",         spnFilasFuncion);
        form->addRow("Columnas:",      spnColumnasFuncion);
        lay->addLayout(form);

        auto* bts = new QHBoxLayout();
        auto* bCrear = new QPushButton("Crear función");
        auto* bEdit  = new QPushButton("Editar función");
        auto* bElim  = new QPushButton("Eliminar función");
        auto* bBus   = new QPushButton("Buscar por código");
        auto* bList  = new QPushButton("Listar todas");
        auto* bMapa  = new QPushButton("Cargar en mapa de asientos");
        cmbOrdenFunciones = new QComboBox();
        cmbOrdenFunciones->addItems({"Inorden","Preorden","Postorden"});
        bts->addWidget(bCrear); bts->addWidget(bEdit); bts->addWidget(bElim);
        bts->addWidget(bBus);   bts->addWidget(bList); bts->addWidget(bMapa);
        bts->addWidget(new QLabel("Orden:"));
        bts->addWidget(cmbOrdenFunciones);
        lay->addLayout(bts);

        tblFuncionesAdmin = new QTableWidget();
        tblFuncionesAdmin->setColumnCount(7);
        tblFuncionesAdmin->setHorizontalHeaderLabels(
            {"Código","Película","Horario","Sala","Filas","Columnas","Archivo"});
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

    // ======== TAB 3: ASIENTOS ========
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
        auto* bLib  = new QPushButton("Liberar asiento (ingresar código reserva)");
        auto* bRep  = new QPushButton("Generar reporte matriz");
        bts->addWidget(bLib);
        bts->addWidget(bRep);
        lay->addLayout(bts);

        connect(bLib, &QPushButton::clicked, this, &MainWindow::onLiberarAsientoAdmin);
        connect(bRep, &QPushButton::clicked, this, &MainWindow::onReporteMatrizAdmin);

        tabs->addTab(tab, "3. Asientos");
    }

    // ======== TAB 4: CLIENTES ========
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
        auto* bBus = new QPushButton("Buscar por ID");
        auto* bEli = new QPushButton("Eliminar cliente");
        auto* bVer = new QPushButton("Ver reservas del cliente");
        auto* bRep = new QPushButton("Reporte Árbol B");
        bts->addWidget(bBus); bts->addWidget(bEli);
        bts->addWidget(bVer); bts->addWidget(bRep);
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

    // ======== TAB 5: RESERVAS ========
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
        auto* bBus = new QPushButton("Buscar por código");
        auto* bEli = new QPushButton("Eliminar reserva");
        auto* bRep = new QPushButton("Reporte Tabla Hash");
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

    // ======== TAB 6: CARGA MASIVA ========
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* bPel = new QPushButton("Cargar Películas + Funciones (JSON)");
        auto* bCli = new QPushButton("Cargar Clientes + Reservas (JSON)");
        lay->addWidget(bPel);
        lay->addWidget(bCli);

        txtLogCarga = new QTextEdit();
        txtLogCarga->setReadOnly(true);
        lay->addWidget(txtLogCarga);

        connect(bPel, &QPushButton::clicked, this, &MainWindow::onCargarPeliculasFunciones);
        connect(bCli, &QPushButton::clicked, this, &MainWindow::onCargarClientesReservas);

        tabs->addTab(tab, "6. Carga masiva");
    }

    // ======== TAB 7: REPORTES ========
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* bts = new QHBoxLayout();
        auto* bBST = new QPushButton("BST Películas");
        auto* bAVL = new QPushButton("AVL Funciones");
        auto* bB   = new QPushButton("Árbol B Clientes");
        auto* bH   = new QPushButton("Tabla Hash Reservas");
        auto* bM   = new QPushButton("Matriz activa");
        bts->addWidget(bBST); bts->addWidget(bAVL); bts->addWidget(bB);
        bts->addWidget(bH);   bts->addWidget(bM);
        lay->addLayout(bts);

        lblReporteAdmin = new QLabel("El reporte aparecerá aquí");
        lblReporteAdmin->setMinimumHeight(400);
        lblReporteAdmin->setAlignment(Qt::AlignCenter);
        lblReporteAdmin->setFrameShape(QFrame::Box);
        lblReporteAdmin->setScaledContents(false);
        lay->addWidget(lblReporteAdmin);

        connect(bBST, &QPushButton::clicked, this, &MainWindow::onReporteBST);
        connect(bAVL, &QPushButton::clicked, this, &MainWindow::onReporteAVL);
        connect(bB,   &QPushButton::clicked, this, &MainWindow::onReporteArbolB);
        connect(bH,   &QPushButton::clicked, this, &MainWindow::onReporteHash);
        connect(bM,   &QPushButton::clicked, this, &MainWindow::onReporteMatrizAdmin);

        tabs->addTab(tab, "7. Reportes");
    }

    layout->addWidget(tabs);
    return page;
}

// ---------- CLIENTE ----------
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

    // ======== TAB 1: CARTELERA Y RESERVA ========
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        tblCarteleraCliente = new QTableWidget();
        tblCarteleraCliente->setColumnCount(4);
        tblCarteleraCliente->setHorizontalHeaderLabels({"Código","Título","Género","Estado"});
        tblCarteleraCliente->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tblCarteleraCliente->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblCarteleraCliente->setMaximumHeight(180);
        lay->addWidget(new QLabel("Cartelera disponible:"));
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
        lay->addWidget(new QLabel("Mapa de asientos (seleccione uno libre):"));
        lay->addWidget(tblMapaAsientosCliente);

        auto* bRes = new QPushButton("Reservar asiento seleccionado");
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

    // ======== TAB 2: MIS RESERVAS ========
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
        auto* bCan = new QPushButton("Cancelar reserva seleccionada");
        bts->addWidget(bAct); bts->addWidget(bCan);
        lay->addLayout(bts);

        connect(bAct, &QPushButton::clicked, this, &MainWindow::onActualizarMisReservas);
        connect(bCan, &QPushButton::clicked, this, &MainWindow::onCancelarMiReserva);

        tabs->addTab(tab, "2. Mis Reservas");
    }

    // ======== TAB 3: PROMOCIONES ========
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* bVer = new QPushButton("Ver promociones activas");
        txtPromocionesCliente = new QTextEdit();
        txtPromocionesCliente->setReadOnly(true);
        lay->addWidget(bVer);
        lay->addWidget(txtPromocionesCliente);

        connect(bVer, &QPushButton::clicked, this, &MainWindow::onConsultarPromociones);

        tabs->addTab(tab, "3. Promociones");
    }

    // ======== TAB 4: SOLICITUDES ========
    {
        auto* tab = new QWidget();
        auto* lay = new QVBoxLayout(tab);

        auto* bts = new QHBoxLayout();
        auto* bNew = new QPushButton("Nueva solicitud");
        auto* bVer = new QPushButton("Consultar mis solicitudes");
        bts->addWidget(bNew); bts->addWidget(bVer);
        lay->addLayout(bts);

        txtSolicitudesCliente = new QTextEdit();
        txtSolicitudesCliente->setReadOnly(true);
        lay->addWidget(txtSolicitudesCliente);

        connect(bNew, &QPushButton::clicked, this, &MainWindow::onNuevaSolicitud);
        connect(bVer, &QPushButton::clicked, this, &MainWindow::onConsultarSolicitudes);

        tabs->addTab(tab, "4. Solicitudes");
    }

    layout->addWidget(tabs);
    return page;
}

// =========================================================================
//  Carga / guardado
// =========================================================================

void MainWindow::cargarDatosIniciales() {
    // Cargar cartelera si existe
    QString rutaCart = rutaDatos("cartelera.json");
    if (QFileInfo::exists(rutaCart)) {
        std::vector<std::string> errores;
        Persistencia::cargarPeliculasYFunciones(rutaCart.toStdString(), arbol, avlFunciones, errores);
    }

    // Cargar clientes
    QString rutaCli = rutaDatos("clientes.json");
    if (QFileInfo::exists(rutaCli)) {
        std::vector<std::string> errores;
        Persistencia::cargarClientesYReservas(rutaCli.toStdString(), arbolClientes,
                                              tablaReservas, avlFunciones, errores);
    }

    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
    poblarFuncionesAdmin();
    poblarClientesAdmin();
    poblarReservasAdmin();
    poblarCarteleraCliente();
}

void MainWindow::guardarTodosLosDatos() {
    asegurarCarpetaDatos();
    Persistencia::guardarCartelera(rutaDatos("cartelera.json").toStdString(),
                                   arbol, avlFunciones);
    Persistencia::guardarClientes(rutaDatos("clientes.json").toStdString(),
                                  arbolClientes);
    Persistencia::guardarReservas(rutaDatos("reservas.json").toStdString(),
                                  tablaReservas);

    // Matriz activa
    if (!matriz->getCodigoFuncion().empty()) {
        Persistencia::guardarAsientosFuncion(
            rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json")
                .toStdString(),
            matriz);
    }
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

    // Admin: dos credenciales válidas
    if ((u == "admin" && p == "admin123") ||
        (u == "AdminCine@gmail.com" && p == "admin123Pass")) {
        stack->setCurrentIndex(1);
        txtUsuario->clear(); txtPassword->clear();
        lblMensajeLogin->clear();
        poblarCarteleraAdmin();
        poblarComboPeliculasFuncion();
        poblarFuncionesAdmin();
        poblarClientesAdmin();
        poblarReservasAdmin();
        return;
    }

    // Cliente: buscar por correo
    Cliente* c = arbolClientes->buscar(u.toStdString());
    if (c == nullptr) {
        // Buscar por correo
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
        return;
    }

    lblMensajeLogin->setText("Credenciales inválidas");
}

void MainWindow::onRegistrarseClicked() {
    bool ok;
    QString id = QInputDialog::getText(this, "Registro", "ID de cliente (ej. u010):",
                                       QLineEdit::Normal, "", &ok);
    if (!ok || id.trimmed().isEmpty()) return;
    QString nombre = QInputDialog::getText(this, "Registro", "Nombre:", QLineEdit::Normal, "", &ok);
    if (!ok || nombre.trimmed().isEmpty()) return;
    QString correo = QInputDialog::getText(this, "Registro", "Correo:", QLineEdit::Normal, "", &ok);
    if (!ok || correo.trimmed().isEmpty()) return;
    QString tel = QInputDialog::getText(this, "Registro", "Teléfono:", QLineEdit::Normal, "", &ok);
    if (!ok || tel.trimmed().isEmpty()) return;
    QString pass = QInputDialog::getText(this, "Registro", "Contraseña:", QLineEdit::Password, "", &ok);
    if (!ok || pass.isEmpty()) return;

    // Validar
    if (!Val::esCorreoValido(correo.toStdString())) {
        QMessageBox::warning(this, "Registro", "Correo inválido.");
        return;
    }
    if (!Val::esTelefonoValido(tel.toStdString())) {
        QMessageBox::warning(this, "Registro", "Teléfono inválido.");
        return;
    }
    if (arbolClientes->buscar(id.trimmed().toStdString()) != nullptr) {
        QMessageBox::warning(this, "Registro", "Ya existe un cliente con ese ID.");
        return;
    }
    for (auto& c : arbolClientes->listar()) {
        if (c.correo == correo.toStdString()) {
            QMessageBox::warning(this, "Registro", "El correo ya está en uso.");
            return;
        }
    }

    Cliente nuevo(id.trimmed().toStdString(), nombre.toStdString(),
                  correo.toStdString(), tel.toStdString(),
                  pass.toStdString(), "cliente");
    arbolClientes->insertar(nuevo);
    QMessageBox::information(this, "Registro", "Cliente registrado. Los datos se guardarán al cerrar o presionar Guardar.");
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
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) {
        lblMensajeAdmin->setText("Código inválido (usa Pxxx).");
        return;
    }
    if (arbol->buscar(cod) != nullptr) {
        lblMensajeAdmin->setText("Ese código ya existe.");
        return;
    }
    QString titulo = txtTitulo->text().trimmed();
    QString genero = txtGenero->text().trimmed();
    QString durStr = txtDuracion->text().trimmed();
    QString clasif = txtClasificacion->text().trimmed();
    QString idioma = txtIdioma->text().trimmed();
    QString fEst   = txtFechaEstreno->text().trimmed();
    QString fFin   = txtFechaFin->text().trimmed();

    if (titulo.isEmpty() || genero.isEmpty() || clasif.isEmpty() ||
        idioma.isEmpty() || durStr.isEmpty()) {
        lblMensajeAdmin->setText("Complete todos los campos.");
        return;
    }
    if (!Val::esEnteroPositivo(durStr.toStdString())) {
        lblMensajeAdmin->setText("Duración inválida.");
        return;
    }
    if (!Val::esFechaValida(fEst.toStdString()) || !Val::esFechaValida(fFin.toStdString())) {
        lblMensajeAdmin->setText("Fechas inválidas (AAAA-MM-DD).");
        return;
    }

    Pelicula p(cod, orig.toStdString(), titulo.toStdString(), genero.toStdString(),
               durStr.toInt(), clasif.toStdString(), idioma.toStdString(),
               fEst.toStdString(), fFin.toStdString());
    arbol->insertar(p);

    limpiarFormularioPelicula();
    lblMensajeAdmin->setText("Película insertada.");
    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
}

void MainWindow::onEditarPelicula() {
    int cod; QString orig;
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) {
        lblMensajeAdmin->setText("Código inválido.");
        return;
    }
    Pelicula* p = arbol->buscar(cod);
    if (p == nullptr) { lblMensajeAdmin->setText("No encontrada."); return; }

    // Actualizar sólo los campos que no estén vacíos
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

    lblMensajeAdmin->setText("Película actualizada.");
    poblarCarteleraAdmin();
}

void MainWindow::onEliminarPelicula() {
    int cod; QString orig;
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) {
        lblMensajeAdmin->setText("Código inválido.");
        return;
    }
    if (arbol->buscar(cod) == nullptr) {
        lblMensajeAdmin->setText("No encontrada.");
        return;
    }
    arbol->eliminar(cod);
    limpiarFormularioPelicula();
    lblMensajeAdmin->setText("Película eliminada (las funciones asociadas no se eliminan automáticamente).");
    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
}

void MainWindow::onBuscarPeliculaAdmin() {
    int cod; QString orig;
    if (!parsearCodigoPelicula(txtCodigo->text(), cod, orig)) {
        lblMensajeAdmin->setText("Código inválido.");
        return;
    }
    Pelicula* p = arbol->buscar(cod);
    if (p == nullptr) { lblMensajeAdmin->setText("No encontrada."); return; }
    txtTitulo->setText(QString::fromStdString(p->titulo));
    txtGenero->setText(QString::fromStdString(p->genero));
    txtDuracion->setText(QString::number(p->duracion));
    txtClasificacion->setText(QString::fromStdString(p->clasificacion));
    txtIdioma->setText(QString::fromStdString(p->idioma));
    txtFechaEstreno->setText(QString::fromStdString(p->fecha_estreno));
    txtFechaFin->setText(QString::fromStdString(p->fecha_fin));
    lblMensajeAdmin->setText("Película cargada.");
}

void MainWindow::poblarCarteleraAdmin() {
    tblPeliculasAdmin->setRowCount(0);
    int orden = cmbOrdenPeliculas ? cmbOrdenPeliculas->currentIndex() : 0;
    std::vector<NodoArbol*> nodos;

    std::function<void(NodoArbol*)> pre = [&](NodoArbol* n) {
        if (!n) return; nodos.push_back(n); pre(n->izquierdo); pre(n->derecho);
    };
    std::function<void(NodoArbol*)> in = [&](NodoArbol* n) {
        if (!n) return; in(n->izquierdo); nodos.push_back(n); in(n->derecho);
    };
    std::function<void(NodoArbol*)> post = [&](NodoArbol* n) {
        if (!n) return; post(n->izquierdo); post(n->derecho); nodos.push_back(n);
    };
    if (orden == 0)      in(arbol->getRaiz());
    else if (orden == 1) pre(arbol->getRaiz());
    else                 post(arbol->getRaiz());

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

void MainWindow::onVerCarteleraAdmin() {
    poblarCarteleraAdmin();
    lblMensajeAdmin->setText("Cartelera actualizada.");
}

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
            .arg(QString::fromStdString(n->pelicula.codigoOriginal))
            .arg(QString::fromStdString(n->pelicula.titulo)),
            QString::fromStdString(n->pelicula.codigoOriginal));
        in(n->derecho);
    };
    in(arbol->getRaiz());
}

void MainWindow::onCrearFuncion() {
    if (cmbPeliculaFuncion->count() == 0) {
        lblMensajeFuncion->setText("No hay películas registradas.");
        return;
    }
    QString codPel = cmbPeliculaFuncion->currentData().toString();
    QString horario = txtHorarioFuncion->text().trimmed();
    QString sala    = txtSalaFuncion->text().trimmed();
    int filas  = spnFilasFuncion->value();
    int cols   = spnColumnasFuncion->value();

    if (!Val::esHorarioValido(horario.toStdString())) {
        lblMensajeFuncion->setText("Horario inválido (HH:MM).");
        return;
    }
    if (sala.isEmpty()) { lblMensajeFuncion->setText("Sala vacía."); return; }
    if (filas <= 0 || cols <= 0) { lblMensajeFuncion->setText("Dimensiones inválidas."); return; }

    // Buscar película
    int cod; QString orig;
    if (!parsearCodigoPelicula(codPel, cod, orig)) {
        lblMensajeFuncion->setText("Película inválida.");
        return;
    }
    Pelicula* pel = arbol->buscar(cod);
    if (pel == nullptr) { lblMensajeFuncion->setText("Película no encontrada."); return; }

    // Generar código Fxxx
    std::string codFun = Persistencia::siguienteCodigoFuncion(avlFunciones);

    // Crear archivo de asientos vacío
    QString rutaAsientos = rutaDatos(QString::fromStdString(codFun) + "_funcion.json");
    Persistencia::crearArchivoAsientosVacio(rutaAsientos.toStdString(), filas, cols);

    Funcion f(codFun, pel->codigoOriginal, pel->titulo, horario.toStdString(),
              sala.toStdString(), filas, cols, rutaAsientos.toStdString());

    avlFunciones->insertar(f);
    pel->funciones.push_back(codFun);

    limpiarFormularioFuncion();
    lblMensajeFuncion->setText(QString("Función %1 creada.").arg(QString::fromStdString(codFun)));
    poblarFuncionesAdmin();
}

void MainWindow::onEditarFuncion() {
    bool ok;
    QString codFun = QInputDialog::getText(this, "Editar función",
                                           "Código de función (Fxxx):",
                                           QLineEdit::Normal, "", &ok);
    if (!ok || codFun.trimmed().isEmpty()) return;
    Funcion* f = avlFunciones->buscar(codFun.trimmed().toStdString());
    if (f == nullptr) { lblMensajeFuncion->setText("No existe esa función."); return; }

    QString nuevoHor = QInputDialog::getText(this, "Editar función", "Nuevo horario:",
                                             QLineEdit::Normal,
                                             QString::fromStdString(f->horario), &ok);
    if (!ok) return;
    if (!Val::esHorarioValido(nuevoHor.toStdString())) {
        lblMensajeFuncion->setText("Horario inválido.");
        return;
    }
    QString nuevaSala = QInputDialog::getText(this, "Editar función", "Nueva sala:",
                                              QLineEdit::Normal,
                                              QString::fromStdString(f->sala), &ok);
    if (!ok) return;
    if (nuevaSala.trimmed().isEmpty()) return;

    f->horario = nuevoHor.toStdString();
    f->sala    = nuevaSala.trimmed().toStdString();
    lblMensajeFuncion->setText("Función actualizada.");
    poblarFuncionesAdmin();
}

void MainWindow::onEliminarFuncion() {
    bool ok;
    QString codFun = QInputDialog::getText(this, "Eliminar función",
                                           "Código de función:", QLineEdit::Normal, "", &ok);
    if (!ok || codFun.trimmed().isEmpty()) return;
    Funcion* f = avlFunciones->buscar(codFun.trimmed().toStdString());
    if (f == nullptr) { lblMensajeFuncion->setText("No existe."); return; }

    QString rutaArchivo = QString::fromStdString(f->archivoAsientos);
    if (avlFunciones->eliminar(codFun.trimmed().toStdString())) {
        QFile::remove(rutaArchivo); // borrar archivo .json
        lblMensajeFuncion->setText("Función eliminada.");
        poblarFuncionesAdmin();
        poblarComboPeliculasFuncion();
    }
}

void MainWindow::onBuscarFuncionAdmin() {
    bool ok;
    QString codFun = QInputDialog::getText(this, "Buscar función", "Código de función:",
                                           QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Funcion* f = avlFunciones->buscar(codFun.trimmed().toStdString());
    if (f == nullptr) { lblMensajeFuncion->setText("No existe."); return; }
    QMessageBox::information(this, "Función",
        QString("Código: %1\nPelícula: %2\nHorario: %3\nSala: %4\nDimensiones: %5x%6\nArchivo: %7")
        .arg(QString::fromStdString(f->codigoFuncion))
        .arg(QString::fromStdString(f->tituloPelicula))
        .arg(QString::fromStdString(f->horario))
        .arg(QString::fromStdString(f->sala))
        .arg(f->filas).arg(f->columnas)
        .arg(QString::fromStdString(f->archivoAsientos)));
}

void MainWindow::onListarFunciones() { poblarFuncionesAdmin(); }

void MainWindow::poblarFuncionesAdmin() {
    tblFuncionesAdmin->setRowCount(0);
    std::vector<Funcion> lista;
    int orden = cmbOrdenFunciones ? cmbOrdenFunciones->currentIndex() : 0;
    if (orden == 0)      lista = avlFunciones->inorden();
    else if (orden == 1) lista = avlFunciones->preorden();
    else                 lista = avlFunciones->postorden();

    for (auto& f : lista) {
        int r = tblFuncionesAdmin->rowCount();
        tblFuncionesAdmin->insertRow(r);
        tblFuncionesAdmin->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(f.codigoFuncion)));
        tblFuncionesAdmin->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(f.tituloPelicula)));
        tblFuncionesAdmin->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(f.horario)));
        tblFuncionesAdmin->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(f.sala)));
        tblFuncionesAdmin->setItem(r, 4, new QTableWidgetItem(QString::number(f.filas)));
        tblFuncionesAdmin->setItem(r, 5, new QTableWidgetItem(QString::number(f.columnas)));
        tblFuncionesAdmin->setItem(r, 6, new QTableWidgetItem(QString::fromStdString(f.archivoAsientos)));
    }
}

void MainWindow::onCargarFuncionEnMapa() {
    int fila = tblFuncionesAdmin->currentRow();
    if (fila < 0) { lblMensajeFuncion->setText("Selecciona una función."); return; }
    QString codFun = tblFuncionesAdmin->item(fila, 0)->text();
    Funcion* f = avlFunciones->buscar(codFun.toStdString());
    if (f == nullptr) return;

    // Guardar la matriz anterior si había
    if (!matriz->getCodigoFuncion().empty()) {
        Persistencia::guardarAsientosFuncion(
            rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
            matriz);
    }

    matriz->vaciar();
    matriz->setCodigoFuncion(f->codigoFuncion);
    matriz->configurarFuncion(f->tituloPelicula, f->horario, f->sala);
    Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), matriz);

    lblFuncionActivaAdmin->setText(
        QString("Función activa: %1 - %2 - %3 (%4x%5)")
        .arg(QString::fromStdString(f->codigoFuncion))
        .arg(QString::fromStdString(f->tituloPelicula))
        .arg(QString::fromStdString(f->horario))
        .arg(f->filas).arg(f->columnas));

    poblarTablaAsientos(tblAsientosAdmin, matriz);
    lblMensajeFuncion->setText("Función cargada en mapa.");
}

// =========================================================================
//  Asientos (admin)
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
                item->setBackground(Qt::red);
                item->setForeground(Qt::white);
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
    if (matriz->getFilas() == 0) {
        QMessageBox::information(this, "Asientos", "Carga una función primero.");
        return;
    }
    bool ok;
    QString codRes = QInputDialog::getText(this, "Liberar asiento",
                                           "Código de reserva:", QLineEdit::Normal, "", &ok);
    if (!ok || codRes.trimmed().isEmpty()) return;

    Reserva* r = tablaReservas->buscar(codRes.trimmed().toStdString());
    if (r == nullptr) { QMessageBox::warning(this, "Reserva", "No existe esa reserva."); return; }

    // Si la reserva es de otra función, cargarla
    if (r->codigoFuncion != matriz->getCodigoFuncion()) {
        // Guardar la actual
        Persistencia::guardarAsientosFuncion(
            rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
            matriz);
        // Cargar la otra
        Funcion* f = avlFunciones->buscar(r->codigoFuncion);
        if (f == nullptr) return;
        matriz->vaciar();
        matriz->setCodigoFuncion(f->codigoFuncion);
        matriz->configurarFuncion(f->tituloPelicula, f->horario, f->sala);
        Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), matriz);
    }

    // Liberar asiento en matriz
    matriz->eliminarAsiento(r->fila, r->columna);
    // Persistir asientos
    Persistencia::guardarAsientosFuncion(
        rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
        matriz);
    // Quitar de hash
    tablaReservas->eliminar(r->codigoReserva);

    // Quitar de cliente
    Cliente* cli = arbolClientes->buscar(r->idCliente);
    if (cli != nullptr) {
        auto& v = cli->codigosReserva;
        v.erase(std::remove(v.begin(), v.end(), r->codigoReserva), v.end());
    }

    poblarTablaAsientos(tblAsientosAdmin, matriz);
    poblarReservasAdmin();
    QMessageBox::information(this, "Asiento liberado", "Reserva eliminada en cascada.");
}

void MainWindow::onReporteMatrizAdmin() {
    if (matriz->getFilas() == 0) {
        QMessageBox::information(this, "Reporte", "Carga una función primero.");
        return;
    }
    GeneradorDot g;
    g.graficarMatriz(matriz, "reporte_matriz.png");
    mostrarReporte("reporte_matriz.png");
}

// =========================================================================
//  Clientes (admin)
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
    QString id = QInputDialog::getText(this, "Buscar cliente", "ID:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Cliente* c = arbolClientes->buscar(id.trimmed().toStdString());
    if (c == nullptr) { lblMensajeClienteAdmin->setText("No existe."); return; }
    lblMensajeClienteAdmin->setText(QString("Cliente: %1 - %2 - %3")
        .arg(QString::fromStdString(c->id))
        .arg(QString::fromStdString(c->nombre))
        .arg(QString::fromStdString(c->correo)));
    // Resaltar fila
    for (int r = 0; r < tblClientesAdmin->rowCount(); r++) {
        if (tblClientesAdmin->item(r,0)->text() == QString::fromStdString(c->id)) {
            tblClientesAdmin->selectRow(r);
            break;
        }
    }
}

void MainWindow::onEliminarClienteAdmin() {
    int fila = tblClientesAdmin->currentRow();
    if (fila < 0) { lblMensajeClienteAdmin->setText("Selecciona un cliente."); return; }
    QString id = tblClientesAdmin->item(fila, 0)->text();
    Cliente* c = arbolClientes->buscar(id.toStdString());
    if (c == nullptr) return;

    // Eliminar reservas en cascada
    for (auto& codRes : c->codigosReserva) {
        Reserva* r = tablaReservas->buscar(codRes);
        if (r != nullptr) {
            // Liberar el asiento en el archivo de la función
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
    lblMensajeClienteAdmin->setText("Cliente eliminado en cascada.");
}

void MainWindow::onVerReservasClienteAdmin() {
    bool ok;
    QString id = QInputDialog::getText(this, "Reservas por cliente", "ID del cliente:",
                                       QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Cliente* c = arbolClientes->buscar(id.trimmed().toStdString());
    if (c == nullptr) { QMessageBox::warning(this, "Reservas", "Cliente no existe."); return; }

    QString txt;
    for (auto& codRes : c->codigosReserva) {
        Reserva* r = tablaReservas->buscar(codRes);
        if (r == nullptr) continue;
        txt += QString("%1 | Función %2 | Fila %3 | Columna %4 | %5\n")
            .arg(QString::fromStdString(r->codigoReserva))
            .arg(QString::fromStdString(r->codigoFuncion))
            .arg(r->fila).arg(r->columna)
            .arg(QString::fromStdString(r->fechaReserva));
    }
    QMessageBox::information(this, "Reservas del cliente",
                             txt.isEmpty() ? "Sin reservas." : txt);
}

// =========================================================================
//  Reservas (admin)
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
    QString cod = QInputDialog::getText(this, "Buscar reserva", "Código:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    Reserva* r = tablaReservas->buscar(cod.trimmed().toStdString());
    if (r == nullptr) { lblMensajeReservasAdmin->setText("No existe."); return; }
    lblMensajeReservasAdmin->setText(QString("Reserva: %1 | Función %2 | Cliente %3 | %4,%5")
        .arg(QString::fromStdString(r->codigoReserva))
        .arg(QString::fromStdString(r->codigoFuncion))
        .arg(QString::fromStdString(r->idCliente))
        .arg(r->fila).arg(r->columna));
}

void MainWindow::onEliminarReservaAdmin() {
    int fila = tblReservasAdmin->currentRow();
    if (fila < 0) return;
    QString cod = tblReservasAdmin->item(fila, 0)->text();
    Reserva* r = tablaReservas->buscar(cod.toStdString());
    if (r == nullptr) return;

    // Liberar asiento
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
    // Quitar de cliente
    Cliente* c = arbolClientes->buscar(r->idCliente);
    if (c != nullptr) {
        auto& v = c->codigosReserva;
        v.erase(std::remove(v.begin(), v.end(), r->codigoReserva), v.end());
    }
    tablaReservas->eliminar(cod.toStdString());
    poblarReservasAdmin();
    lblMensajeReservasAdmin->setText("Reserva eliminada.");
}

// =========================================================================
//  Carga masiva
// =========================================================================

void MainWindow::onCargarPeliculasFunciones() {
    QString ruta = QFileDialog::getOpenFileName(this, "Archivo JSON de películas y funciones",
                                                CARPETA_DATOS, "JSON (*.json)");
    if (ruta.isEmpty()) return;
    std::vector<std::string> errores;
    bool ok = Persistencia::cargarPeliculasYFunciones(ruta.toStdString(), arbol, avlFunciones, errores);
    txtLogCarga->append(QString("=== Carga películas/funciones: %1 ===").arg(ok ? "OK" : "FALLÓ"));
    for (auto& e : errores) txtLogCarga->append(QString::fromStdString(e));
    poblarCarteleraAdmin();
    poblarComboPeliculasFuncion();
    poblarFuncionesAdmin();
}

void MainWindow::onCargarClientesReservas() {
    QString ruta = QFileDialog::getOpenFileName(this, "Archivo JSON de clientes y reservas",
                                                CARPETA_DATOS, "JSON (*.json)");
    if (ruta.isEmpty()) return;
    std::vector<std::string> errores;
    bool ok = Persistencia::cargarClientesYReservas(ruta.toStdString(), arbolClientes,
                                                    tablaReservas, avlFunciones, errores);
    txtLogCarga->append(QString("=== Carga clientes/reservas: %1 ===").arg(ok ? "OK" : "FALLÓ"));
    for (auto& e : errores) txtLogCarga->append(QString::fromStdString(e));
    poblarClientesAdmin();
    poblarReservasAdmin();
}

void MainWindow::onGuardarTodoManual() {
    guardarTodosLosDatos();
    QMessageBox::information(this, "Guardado", "Datos guardados en la carpeta 'datos'.");
}

// =========================================================================
//  Reportes
// =========================================================================

void MainWindow::onReporteBST() {
    GeneradorDot g;
    g.graficarArbol(arbol, "reporte_bst.png");
    mostrarReporte("reporte_bst.png");
}
void MainWindow::onReporteAVL() {
    GeneradorDot g;
    g.graficarAVL(avlFunciones, "reporte_avl.png");
    mostrarReporte("reporte_avl.png");
}
void MainWindow::onReporteArbolB() {
    GeneradorDot g;
    g.graficarArbolB(arbolClientes, "reporte_b.png");
    mostrarReporte("reporte_b.png");
}
void MainWindow::onReporteHash() {
    GeneradorDot g;
    g.graficarHash(tablaReservas, "reporte_hash.png");
    mostrarReporte("reporte_hash.png");
}

void MainWindow::mostrarReporte(const QString& nombrePng) {
    QString ruta = QDir::current().filePath("reportes_generados/" + nombrePng);
    QFileInfo fi(ruta);
    if (!fi.exists()) {
        QMessageBox::warning(this, "Reporte", "No se generó el PNG. ¿Graphviz está en el PATH?");
        return;
    }
    QPixmap pix(ruta);
    if (pix.isNull()) return;
    lblReporteAdmin->setText("");
    lblReporteAdmin->setPixmap(pix.scaled(lblReporteAdmin->size(),
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation));
    lblReporteAdmin->setWindowTitle("Reporte: " + nombrePng);
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
        QMessageBox::information(this, "Perfil", "Datos actualizados (se guardarán al cerrar).");
    }
}

void MainWindow::poblarCarteleraCliente() {
    tblCarteleraCliente->setRowCount(0);
    std::function<void(NodoArbol*)> in = [&](NodoArbol* n) {
        if (!n) return; in(n->izquierdo);
        QString estado = estadoPelicula(n->pelicula);
        if (estado == "En cartelera" || estado == "Próximo a retirar" ||
            estado == "Próximo estreno") {
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

    // Poblar combo de películas
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
        cmbFuncionCliente->addItem(QString("%1 - %2 - %3").arg(
            QString::fromStdString(f->codigoFuncion),
            QString::fromStdString(f->horario),
            QString::fromStdString(f->sala)),
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

    // Guardar la anterior si era otra
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
    if (matriz->getFilas() == 0) {
        lblMensajeCliente->setText("Selecciona una función primero.");
        return;
    }
    int f = tblMapaAsientosCliente->currentRow();
    int c = tblMapaAsientosCliente->currentColumn();
    if (f < 0 || c < 1) {
        lblMensajeCliente->setText("Selecciona un asiento en la tabla.");
        return;
    }
    int fila = f + 1;
    int columna = c; // col 0 es "Fila", asientos desde col 1
    NodoMatriz* a = matriz->buscarAsiento(fila, columna);
    if (a != nullptr) {
        lblMensajeCliente->setText("Ese asiento ya está ocupado.");
        return;
    }

    // Generar codigo de reserva
    std::string codRes = Persistencia::siguienteCodigoReserva(tablaReservas);

    // Crear reserva
    Reserva r(codRes, matriz->getCodigoFuncion(), clienteActual->id,
              fila, columna, QDate::currentDate().toString("yyyy-MM-dd").toStdString());
    tablaReservas->insertar(r);
    clienteActual->codigosReserva.push_back(codRes);

    // Insertar en matriz y persistir
    matriz->insertarAsiento(fila, columna, codRes);
    Persistencia::guardarAsientosFuncion(
        rutaDatos(QString::fromStdString(matriz->getCodigoFuncion()) + "_funcion.json").toStdString(),
        matriz);

    poblarTablaAsientos(tblMapaAsientosCliente, matriz);
    lblMensajeCliente->setText(QString("Reserva creada: %1").arg(QString::fromStdString(codRes)));
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
        // Estado: VENCIDA si hoy > fecha_reserva + (asumimos ya pasó)
        // Simplificación: comparamos contra hoy
        QString estado = "VÁLIDA";
        // Sin la fecha de la función es complejo; aquí sólo marcamos
        // vencida si fecha_reserva < hoy - 1 día (aprox.)
        QDate fRes = QDate::fromString(QString::fromStdString(r->fechaReserva), "yyyy-MM-dd");
        if (fRes.isValid() && fRes.daysTo(QDate::currentDate()) > 30) estado = "VENCIDA";
        tblMisReservas->setItem(row, 5, new QTableWidgetItem(estado));
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

    // Cargar función de la reserva
    Funcion* f = avlFunciones->buscar(r->codigoFuncion);
    if (f == nullptr) return;

    MatrizAsientos temp;
    Persistencia::cargarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);
    temp.eliminarAsiento(r->fila, r->columna);
    Persistencia::guardarAsientosFuncion(rutaDatos(QString::fromStdString(f->archivoAsientos)).toStdString(), &temp);

    // Si es la activa, actualizar la matriz también
    if (matriz->getCodigoFuncion() == f->codigoFuncion) {
        matriz->eliminarAsiento(r->fila, r->columna);
        poblarTablaAsientos(tblMapaAsientosCliente, matriz);
    }

    // Quitar de hash y cliente
    tablaReservas->eliminar(codRes.toStdString());
    auto& v = clienteActual->codigosReserva;
    v.erase(std::remove(v.begin(), v.end(), codRes.toStdString()), v.end());

    poblarMisReservas();
    QMessageBox::information(this, "Cancelada", "Reserva cancelada.");
}

// =========================================================================
//  Promociones y solicitudes (mantengo tu lógica, adaptada)
// =========================================================================

void MainWindow::onConsultarPromociones() {
    if (listaPromociones->getPrimero() == nullptr) {
        txtPromocionesCliente->setText("No hay promociones registradas.");
        return;
    }
    QDate hoy = QDate::currentDate();
    QStringList dias = {"lunes","martes","miércoles","jueves","viernes","sábado","domingo"};
    QString diaActual = dias.value(hoy.dayOfWeek() - 1);

    QString txt;
    NodoListaListas* actual = listaPromociones->getPrimero();
    do {
        QDate ini = QDate::fromString(QString::fromStdString(actual->promocion.fechaInicio), "yyyy-MM-dd");
        QDate fin = QDate::fromString(QString::fromStdString(actual->promocion.fechaFin), "yyyy-MM-dd");
        QString dias2 = QString::fromStdString(actual->promocion.diasAplicables).toLower();
        bool dentro = ini.isValid() && fin.isValid() && hoy >= ini && hoy <= fin;
        bool aplica = dias2.isEmpty() || dias2.contains(diaActual, Qt::CaseInsensitive);
        if (dentro && aplica) {
            txt += QString("Promo %1: %2\n  Vigencia: %3 a %4\n")
                .arg(actual->promocion.id)
                .arg(QString::fromStdString(actual->promocion.descripcion))
                .arg(QString::fromStdString(actual->promocion.fechaInicio))
                .arg(QString::fromStdString(actual->promocion.fechaFin));
            NodoBeneficio* b = actual->listaBeneficios;
            while (b) {
                txt += QString("  - %1: %2 (%3)\n")
                    .arg(QString::fromStdString(b->tipo))
                    .arg(QString::fromStdString(b->beneficio))
                    .arg(QString::fromStdString(b->valor));
                b = b->siguiente;
            }
            txt += "\n";
        }
        actual = actual->siguiente;
    } while (actual != listaPromociones->getPrimero());

    txtPromocionesCliente->setText(txt.isEmpty() ? "No hay promociones activas." : txt);
}

void MainWindow::onNuevaSolicitud() {
    if (clienteActual == nullptr) return;
    bool ok;
    QString tipo = QInputDialog::getText(this, "Nueva solicitud", "Tipo:", QLineEdit::Normal, "", &ok);
    if (!ok || tipo.trimmed().isEmpty()) return;
    QString prio = QInputDialog::getItem(this, "Nueva solicitud", "Prioridad:",
                                         {"Baja","Normal","Alta"}, 1, false, &ok);
    if (!ok) return;
    QString desc = QInputDialog::getText(this, "Nueva solicitud", "Descripción:",
                                         QLineEdit::Normal, "", &ok);
    if (!ok) return;

    Solicitud s(clienteActual->nombre, tipo.toStdString());
    s.numero = siguienteNumeroSolicitud++;
    s.prioridad = prio.toStdString();
    s.telefono = clienteActual->telefono;
    s.descripcion = desc.toStdString();
    s.fecha = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    listaSolicitudes->insertar(s);
    QMessageBox::information(this, "Solicitud", "Registrada.");
}

void MainWindow::onConsultarSolicitudes() {
    txtSolicitudesCliente->clear();
    if (clienteActual == nullptr) return;
    NodoListaDoble* actual = listaSolicitudes->getPrimero();
    if (actual == nullptr) { txtSolicitudesCliente->setText("Sin solicitudes."); return; }
    QString txt;
    do {
        if (actual->solicitud.telefono == clienteActual->telefono) {
            txt += QString("#%1 Tipo: %2\nEstado: %3\nDesc: %4\nFecha: %5\n\n")
                .arg(actual->solicitud.numero)
                .arg(QString::fromStdString(actual->solicitud.tipo))
                .arg(QString::fromStdString(actual->solicitud.estado))
                .arg(QString::fromStdString(actual->solicitud.descripcion))
                .arg(QString::fromStdString(actual->solicitud.fecha));
        }
        actual = actual->siguiente;
    } while (actual != listaSolicitudes->getPrimero());
    txtSolicitudesCliente->setText(txt.isEmpty() ? "No tienes solicitudes." : txt);
}