#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Reportes/GeneradorDot.h"

#include <QPixmap>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringList>
#include <fstream>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Inicializamos las estructuras en la memoria al abrir el programa
    arbol = new ArbolPeliculas();
    matriz = new MatrizAsientos();
    listaSolicitudes = new ListaCircularDoble();
    listaPromociones = new ListaDeListas();
}

MainWindow::~MainWindow() {
    delete ui;
    delete arbol;
    delete matriz;
    delete listaSolicitudes;
    delete listaPromociones;
}

// --- LÓGICA DEL LOGIN ---
void MainWindow::on_btnIngresar_clicked() {
    QString usuario = ui->txtUsuario->text();
    QString password = ui->txtPassword->text();

    // 1. Acceso para el Administrador
    if (usuario == "admin" && password == "admin123") {
        ui->stackedWidget->setCurrentIndex(1); // Va a la página 1 (Admin)
        ui->txtUsuario->clear();
        ui->txtPassword->clear();
        ui->lblMensaje->clear();
    } 
    // 2. Acceso para el Usuario/Cliente
    else if (usuario == "cliente" && password == "cliente123") {
        ui->stackedWidget->setCurrentIndex(2); // Va a la página 2 (Cliente)
        ui->txtUsuario->clear();
        ui->txtPassword->clear();
        ui->lblMensaje->clear();
    } 
    // 3. Error
    else {
        ui->lblMensaje->setText("Usuario o contraseña incorrectos");
    }
}

// --- LÓGICA DE CARGA MASIVA CSV DE PELÍCULAS ---
void MainWindow::on_btnCargarPeliculas_clicked() {
    QString rutaArchivo = QFileDialog::getOpenFileName(this, "Abrir CSV de Películas", "", "Archivos CSV (*.csv)");
    if (rutaArchivo.isEmpty()) return; 

    std::ifstream archivo(rutaArchivo.toStdString());
    std::string linea;

    if (archivo.is_open()) {
        std::getline(archivo, linea); // Ignorar la primera línea (los encabezados)

        while (std::getline(archivo, linea)) {
            if (!linea.empty() && linea.back() == '\r') {
                linea.pop_back();
            }

            std::stringstream streamLinea(linea);
            std::string codStr, titulo, genero, duracionStr, clasificacion, idioma, fechaEstreno, fechaFin;

            std::getline(streamLinea, codStr, ',');
            std::getline(streamLinea, titulo, ',');
            std::getline(streamLinea, genero, ',');
            std::getline(streamLinea, duracionStr, ',');
            std::getline(streamLinea, clasificacion, ',');
            std::getline(streamLinea, idioma, ',');
            std::getline(streamLinea, fechaEstreno, ',');
            std::getline(streamLinea, fechaFin, ',');

            try {
                std::string soloNumeros = (codStr.length() > 1 && (codStr[0] == 'P' || codStr[0] == 'p')) 
                                          ? codStr.substr(1) : codStr;
                int codigoABuscar = std::stoi(soloNumeros);
                int duracion = std::stoi(duracionStr);

                Pelicula nuevaPelicula(codigoABuscar, codStr, titulo, genero, duracion, clasificacion, idioma, fechaEstreno, fechaFin);
                arbol->insertar(nuevaPelicula);
                
            } catch (...) {
                continue; 
            }
        }
        archivo.close();
        ui->lblMensajeAdmin->setText("¡Archivo cargado con éxito en el Árbol!");
    } else {
        ui->lblMensajeAdmin->setText("Error: No se pudo abrir el archivo");
    }
}

