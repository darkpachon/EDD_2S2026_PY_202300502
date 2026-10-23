#ifndef PERFILDIALOG_H
#define PERFILDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../Modelos/Cliente.h"

class PerfilDialog : public QDialog {
    Q_OBJECT
public:
    PerfilDialog(Cliente* cliente, QWidget* parent = nullptr);

private:
    Cliente* cliente;
    QLineEdit *nombreEdit, *correoEdit, *telefonoEdit, *passwordEdit;
    QLabel* lblError;
};

#endif