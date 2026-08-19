#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Estructuras/ArbolPeliculas.h"
#include "../Estructuras/MatrizAsientos.h"
#include "../Estructuras/ListaCircularDoble.h"
#include "../Estructuras/ListaDeListas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // --- Eventos Generales ---
    void on_btnIngresar_clicked();
    void on_btnCerrarSesionAdmin_clicked();
    void on_btnCerrarSesionCliente_clicked();

    // --- Eventos del Administrador ---
    void on_btnCargarPeliculas_clicked();
    void on_btnInsertarPelicula_clicked();
    void on_btnEliminarPelicula_clicked();
    void on_btnVerCarteleraAdmin_clicked();
    void on_btnGraficarArbol_clicked();
    void on_btnGestionarAsientos_clicked();     // Para administrar la matriz
    void on_btnGestionarPromociones_clicked();  // Para agregar promos y beneficios
    void on_btnGestionarSolicitudes_clicked();  // Para ver la lista circular

    // --- Eventos del Cliente ---
    void on_btnVerCarteleraCliente_clicked();   // Para ver el árbol
    void on_btnReservarAsiento_clicked();       // <--- NUEVO: Inserción en Matriz
    void on_btnCancelarReserva_clicked();
    void on_btnConsultarPromociones_clicked();  // Para ver la lista de listas
    void on_btnRealizarSolicitud_clicked();     // Para insertar en la lista circular
    void on_btnConsultarSolicitud_clicked();

private:
    Ui::MainWindow *ui;
    
    // Punteros a las estructuras globales en memoria
    ArbolPeliculas* arbol;
    MatrizAsientos* matriz;
    ListaCircularDoble* listaSolicitudes;
    ListaDeListas* listaPromociones;
    int siguienteNumeroSolicitud;

    void agregarPeliculasCartelera(NodoArbol* nodo, QStringList& peliculas);
};

#endif // MAINWINDOW_H