// --- INSERTAR PELÍCULA MANUALMENTE ---
void MainWindow::on_btnInsertarPelicula_clicked() {
    try {
        QString codStr = ui->txtCodigo->text();
        QString titulo = ui->txtTitulo->text();
        QString genero = ui->txtGenero->text();
        QString duracionStr = ui->txtDuracion->text();
        QString clasificacion = ui->txtClasificacion->text();
        QString idioma = ui->txtIdioma->text();
        QString fechaEstreno = ui->txtFechaEstreno->text();
        QString fechaFin = ui->txtFechaFin->text();

        if (codStr.isEmpty() || titulo.isEmpty() || duracionStr.isEmpty()) {
            ui->lblMensajeAdmin->setText("Error: Faltan datos obligatorios.");
            return;
        }

        std::string codigoPelicula = codStr.toStdString();
        std::string soloNumeros = (codigoPelicula.length() > 1 && (codigoPelicula[0] == 'P' || codigoPelicula[0] == 'p')) 
                                  ? codigoPelicula.substr(1) : codigoPelicula;
        int codigoEntero = std::stoi(soloNumeros);
        int duracion = duracionStr.toInt();

        Pelicula nuevaPelicula(codigoEntero, codigoPelicula, titulo.toStdString(), 
                               genero.toStdString(), duracion, clasificacion.toStdString(), 
                               idioma.toStdString(), fechaEstreno.toStdString(), fechaFin.toStdString());
        
        arbol->insertar(nuevaPelicula);

        ui->txtCodigo->clear();
        ui->txtTitulo->clear();
        ui->txtGenero->clear();
        ui->txtDuracion->clear();
        ui->txtClasificacion->clear();
        ui->txtIdioma->clear();
        ui->txtFechaEstreno->clear();
        ui->txtFechaFin->clear();
        
        ui->lblMensajeAdmin->setText("Película guardada correctamente.");

    } catch (...) {
        ui->lblMensajeAdmin->setText("Error al guardar. Revisa que el código y duración sean válidos.");
    }
}

