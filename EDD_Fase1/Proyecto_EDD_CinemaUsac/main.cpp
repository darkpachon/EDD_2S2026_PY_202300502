#include "InterfazGrafica/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w; // Crea la ventana principal
    w.show();     // La muestra en pantalla
    return a.exec(); // Mantiene el programa corriendo
}