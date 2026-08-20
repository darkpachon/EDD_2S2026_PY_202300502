#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Reportes/GeneradorDot.h"

#include <QPixmap>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringList>
#include <QDate>
#include <QDir>
#include <QFileInfo>
#include <fstream>
#include <sstream>

namespace {
void abrirReporte(MainWindow* ventana, const QString& nombreArchivo) {
    QString ruta = QDir::current().filePath("reportes_generados/" + nombreArchivo);
    QFileInfo archivo(ruta);
    if (archivo.exists()) {
        QLabel* imagen = ventana->findChild<QLabel*>("lblReporte");
        QPixmap pixmap(archivo.absoluteFilePath());
        if (imagen != nullptr && !pixmap.isNull()) {
            imagen->setText("");
            imagen->setPixmap(pixmap.scaled(imagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        QMessageBox::information(ventana, "Reporte generado", "El reporte se muestra dentro de la ventana.");
    } else {
        QMessageBox::warning(ventana, "Graphviz", "No se encontró el PNG. Verifique que Graphviz esté instalado y que dot esté en el PATH.");
    }
}
}

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
    siguienteNumeroSolicitud = 1;
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

void MainWindow::on_btnCerrarSesionAdmin_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->txtUsuario->clear();
    ui->txtPassword->clear();
    ui->lblMensaje->clear();
}

void MainWindow::on_btnCerrarSesionCliente_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->txtUsuario->clear();
    ui->txtPassword->clear();
    ui->lblMensaje->clear();
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

void MainWindow::on_btnEliminarPelicula_clicked() {
    bool ok;
    QString codigo = QInputDialog::getText(this, "Eliminar película", "Código de película (Ej. P034):", QLineEdit::Normal, "", &ok);
    if (!ok || codigo.trimmed().isEmpty()) return;

    std::string codigoTexto = codigo.toStdString();
    if (codigoTexto.size() > 1 && (codigoTexto[0] == 'P' || codigoTexto[0] == 'p')) codigoTexto.erase(0, 1);

    int codigoNumerico;
    try {
        codigoNumerico = std::stoi(codigoTexto);
    } catch (...) {
        QMessageBox::warning(this, "Código inválido", "Ingrese un código como P034.");
        return;
    }

    if (arbol->buscar(codigoNumerico) == nullptr) {
        QMessageBox::warning(this, "No encontrada", "La película no existe en la cartelera.");
        return;
    }
    arbol->eliminar(codigoNumerico);
    ui->lblMensajeAdmin->setText("Película eliminada correctamente.");
}

void MainWindow::agregarPeliculasCartelera(NodoArbol* nodo, QStringList& peliculas) {
    if (nodo == nullptr) return;
    agregarPeliculasCartelera(nodo->izquierdo, peliculas);
    QDate hoy = QDate::currentDate();
    QDate estreno = QDate::fromString(QString::fromStdString(nodo->pelicula.fecha_estreno), "yyyy-MM-dd");
    QDate fin = QDate::fromString(QString::fromStdString(nodo->pelicula.fecha_fin), "yyyy-MM-dd");
    QString estado = "En cartelera";
    if (hoy < estreno) estado = "Proximo estreno";
    else if (hoy > fin) estado = "Fuera de cartelera";
    else if (hoy.daysTo(fin) < 7) estado = "Proximo a retirar";
    peliculas << QString("%1 | %2 | %3 | %4 min | %5 | %6 | %7 a %8 | %9")
        .arg(QString::fromStdString(nodo->pelicula.codigoOriginal))
        .arg(QString::fromStdString(nodo->pelicula.titulo))
        .arg(QString::fromStdString(nodo->pelicula.genero))
        .arg(nodo->pelicula.duracion)
        .arg(QString::fromStdString(nodo->pelicula.clasificacion))
        .arg(QString::fromStdString(nodo->pelicula.idioma))
        .arg(QString::fromStdString(nodo->pelicula.fecha_estreno))
        .arg(QString::fromStdString(nodo->pelicula.fecha_fin))
        .arg(estado);
    agregarPeliculasCartelera(nodo->derecho, peliculas);
}

void MainWindow::on_btnVerCarteleraAdmin_clicked() {
    QStringList peliculas;
    agregarPeliculasCartelera(arbol->getRaiz(), peliculas);
    QMessageBox::information(this, "Cartelera completa", peliculas.isEmpty() ? "No hay peliculas registradas." : peliculas.join("\n"));
}

// --- GENERAR REPORTE DEL ÁRBOL ---
void MainWindow::on_btnGraficarArbol_clicked() {
    std::string rutaImagen = "reporte_arbol.png";

    GeneradorDot generador;
    generador.graficarArbol(arbol, rutaImagen);
    abrirReporte(this, QString::fromStdString(rutaImagen));

    ui->lblMensajeAdmin->setText("¡Reporte de Árbol generado con éxito!");
}

// --- GESTIÓN DE ASIENTOS (ADMIN) ---
void MainWindow::on_btnGestionarAsientos_clicked() {
    QStringList opciones;
    opciones << "1. Configurar función"
             << "2. Reservar asiento"
             << "3. Liberar asiento"
             << "4. Visualizar estado de función"
             << "5. Eliminar función"
             << "6. Generar reporte de matriz";

    bool ok;
    QString seleccion = QInputDialog::getItem(this, "Gestión de Asientos",
                                              "Seleccione una acción:", opciones, 0, false, &ok);
    if (!ok) return;

    if (seleccion == opciones[0]) {
        int filas = QInputDialog::getInt(this, "Configurar función", "Número de filas:", 10, 1, 100, 1, &ok);
        if (!ok) return;
        int columnas = QInputDialog::getInt(this, "Configurar función", "Asientos por fila:", 20, 1, 100, 1, &ok);
        if (!ok) return;
        QString codigo = QInputDialog::getText(this, "Configurar función", "Código de película:", QLineEdit::Normal, "P001", &ok);
        if (!ok) return;
        std::string codigoTexto = codigo.toStdString();
        if (codigoTexto.size() > 1 && (codigoTexto[0] == 'P' || codigoTexto[0] == 'p')) codigoTexto.erase(0, 1);
        int codigoNumerico;
        try {
            codigoNumerico = std::stoi(codigoTexto);
        } catch (...) {
            QMessageBox::warning(this, "Código inválido", "Ingrese un código válido.");
            return;
        }
        Pelicula* pelicula = arbol->buscar(codigoNumerico);
        if (pelicula == nullptr) {
            QMessageBox::warning(this, "Película no encontrada", "Registre la película antes de crear la función.");
            return;
        }
        QString horario = QInputDialog::getText(this, "Configurar función", "Horario (HH:MM):", QLineEdit::Normal, "17:00", &ok);
        if (!ok) return;
        QString sala = QInputDialog::getText(this, "Configurar función", "Sala:", QLineEdit::Normal, "Sala 1", &ok);
        if (!ok) return;
        matriz->configurar(filas, columnas);
        matriz->configurarFuncion(pelicula->titulo, horario.toStdString(), sala.toStdString());
        QMessageBox::information(this, "Función configurada", "La función fue creada y la matriz anterior fue reemplazada.");
    } else if (seleccion == opciones[1]) {
        on_btnReservarAsiento_clicked();
    } else if (seleccion == opciones[2]) {
        int fila = QInputDialog::getInt(this, "Liberar asiento", "Número de fila:", 1, 1, 100, 1, &ok);
        if (!ok) return;
        int columna = QInputDialog::getInt(this, "Liberar asiento", "Número de asiento:", 1, 1, 100, 1, &ok);
        if (!ok) return;
        if (matriz->eliminarAsiento(fila, columna)) {
            QMessageBox::information(this, "Asiento liberado", "El asiento volvió a estar disponible.");
        } else {
            QMessageBox::warning(this, "No encontrado", "Ese asiento no está reservado.");
        }
    } else if (seleccion == opciones[3]) {
        if (matriz->getFilas() == 0) {
            QMessageBox::information(this, "Funciones", "No hay una función configurada.");
            return;
        }
        int total = matriz->getFilas() * matriz->getColumnas();
        int ocupados = 0;
        for (int fila = 1; fila <= matriz->getFilas(); fila++) {
            for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
                if (matriz->buscarAsiento(fila, columna) != nullptr) ocupados++;
            }
        }
        QMessageBox::information(this, "Estado de función",
            "Película: " + QString::fromStdString(matriz->getPelicula())
            + "\nHorario: " + QString::fromStdString(matriz->getHorario())
            + "\nSala: " + QString::fromStdString(matriz->getSala())
            + "\nDimensiones: " + QString::number(matriz->getFilas()) + " x " + QString::number(matriz->getColumnas())
            + "\nAsientos libres: " + QString::number(total - ocupados)
            + "\nAsientos ocupados: " + QString::number(ocupados));
    } else if (seleccion == opciones[4]) {
        if (matriz->getFilas() == 0) {
            QMessageBox::information(this, "Funciones", "No hay una función configurada.");
            return;
        }
        matriz->eliminarFuncion();
        QMessageBox::information(this, "Función eliminada", "La función y sus reservas fueron eliminadas.");
    } else if (seleccion == opciones[5]) {
        GeneradorDot generador;
        generador.graficarMatriz(matriz, "reporte_matriz.png");
        abrirReporte(this, "reporte_matriz.png");
    }
}

// --- GESTIÓN DE PROMOCIONES (ADMIN) ---
void MainWindow::on_btnGestionarPromociones_clicked() {
    QStringList opciones;
    opciones << "1. Agregar Nueva Promoción" 
             << "2. Agregar Beneficio a una Promoción" 
             << "3. Eliminar Promoción"
             << "4. Generar Reporte (Graphviz)";
             
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
        nuevaPromo.fechaInicio = QInputDialog::getText(this, "Nueva Promoción", "Fecha de inicio (AAAA-MM-DD):", QLineEdit::Normal, "", &ok).toStdString();
        if (!ok) return;
        nuevaPromo.fechaFin = QInputDialog::getText(this, "Nueva Promoción", "Fecha de fin (AAAA-MM-DD):", QLineEdit::Normal, "", &ok).toStdString();
        if (!ok) return;
        nuevaPromo.diasAplicables = QInputDialog::getText(this, "Nueva Promoción", "Días aplicables:", QLineEdit::Normal, "Lunes, Martes", &ok).toStdString();
        if (!ok) return;
        listaPromociones->insertarPromocion(nuevaPromo);
        QMessageBox::information(this, "Éxito", "Promoción creada correctamente.");
    } 
    else if (seleccion == opciones[1]) {
        int id = QInputDialog::getInt(this, "Agregar Beneficio", "Ingrese el ID de la promoción existente:", 1, 1, 1000, 1, &ok);
        if (!ok) return;
        
        QString beneficio = QInputDialog::getText(this, "Agregar Beneficio", "Descripción del beneficio (Ej. Palomitas gratis):", QLineEdit::Normal, "", &ok);
        if (!ok || beneficio.isEmpty()) return;

        QString tipo = QInputDialog::getText(this, "Agregar Beneficio", "Tipo (descuento, combo, 2x1):", QLineEdit::Normal, "combo", &ok);
        if (!ok) return;
        QString valor = QInputDialog::getText(this, "Agregar Beneficio", "Valor (Ej. 10% o NA):", QLineEdit::Normal, "NA", &ok);
        if (!ok) return;
        listaPromociones->insertarBeneficio(id, tipo.toStdString(), beneficio.toStdString(), valor.toStdString());
        QMessageBox::information(this, "Éxito", "Beneficio agregado a la promoción.");
    } 
    else if (seleccion == opciones[2]) {
        int id = QInputDialog::getInt(this, "Eliminar Promoción", "ID de la promoción:", 1, 1, 1000, 1, &ok);
        if (!ok) return;
        if (listaPromociones->eliminarPromocion(id)) {
            QMessageBox::information(this, "Éxito", "Promoción eliminada correctamente.");
        } else {
            QMessageBox::warning(this, "No encontrada", "No existe una promoción con ese ID.");
        }
    }
    else if (seleccion == opciones[3]) {
        if (listaPromociones->getPrimero() == nullptr) {
            QMessageBox::information(this, "Promociones", "No hay promociones registradas. Cree una promoción antes de generar el reporte.");
            return;
        }
        GeneradorDot generador;
        generador.graficarListaListas(listaPromociones, "reporte_promociones.png");
        abrirReporte(this, "reporte_promociones.png");
    }
}

