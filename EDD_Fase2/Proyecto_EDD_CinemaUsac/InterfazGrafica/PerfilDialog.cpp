#include "PerfilDialog.h"
#include "../Utilidades/Validaciones.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>

PerfilDialog::PerfilDialog(Cliente* c, QWidget* parent)
    : QDialog(parent), cliente(c)
{
    setWindowTitle("Editar Perfil");
    setMinimumWidth(380);

    auto* layout = new QVBoxLayout(this);
    auto* form = new QFormLayout();

    nombreEdit   = new QLineEdit(QString::fromStdString(c->nombre));
    correoEdit   = new QLineEdit(QString::fromStdString(c->correo));
    telefonoEdit = new QLineEdit(QString::fromStdString(c->telefono));
    passwordEdit = new QLineEdit(QString::fromStdString(c->password));
    passwordEdit->setEchoMode(QLineEdit::Password);

    form->addRow("Nombre:",   nombreEdit);
    form->addRow("Correo:",   correoEdit);
    form->addRow("Teléfono:", telefonoEdit);
    form->addRow("Password:", passwordEdit);
    layout->addLayout(form);

    lblError = new QLabel();
    lblError->setStyleSheet("color:red; font-weight:bold;");
    layout->addWidget(lblError);

    auto* botones = new QHBoxLayout();
    auto* btnAceptar  = new QPushButton("Guardar");
    auto* btnCancelar = new QPushButton("Cancelar");
    botones->addWidget(btnAceptar);
    botones->addWidget(btnCancelar);
    layout->addLayout(botones);

    connect(btnAceptar, &QPushButton::clicked, this, [this]() {
        std::string n = Val::trim(nombreEdit->text().toStdString());
        std::string co = Val::trim(correoEdit->text().toStdString());
        std::string t = Val::trim(telefonoEdit->text().toStdString());
        std::string p = passwordEdit->text().toStdString();

        if (n.empty())                     { lblError->setText("Nombre vacío"); return; }
        if (!Val::esCorreoValido(co))      { lblError->setText("Correo inválido"); return; }
        if (!Val::esTelefonoValido(t))     { lblError->setText("Teléfono inválido"); return; }
        if (p.empty())                     { lblError->setText("Password vacío"); return; }

        cliente->nombre   = n;
        cliente->correo   = co;
        cliente->telefono = t;
        cliente->password = p;
        accept();
    });
    connect(btnCancelar, &QPushButton::clicked, this, &QDialog::reject);
}