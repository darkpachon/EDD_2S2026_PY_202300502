/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *page_login;
    QVBoxLayout *verticalLayout_login;
    QLabel *lblTituloLogin;
    QLineEdit *txtUsuario;
    QLineEdit *txtPassword;
    QPushButton *btnIngresar;
    QLabel *lblMensaje;
    QWidget *page_admin;
    QVBoxLayout *verticalLayout_admin;
    QLabel *lblTituloAdmin;
    QPushButton *btnCerrarSesionAdmin;
    QTabWidget *tabAdmin;
    QWidget *tab_Peliculas;
    QVBoxLayout *vboxLayout;
    QPushButton *btnCargarPeliculas;
    QGroupBox *groupBoxPelicula;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *txtCodigo;
    QLabel *label1;
    QLineEdit *txtTitulo;
    QLabel *label2;
    QLineEdit *txtGenero;
    QLabel *label3;
    QLineEdit *txtDuracion;
    QLabel *label4;
    QLineEdit *txtClasificacion;
    QLabel *label5;
    QLineEdit *txtIdioma;
    QLabel *label6;
    QLineEdit *txtFechaEstreno;
    QLabel *label7;
    QLineEdit *txtFechaFin;
    QPushButton *btnInsertarPelicula;
    QPushButton *btnEliminarPelicula;
    QPushButton *btnVerCarteleraAdmin;
    QPushButton *btnGraficarArbol;
    QLabel *lblMensajeAdmin;
    QWidget *tab_Asientos;
    QVBoxLayout *vboxLayout1;
    QPushButton *btnGestionarAsientos;
    QWidget *tab_PromocionesAdmin;
    QVBoxLayout *vboxLayout2;
    QPushButton *btnGestionarPromociones;
    QWidget *tab_SolicitudesAdmin;
    QVBoxLayout *vboxLayout3;
    QPushButton *btnGestionarSolicitudes;
    QLabel *lblReporte;
    QWidget *page_cliente;
    QVBoxLayout *verticalLayout_cliente;
    QLabel *lblTituloCliente;
    QPushButton *btnCerrarSesionCliente;
    QTabWidget *tabCliente;
    QWidget *tab_Reservas;
    QVBoxLayout *vboxLayout4;
    QPushButton *btnVerCarteleraCliente;
    QPushButton *btnReservarAsiento;
    QPushButton *btnCancelarReserva;
    QWidget *tab_PromocionesCliente;
    QVBoxLayout *vboxLayout5;
    QPushButton *btnConsultarPromociones;
    QWidget *tab_SolicitudesCliente;
    QVBoxLayout *vboxLayout6;
    QPushButton *btnRealizarSolicitud;
    QPushButton *btnConsultarSolicitud;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(900, 700);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        page_login = new QWidget();
        page_login->setObjectName("page_login");
        verticalLayout_login = new QVBoxLayout(page_login);
        verticalLayout_login->setObjectName("verticalLayout_login");
        lblTituloLogin = new QLabel(page_login);
        lblTituloLogin->setObjectName("lblTituloLogin");
        lblTituloLogin->setAlignment(Qt::AlignCenter);
        lblTituloLogin->setStyleSheet(QString::fromUtf8("font-size: 24px; font-weight: bold;"));

        verticalLayout_login->addWidget(lblTituloLogin);

        txtUsuario = new QLineEdit(page_login);
        txtUsuario->setObjectName("txtUsuario");

        verticalLayout_login->addWidget(txtUsuario);

        txtPassword = new QLineEdit(page_login);
        txtPassword->setObjectName("txtPassword");
        txtPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_login->addWidget(txtPassword);

        btnIngresar = new QPushButton(page_login);
        btnIngresar->setObjectName("btnIngresar");

        verticalLayout_login->addWidget(btnIngresar);

        lblMensaje = new QLabel(page_login);
        lblMensaje->setObjectName("lblMensaje");
        lblMensaje->setStyleSheet(QString::fromUtf8("color: red; font-weight: bold;"));
        lblMensaje->setAlignment(Qt::AlignCenter);

        verticalLayout_login->addWidget(lblMensaje);

        stackedWidget->addWidget(page_login);
        page_admin = new QWidget();
        page_admin->setObjectName("page_admin");
        verticalLayout_admin = new QVBoxLayout(page_admin);
        verticalLayout_admin->setObjectName("verticalLayout_admin");
        lblTituloAdmin = new QLabel(page_admin);
        lblTituloAdmin->setObjectName("lblTituloAdmin");
        lblTituloAdmin->setAlignment(Qt::AlignCenter);
        lblTituloAdmin->setStyleSheet(QString::fromUtf8("font-size: 18px; font-weight: bold;"));

        verticalLayout_admin->addWidget(lblTituloAdmin);

        btnCerrarSesionAdmin = new QPushButton(page_admin);
        btnCerrarSesionAdmin->setObjectName("btnCerrarSesionAdmin");

        verticalLayout_admin->addWidget(btnCerrarSesionAdmin);

        tabAdmin = new QTabWidget(page_admin);
        tabAdmin->setObjectName("tabAdmin");
        tab_Peliculas = new QWidget();
        tab_Peliculas->setObjectName("tab_Peliculas");
        vboxLayout = new QVBoxLayout(tab_Peliculas);
        vboxLayout->setObjectName("vboxLayout");
        btnCargarPeliculas = new QPushButton(tab_Peliculas);
        btnCargarPeliculas->setObjectName("btnCargarPeliculas");

        vboxLayout->addWidget(btnCargarPeliculas);

        groupBoxPelicula = new QGroupBox(tab_Peliculas);
        groupBoxPelicula->setObjectName("groupBoxPelicula");
        formLayout = new QFormLayout(groupBoxPelicula);
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBoxPelicula);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        txtCodigo = new QLineEdit(groupBoxPelicula);
        txtCodigo->setObjectName("txtCodigo");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, txtCodigo);

        label1 = new QLabel(groupBoxPelicula);
        label1->setObjectName("label1");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label1);

        txtTitulo = new QLineEdit(groupBoxPelicula);
        txtTitulo->setObjectName("txtTitulo");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, txtTitulo);

        label2 = new QLabel(groupBoxPelicula);
        label2->setObjectName("label2");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label2);

        txtGenero = new QLineEdit(groupBoxPelicula);
        txtGenero->setObjectName("txtGenero");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, txtGenero);

        label3 = new QLabel(groupBoxPelicula);
        label3->setObjectName("label3");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, label3);

        txtDuracion = new QLineEdit(groupBoxPelicula);
        txtDuracion->setObjectName("txtDuracion");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, txtDuracion);

        label4 = new QLabel(groupBoxPelicula);
        label4->setObjectName("label4");

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, label4);

        txtClasificacion = new QLineEdit(groupBoxPelicula);
        txtClasificacion->setObjectName("txtClasificacion");

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, txtClasificacion);

        label5 = new QLabel(groupBoxPelicula);
        label5->setObjectName("label5");

        formLayout->setWidget(5, QFormLayout::ItemRole::LabelRole, label5);

        txtIdioma = new QLineEdit(groupBoxPelicula);
        txtIdioma->setObjectName("txtIdioma");

        formLayout->setWidget(5, QFormLayout::ItemRole::FieldRole, txtIdioma);

        label6 = new QLabel(groupBoxPelicula);
        label6->setObjectName("label6");

        formLayout->setWidget(6, QFormLayout::ItemRole::LabelRole, label6);

        txtFechaEstreno = new QLineEdit(groupBoxPelicula);
        txtFechaEstreno->setObjectName("txtFechaEstreno");

        formLayout->setWidget(6, QFormLayout::ItemRole::FieldRole, txtFechaEstreno);

        label7 = new QLabel(groupBoxPelicula);
        label7->setObjectName("label7");

        formLayout->setWidget(7, QFormLayout::ItemRole::LabelRole, label7);

        txtFechaFin = new QLineEdit(groupBoxPelicula);
        txtFechaFin->setObjectName("txtFechaFin");

        formLayout->setWidget(7, QFormLayout::ItemRole::FieldRole, txtFechaFin);


        vboxLayout->addWidget(groupBoxPelicula);

        btnInsertarPelicula = new QPushButton(tab_Peliculas);
        btnInsertarPelicula->setObjectName("btnInsertarPelicula");

        vboxLayout->addWidget(btnInsertarPelicula);

        btnEliminarPelicula = new QPushButton(tab_Peliculas);
        btnEliminarPelicula->setObjectName("btnEliminarPelicula");

        vboxLayout->addWidget(btnEliminarPelicula);

        btnVerCarteleraAdmin = new QPushButton(tab_Peliculas);
        btnVerCarteleraAdmin->setObjectName("btnVerCarteleraAdmin");

        vboxLayout->addWidget(btnVerCarteleraAdmin);

        btnGraficarArbol = new QPushButton(tab_Peliculas);
        btnGraficarArbol->setObjectName("btnGraficarArbol");

        vboxLayout->addWidget(btnGraficarArbol);

        lblMensajeAdmin = new QLabel(tab_Peliculas);
        lblMensajeAdmin->setObjectName("lblMensajeAdmin");
        lblMensajeAdmin->setStyleSheet(QString::fromUtf8("color: green; font-weight: bold;"));

        vboxLayout->addWidget(lblMensajeAdmin);

        tabAdmin->addTab(tab_Peliculas, QString());
        tab_Asientos = new QWidget();
        tab_Asientos->setObjectName("tab_Asientos");
        vboxLayout1 = new QVBoxLayout(tab_Asientos);
        vboxLayout1->setObjectName("vboxLayout1");
        btnGestionarAsientos = new QPushButton(tab_Asientos);
        btnGestionarAsientos->setObjectName("btnGestionarAsientos");

        vboxLayout1->addWidget(btnGestionarAsientos);

        tabAdmin->addTab(tab_Asientos, QString());
        tab_PromocionesAdmin = new QWidget();
        tab_PromocionesAdmin->setObjectName("tab_PromocionesAdmin");
        vboxLayout2 = new QVBoxLayout(tab_PromocionesAdmin);
        vboxLayout2->setObjectName("vboxLayout2");
        btnGestionarPromociones = new QPushButton(tab_PromocionesAdmin);
        btnGestionarPromociones->setObjectName("btnGestionarPromociones");

        vboxLayout2->addWidget(btnGestionarPromociones);

        tabAdmin->addTab(tab_PromocionesAdmin, QString());
        tab_SolicitudesAdmin = new QWidget();
        tab_SolicitudesAdmin->setObjectName("tab_SolicitudesAdmin");
        vboxLayout3 = new QVBoxLayout(tab_SolicitudesAdmin);
        vboxLayout3->setObjectName("vboxLayout3");
        btnGestionarSolicitudes = new QPushButton(tab_SolicitudesAdmin);
        btnGestionarSolicitudes->setObjectName("btnGestionarSolicitudes");

        vboxLayout3->addWidget(btnGestionarSolicitudes);

        tabAdmin->addTab(tab_SolicitudesAdmin, QString());

        verticalLayout_admin->addWidget(tabAdmin);

        lblReporte = new QLabel(page_admin);
        lblReporte->setObjectName("lblReporte");
        lblReporte->setMinimumSize(QSize(700, 300));
        lblReporte->setFrameShape(QFrame::Box);
        lblReporte->setAlignment(Qt::AlignCenter);
        lblReporte->setScaledContents(false);

        verticalLayout_admin->addWidget(lblReporte);

        stackedWidget->addWidget(page_admin);
        page_cliente = new QWidget();
        page_cliente->setObjectName("page_cliente");
        verticalLayout_cliente = new QVBoxLayout(page_cliente);
        verticalLayout_cliente->setObjectName("verticalLayout_cliente");
        lblTituloCliente = new QLabel(page_cliente);
        lblTituloCliente->setObjectName("lblTituloCliente");
        lblTituloCliente->setAlignment(Qt::AlignCenter);
        lblTituloCliente->setStyleSheet(QString::fromUtf8("font-size: 18px; font-weight: bold;"));

        verticalLayout_cliente->addWidget(lblTituloCliente);

        btnCerrarSesionCliente = new QPushButton(page_cliente);
        btnCerrarSesionCliente->setObjectName("btnCerrarSesionCliente");

        verticalLayout_cliente->addWidget(btnCerrarSesionCliente);

        tabCliente = new QTabWidget(page_cliente);
        tabCliente->setObjectName("tabCliente");
        tab_Reservas = new QWidget();
        tab_Reservas->setObjectName("tab_Reservas");
        vboxLayout4 = new QVBoxLayout(tab_Reservas);
        vboxLayout4->setObjectName("vboxLayout4");
        btnVerCarteleraCliente = new QPushButton(tab_Reservas);
        btnVerCarteleraCliente->setObjectName("btnVerCarteleraCliente");

        vboxLayout4->addWidget(btnVerCarteleraCliente);

        btnReservarAsiento = new QPushButton(tab_Reservas);
        btnReservarAsiento->setObjectName("btnReservarAsiento");

        vboxLayout4->addWidget(btnReservarAsiento);

        btnCancelarReserva = new QPushButton(tab_Reservas);
        btnCancelarReserva->setObjectName("btnCancelarReserva");

        vboxLayout4->addWidget(btnCancelarReserva);

        tabCliente->addTab(tab_Reservas, QString());
        tab_PromocionesCliente = new QWidget();
        tab_PromocionesCliente->setObjectName("tab_PromocionesCliente");
        vboxLayout5 = new QVBoxLayout(tab_PromocionesCliente);
        vboxLayout5->setObjectName("vboxLayout5");
        btnConsultarPromociones = new QPushButton(tab_PromocionesCliente);
        btnConsultarPromociones->setObjectName("btnConsultarPromociones");

        vboxLayout5->addWidget(btnConsultarPromociones);

        tabCliente->addTab(tab_PromocionesCliente, QString());
        tab_SolicitudesCliente = new QWidget();
        tab_SolicitudesCliente->setObjectName("tab_SolicitudesCliente");
        vboxLayout6 = new QVBoxLayout(tab_SolicitudesCliente);
        vboxLayout6->setObjectName("vboxLayout6");
        btnRealizarSolicitud = new QPushButton(tab_SolicitudesCliente);
        btnRealizarSolicitud->setObjectName("btnRealizarSolicitud");

        vboxLayout6->addWidget(btnRealizarSolicitud);

        btnConsultarSolicitud = new QPushButton(tab_SolicitudesCliente);
        btnConsultarSolicitud->setObjectName("btnConsultarSolicitud");

        vboxLayout6->addWidget(btnConsultarSolicitud);

        tabCliente->addTab(tab_SolicitudesCliente, QString());

        verticalLayout_cliente->addWidget(tabCliente);

        stackedWidget->addWidget(page_cliente);

        verticalLayout->addWidget(stackedWidget);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);
        tabAdmin->setCurrentIndex(0);
        tabCliente->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "CinemaUSAC", nullptr));
        lblTituloLogin->setText(QCoreApplication::translate("MainWindow", "INICIAR SESI\303\223N", nullptr));
        txtUsuario->setPlaceholderText(QCoreApplication::translate("MainWindow", "Usuario (admin o cliente)", nullptr));
        txtPassword->setPlaceholderText(QCoreApplication::translate("MainWindow", "Contrase\303\261a", nullptr));
        btnIngresar->setText(QCoreApplication::translate("MainWindow", "Ingresar", nullptr));
        lblMensaje->setText(QString());
        lblTituloAdmin->setText(QCoreApplication::translate("MainWindow", "Panel de Administrador", nullptr));
        btnCerrarSesionAdmin->setText(QCoreApplication::translate("MainWindow", "Cerrar sesi\303\263n", nullptr));
        btnCargarPeliculas->setText(QCoreApplication::translate("MainWindow", "Carga Masiva de Pel\303\255culas (CSV)", nullptr));
        groupBoxPelicula->setTitle(QCoreApplication::translate("MainWindow", "Registrar Pel\303\255cula Manual", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "C\303\263digo (Ej: P034)", nullptr));
        label1->setText(QCoreApplication::translate("MainWindow", "T\303\255tulo", nullptr));
        label2->setText(QCoreApplication::translate("MainWindow", "G\303\251nero", nullptr));
        label3->setText(QCoreApplication::translate("MainWindow", "Duraci\303\263n (min)", nullptr));
        label4->setText(QCoreApplication::translate("MainWindow", "Clasificaci\303\263n", nullptr));
        label5->setText(QCoreApplication::translate("MainWindow", "Idioma", nullptr));
        label6->setText(QCoreApplication::translate("MainWindow", "Fecha Estreno", nullptr));
        label7->setText(QCoreApplication::translate("MainWindow", "Fecha Fin", nullptr));
        btnInsertarPelicula->setText(QCoreApplication::translate("MainWindow", "Guardar Pel\303\255cula", nullptr));
        btnEliminarPelicula->setText(QCoreApplication::translate("MainWindow", "Eliminar Pel\303\255cula", nullptr));
        btnVerCarteleraAdmin->setText(QCoreApplication::translate("MainWindow", "Ver Cartelera Completa", nullptr));
        btnGraficarArbol->setText(QCoreApplication::translate("MainWindow", "Generar Reporte (Graphviz)", nullptr));
        lblMensajeAdmin->setText(QString());
        tabAdmin->setTabText(tabAdmin->indexOf(tab_Peliculas), QCoreApplication::translate("MainWindow", "1. Cartelera y Pel\303\255culas", nullptr));
        btnGestionarAsientos->setText(QCoreApplication::translate("MainWindow", "Gestionar Funciones y Mapa de Asientos", nullptr));
        tabAdmin->setTabText(tabAdmin->indexOf(tab_Asientos), QCoreApplication::translate("MainWindow", "2. Funciones y Asientos", nullptr));
        btnGestionarPromociones->setText(QCoreApplication::translate("MainWindow", "Gesti\303\263n de Promociones Especiales", nullptr));
        tabAdmin->setTabText(tabAdmin->indexOf(tab_PromocionesAdmin), QCoreApplication::translate("MainWindow", "3. Promociones", nullptr));
        btnGestionarSolicitudes->setText(QCoreApplication::translate("MainWindow", "Gestionar Solicitudes Especiales", nullptr));
        tabAdmin->setTabText(tabAdmin->indexOf(tab_SolicitudesAdmin), QCoreApplication::translate("MainWindow", "4. Solicitudes", nullptr));
        lblReporte->setText(QCoreApplication::translate("MainWindow", "El reporte generado aparecer\303\241 aqu\303\255", nullptr));
        lblTituloCliente->setText(QCoreApplication::translate("MainWindow", "Panel de Cliente", nullptr));
        btnCerrarSesionCliente->setText(QCoreApplication::translate("MainWindow", "Cerrar sesi\303\263n", nullptr));
        btnVerCarteleraCliente->setText(QCoreApplication::translate("MainWindow", "Consultar Cartelera", nullptr));
        btnReservarAsiento->setText(QCoreApplication::translate("MainWindow", "Realizar Reserva de Asiento", nullptr));
        btnCancelarReserva->setText(QCoreApplication::translate("MainWindow", "Cancelar Reserva", nullptr));
        tabCliente->setTabText(tabCliente->indexOf(tab_Reservas), QCoreApplication::translate("MainWindow", "1. Cartelera y Reservas", nullptr));
        btnConsultarPromociones->setText(QCoreApplication::translate("MainWindow", "Consultar Promociones Activas", nullptr));
        tabCliente->setTabText(tabCliente->indexOf(tab_PromocionesCliente), QCoreApplication::translate("MainWindow", "2. Promociones", nullptr));
        btnRealizarSolicitud->setText(QCoreApplication::translate("MainWindow", "Realizar Solicitud Especial", nullptr));
        btnConsultarSolicitud->setText(QCoreApplication::translate("MainWindow", "Consultar Estado de Solicitud", nullptr));
        tabCliente->setTabText(tabCliente->indexOf(tab_SolicitudesCliente), QCoreApplication::translate("MainWindow", "3. Solicitudes Especiales", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