// --- GESTIÓN DE SOLICITUDES (ADMIN) ---
void MainWindow::on_btnGestionarSolicitudes_clicked() {
    QStringList opciones;
    opciones << "1. Ver cantidad pendientes"
             << "2. Procesar primera solicitud"
             << "3. Rechazar solicitud"
             << "4. Generar reporte Graphviz";
    bool ok;
    QString seleccion = QInputDialog::getItem(this, "Solicitudes especiales", "Seleccione una acción:", opciones, 0, false, &ok);
    if (!ok) return;

    if (seleccion == opciones[0]) {
        QMessageBox::information(this, "Solicitudes", "Solicitudes registradas: " + QString::number(listaSolicitudes->contar()));
    } else if (seleccion == opciones[1]) {
        NodoListaDoble* primera = listaSolicitudes->getPrimero();
        if (primera == nullptr) {
            QMessageBox::information(this, "Solicitudes", "No hay solicitudes pendientes.");
            return;
        }
        primera->solicitud.estado = "Atendida";
        QMessageBox::information(this, "Solicitud procesada", "La solicitud de " + QString::fromStdString(primera->solicitud.cliente) + " fue atendida.");
    } else if (seleccion == opciones[2]) {
        int numero = QInputDialog::getInt(this, "Rechazar solicitud", "Número de solicitud:", 1, 1, 100000, 1, &ok);
        if (!ok) return;
        if (listaSolicitudes->eliminarPorNumero(numero)) {
            QMessageBox::information(this, "Solicitud rechazada", "La solicitud fue eliminada.");
        } else {
            QMessageBox::warning(this, "No encontrada", "No existe una solicitud con ese número.");
        }
    } else {
        GeneradorDot generador;
        generador.graficarListaDoble(listaSolicitudes, "reporte_solicitudes.png");
        abrirReporte(this, "reporte_solicitudes.png");
    }
}

