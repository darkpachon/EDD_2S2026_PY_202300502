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
     
    void on_btnIngresar_clicked();
    void on_btnCerrarSesionAdmin_clicked();
    void on_btnCerrarSesionCliente_clicked();

     
    void on_btnCargarPeliculas_clicked();
    void on_btnInsertarPelicula_clicked();
    void on_btnEliminarPelicula_clicked();
    void on_btnVerCarteleraAdmin_clicked();
    void on_btnGraficarArbol_clicked();
    void on_btnGestionarAsientos_clicked();      
    void on_btnGestionarPromociones_clicked();   
    void on_btnGestionarSolicitudes_clicked();   

     
    void on_btnVerCarteleraCliente_clicked();    
    void on_btnBuscarPeliculaCliente_clicked();
    void on_btnReservarAsiento_clicked();        
    void on_btnCancelarReserva_clicked();
    void on_btnConsultarPromociones_clicked();   
    void on_btnRealizarSolicitud_clicked();      
    void on_btnConsultarSolicitud_clicked();

private:
    Ui::MainWindow *ui;
    
     
    ArbolPeliculas* arbol;
    MatrizAsientos* matriz;
    ListaCircularDoble* listaSolicitudes;
    ListaDeListas* listaPromociones;
    int siguienteNumeroSolicitud;

    void agregarPeliculasCartelera(NodoArbol* nodo, QStringList& peliculas);
};

#endif