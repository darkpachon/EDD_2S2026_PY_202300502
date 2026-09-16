#ifndef PERFILDIALOG_H
#define PERFILDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../Modelos/Cliente.h"

// Ventana aparte para que el cliente edite sus datos.
class PerfilDialog : public QDialog {
    Q_OBJECT
public:
    PerfilDialog(Cliente* cliente, QWidget* parent = nullptr);

    // Datos resultantes tras aceptar
    QString getNombre()   const { return nombreEdit->text(); }
    QString getCorreo()   const { return correoEdit->text(); }
    QString getTelefono() const { return telefonoEdit->text(); }
    QString getPassword() const { return passwordEdit->text(); }

private:
    Cliente* cliente;
    QLineEdit *nombreEdit, *correoEdit, *telefonoEdit, *passwordEdit;
    QLabel* lblError;
};

#endif