// --- VISTA CLIENTE: CONSULTAR CARTELERA ---
void MainWindow::on_btnVerCarteleraCliente_clicked() {
    QStringList peliculas;
    agregarPeliculasCartelera(arbol->getRaiz(), peliculas);
    QStringList disponibles;
    for (const QString& pelicula : peliculas) {
        if (pelicula.endsWith("En cartelera") || pelicula.endsWith("Proximo a retirar")) {
            disponibles << pelicula;
        }
    }
    QMessageBox::information(this, "Cartelera disponible",
        disponibles.isEmpty() ? "No hay películas disponibles actualmente." : disponibles.join("\n"));
}

void MainWindow::on_btnBuscarPeliculaCliente_clicked() {
    bool ok;
    QString codigo = QInputDialog::getText(this, "Buscar película", "Código de película (Ej. P034):", QLineEdit::Normal, "", &ok);
    if (!ok || codigo.trimmed().isEmpty()) return;

    std::string codigoTexto = codigo.toStdString();
    if (codigoTexto.size() > 1 && (codigoTexto[0] == 'P' || codigoTexto[0] == 'p')) codigoTexto.erase(0, 1);
    int codigoNumerico;
    try {
        codigoNumerico = std::stoi(codigoTexto);
    } catch (...) {
        QMessageBox::warning(this, "Código inválido", "Ingrese un código como P034.");
        return;
    }
    Pelicula* pelicula = arbol->buscar(codigoNumerico);
    if (pelicula == nullptr) {
        QMessageBox::information(this, "Búsqueda", "No se encontró la película.");
        return;
    }
    QMessageBox::information(this, "Detalle de película",
        "Código: " + QString::fromStdString(pelicula->codigoOriginal)
        + "\nTítulo: " + QString::fromStdString(pelicula->titulo)
        + "\nGénero: " + QString::fromStdString(pelicula->genero)
        + "\nDuración: " + QString::number(pelicula->duracion) + " min"
        + "\nClasificación: " + QString::fromStdString(pelicula->clasificacion)
        + "\nIdioma: " + QString::fromStdString(pelicula->idioma)
        + "\nEstreno: " + QString::fromStdString(pelicula->fecha_estreno)
        + "\nFin: " + QString::fromStdString(pelicula->fecha_fin));
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

    if (matriz->getFilas() == 0) {
        QMessageBox::warning(this, "Función no configurada", "El administrador debe configurar una función antes de reservar.");
        return;
    }

    if (!matriz->insertarAsiento(fila, columna, nombreCliente.toStdString())) {
        QMessageBox::warning(this, "Asiento no disponible", "El asiento está ocupado o fuera de los límites de la sala.");
        return;
    }

    QMessageBox::information(this, "Éxito", "Asiento reservado correctamente para: " + nombreCliente);
}