// --- GENERAR REPORTE DEL ÁRBOL ---
void MainWindow::on_btnGraficarArbol_clicked() {
    std::string rutaImagen = "reporte_arbol.png";

    GeneradorDot generador;
    generador.graficarArbol(arbol, rutaImagen);

    QLabel* lblImg = findChild<QLabel*>("lblImagen");
    if (lblImg) {
        QPixmap pix(QString::fromStdString(rutaImagen));
        if (!pix.isNull()) {
            lblImg->setPixmap(pix.scaled(lblImg->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    ui->lblMensajeAdmin->setText("¡Reporte de Árbol generado con éxito!");
}

// --- GESTIÓN DE ASIENTOS (ADMIN) ---
void MainWindow::on_btnGestionarAsientos_clicked() {
    QStringList opciones;
    opciones << "1. Reservar Asiento (Manual)" 
             << "2. Generar Reporte de Matriz (Graphviz)";
             
    bool ok;
    QString seleccion = QInputDialog::getItem(this, "Gestión de Asientos", 
                                              "Seleccione una acción:", opciones, 0, false, &ok);
    if (!ok) return;

    if (seleccion == opciones[0]) {
        on_btnReservarAsiento_clicked();
    } else if (seleccion == opciones[1]) {
        GeneradorDot generador;
        generador.graficarMatriz(matriz, "reporte_matriz.png");
        QMessageBox::information(this, "Éxito", "Reporte de la Matriz generado como 'reporte_matriz.png'.");
    }
}

// --- GESTIÓN DE PROMOCIONES (ADMIN) ---
void MainWindow::on_btnGestionarPromociones_clicked() {
    QStringList opciones;
    opciones << "1. Agregar Nueva Promoción" 
             << "2. Agregar Beneficio a una Promoción" 
             << "3. Generar Reporte (Graphviz)";
             
    bool ok;
    QString seleccion = QInputDialog::getItem(this, "Gestión de Promociones", 
                                              "Seleccione una acción:", opciones, 0, false, &ok);
    if (!ok) return;

    if (seleccion == opciones[0]) {
        int id = QInputDialog::getInt(this, "Nueva Promoción", "ID de la promoción (Ej. 1, 2, 3):", 1, 1, 1000, 1, &ok);
        if (!ok) return;
        
        QString desc = QInputDialog::getText(this, "Nueva Promoción", "Descripción (Ej. 2x1 Estudiantes):", QLineEdit::Normal, "", &ok);
        if (!ok || desc.isEmpty()) return;
        
        int descuento = QInputDialog::getInt(this, "Nueva Promoción", "Porcentaje de descuento (%):", 10, 1, 100, 1, &ok);
        if (!ok) return;

        Promocion nuevaPromo(id, desc.toStdString(), descuento);
        listaPromociones->insertarPromocion(nuevaPromo);
        QMessageBox::information(this, "Éxito", "Promoción creada correctamente.");
    } 
    else if (seleccion == opciones[1]) {
        int id = QInputDialog::getInt(this, "Agregar Beneficio", "Ingrese el ID de la promoción existente:", 1, 1, 1000, 1, &ok);
        if (!ok) return;
        
        QString beneficio = QInputDialog::getText(this, "Agregar Beneficio", "Descripción del beneficio (Ej. Palomitas gratis):", QLineEdit::Normal, "", &ok);
        if (!ok || beneficio.isEmpty()) return;

        listaPromociones->insertarBeneficio(id, beneficio.toStdString());
        QMessageBox::information(this, "Éxito", "Beneficio agregado a la promoción.");
    } 
    else if (seleccion == opciones[2]) {
        GeneradorDot generador;
        generador.graficarListaListas(listaPromociones, "reporte_promociones.png");
        QMessageBox::information(this, "Éxito", "Reporte de Promociones generado como 'reporte_promociones.png'.");
    }
}

// --- GESTIÓN DE SOLICITUDES (ADMIN) ---
void MainWindow::on_btnGestionarSolicitudes_clicked() {
    GeneradorDot generador;
    generador.graficarListaDoble(listaSolicitudes, "reporte_solicitudes.png");
    QMessageBox::information(this, "Éxito", "Reporte de Solicitudes generado como 'reporte_solicitudes.png'.");
}

// --- VISTA CLIENTE: CONSULTAR CARTELERA ---
void MainWindow::on_btnVerCarteleraCliente_clicked() {
    GeneradorDot generador;
    generador.graficarArbol(arbol, "cartelera_cliente.png");
    QMessageBox::information(this, "Cartelera", "Se ha generado la cartelera de películas en 'cartelera_cliente.png'.");
}

// --- VISTA CLIENTE: RESERVAR ASIENTO ---
void MainWindow::on_btnReservarAsiento_clicked() {
    bool ok;
    
    QString nombreCliente = QInputDialog::getText(this, "Reserva de Asiento", "Ingrese su nombre:", QLineEdit::Normal, "", &ok);
    if (!ok || nombreCliente.isEmpty()) return;
    
    int fila = QInputDialog::getInt(this, "Reserva de Asiento", "Ingrese el número de Fila (1-100):", 1, 1, 100, 1, &ok);
    if (!ok) return;

    int columna = QInputDialog::getInt(this, "Reserva de Asiento", "Ingrese el número de Columna (1-100):", 1, 1, 100, 1, &ok);
    if (!ok) return;

    matriz->insertarAsiento(fila, columna, nombreCliente.toStdString());

    QMessageBox::information(this, "Éxito", "Asiento reservado correctamente para: " + nombreCliente);
}

// --- VISTA CLIENTE: CONSULTAR PROMOCIONES ---
void MainWindow::on_btnConsultarPromociones_clicked() {
    GeneradorDot generador;
    generador.graficarListaListas(listaPromociones, "promociones_cliente.png");
    QMessageBox::information(this, "Promociones", "Se ha generado el reporte de promociones en 'promociones_cliente.png'.");
}

// --- VISTA CLIENTE: REALIZAR SOLICITUD ESPECIAL ---
void MainWindow::on_btnRealizarSolicitud_clicked() {
    bool ok;
    
    QString nombreCliente = QInputDialog::getText(this, "Nueva Solicitud Especial", 
                                                  "Ingrese su nombre:", QLineEdit::Normal, "", &ok);
    if (!ok || nombreCliente.isEmpty()) return;

    QString tipoSolicitud = QInputDialog::getText(this, "Nueva Solicitud Especial", 
                                                  "Tipo de solicitud (Ej. Silla de ruedas, Subtítulos):", 
                                                  QLineEdit::Normal, "", &ok);
    if (!ok || tipoSolicitud.isEmpty()) return;

    Solicitud nuevaSolicitud(nombreCliente.toStdString(), tipoSolicitud.toStdString());
    listaSolicitudes->insertar(nuevaSolicitud);

    QMessageBox::information(this, "Éxito", "Su solicitud ha sido registrada en el sistema.");
}