void MainWindow::on_btnCancelarReserva_clicked() {
    bool ok;
    QString cliente = QInputDialog::getText(this, "Cancelar reserva", "Nombre del cliente:", QLineEdit::Normal, "", &ok);
    if (!ok || cliente.trimmed().isEmpty()) return;
    int fila = QInputDialog::getInt(this, "Cancelar reserva", "Número de fila:", 1, 1, 100, 1, &ok);
    if (!ok) return;
    int columna = QInputDialog::getInt(this, "Cancelar reserva", "Número de asiento:", 1, 1, 100, 1, &ok);
    if (!ok) return;
    NodoMatriz* asiento = matriz->buscarAsiento(fila, columna);
    if (asiento == nullptr) {
        QMessageBox::warning(this, "Reserva no encontrada", "No existe una reserva en esa posicion.");
    } else if (asiento->cliente != cliente.toStdString()) {
        QMessageBox::warning(this, "Cliente incorrecto", "La reserva pertenece a otro cliente.");
    } else if (matriz->eliminarAsiento(fila, columna)) {
        QMessageBox::information(this, "Reserva cancelada", "El asiento fue liberado correctamente.");
    }
}

// --- VISTA CLIENTE: CONSULTAR PROMOCIONES ---
void MainWindow::on_btnConsultarPromociones_clicked() {
    if (listaPromociones->getPrimero() == nullptr) {
        QMessageBox::information(this, "Promociones", "No hay promociones registradas.");
        return;
    }

    QDate hoy = QDate::currentDate();
    QStringList nombresDias;
    nombresDias << "lunes" << "martes" << "miércoles" << "jueves"
                << "viernes" << "sábado" << "domingo";
    QString diaActual = nombresDias.value(hoy.dayOfWeek() - 1);
    QString resultado;
    NodoListaListas* actual = listaPromociones->getPrimero();
    do {
        QDate inicio = QDate::fromString(QString::fromStdString(actual->promocion.fechaInicio), "yyyy-MM-dd");
        QDate fin = QDate::fromString(QString::fromStdString(actual->promocion.fechaFin), "yyyy-MM-dd");
        QString dias = QString::fromStdString(actual->promocion.diasAplicables).toLower();
        bool fechaValida = inicio.isValid() && fin.isValid();
        bool dentroVigencia = fechaValida && hoy >= inicio && hoy <= fin;
        bool aplicaHoy = dias.isEmpty() || dias.contains(diaActual, Qt::CaseInsensitive);

        if (dentroVigencia && aplicaHoy) {
            resultado += "Promoción " + QString::number(actual->promocion.id)
                + "\n" + QString::fromStdString(actual->promocion.descripcion)
                + "\nVigencia: " + QString::fromStdString(actual->promocion.fechaInicio)
                + " a " + QString::fromStdString(actual->promocion.fechaFin)
                + "\nDías: " + QString::fromStdString(actual->promocion.diasAplicables)
                + "\nBeneficios:\n";

            NodoBeneficio* beneficio = actual->listaBeneficios;
            if (beneficio == nullptr) {
                resultado += "  Sin beneficios registrados\n";
            } else {
                while (beneficio != nullptr) {
                    resultado += "  - " + QString::fromStdString(beneficio->tipo)
                        + ": " + QString::fromStdString(beneficio->beneficio)
                        + " (" + QString::fromStdString(beneficio->valor) + ")\n";
                    beneficio = beneficio->siguiente;
                }
            }
            resultado += "\n";
        }
        actual = actual->siguiente;
    } while (actual != listaPromociones->getPrimero());

    QMessageBox::information(this, "Promociones activas",
        resultado.isEmpty() ? "No hay promociones activas para hoy." : resultado);
}

// --- VISTA CLIENTE: REALIZAR SOLICITUD ESPECIAL ---
void MainWindow::on_btnRealizarSolicitud_clicked() {
    bool ok;
    
    QString nombreCliente = QInputDialog::getText(this, "Nueva Solicitud Especial", 
                                                  "Ingrese su nombre:", QLineEdit::Normal, "", &ok);
    if (!ok || nombreCliente.isEmpty()) return;

    QString telefono = QInputDialog::getText(this, "Nueva Solicitud Especial", "Ingrese su teléfono:", QLineEdit::Normal, "", &ok);
    if (!ok || telefono.isEmpty()) return;

    QString tipoSolicitud = QInputDialog::getText(this, "Nueva Solicitud Especial", 
                                                  "Tipo de solicitud (Ej. Silla de ruedas, Subtítulos):", 
                                                  QLineEdit::Normal, "", &ok);
    if (!ok || tipoSolicitud.isEmpty()) return;

    QString prioridad = QInputDialog::getItem(this, "Nueva Solicitud Especial", "Prioridad:",
                                               QStringList() << "Baja" << "Normal" << "Alta", 1, false, &ok);
    if (!ok) return;

    QString descripcion = QInputDialog::getText(this, "Nueva Solicitud Especial", "Describa su solicitud:", QLineEdit::Normal, "", &ok);
    if (!ok || descripcion.isEmpty()) return;

    Solicitud nuevaSolicitud(nombreCliente.toStdString(), tipoSolicitud.toStdString());
    nuevaSolicitud.numero = siguienteNumeroSolicitud++;
    nuevaSolicitud.prioridad = prioridad.toStdString();
    nuevaSolicitud.telefono = telefono.toStdString();
    nuevaSolicitud.descripcion = descripcion.toStdString();
    nuevaSolicitud.fecha = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    listaSolicitudes->insertar(nuevaSolicitud);

    QMessageBox::information(this, "Éxito", "Su solicitud ha sido registrada en el sistema.");
}

void MainWindow::on_btnConsultarSolicitud_clicked() {
    bool ok;
    QString telefono = QInputDialog::getText(this, "Consultar solicitud", "Ingrese su telefono:", QLineEdit::Normal, "", &ok);
    if (!ok || telefono.isEmpty()) return;

    NodoListaDoble* actual = listaSolicitudes->getPrimero();
    QString resultado;
    if (actual != nullptr) {
        do {
            if (actual->solicitud.telefono == telefono.toStdString()) {
                resultado += "Solicitud " + QString::number(actual->solicitud.numero)
                    + "\nTipo: " + QString::fromStdString(actual->solicitud.tipo)
                    + "\nDescripcion: " + QString::fromStdString(actual->solicitud.descripcion)
                    + "\nEstado: " + QString::fromStdString(actual->solicitud.estado)
                    + "\nFecha: " + QString::fromStdString(actual->solicitud.fecha) + "\n\n";
            }
            actual = actual->siguiente;
        } while (actual != listaSolicitudes->getPrimero());
    }
    QMessageBox::information(this, "Estado de solicitudes", resultado.isEmpty() ? "No se encontraron solicitudes." : resultado);
}