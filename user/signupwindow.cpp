#include "signupwindow.h"
#include "ui_signupwindow.h"
#include "functions.h"
SignUpWindow::SignUpWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SignUpWindow)
{
    ui->setupUi(this);
}

SignUpWindow::~SignUpWindow()
{
    delete ui;
}

void SignUpWindow::on_pushButton_clicked()
{
    std::string loginName, password;

    if(ui->lineEdit_2->text()!=ui->lineEdit_3->text() || ui->lineEdit_2->text().isEmpty() || ui->lineEdit_3->text().isEmpty())
    {
        ui->errorLabel->setText("Incorrect password confirmation!");
    }
    else
    {
        password = ui->lineEdit_3->text().toStdString();
        if(!ui->lineEdit->text().isEmpty())
        {
            loginName = ui->lineEdit->text().toStdString();
            if(checkIfNameExistsAlready(loginName))
            {
                ui->errorLabel->setText("This username exists already!");
            }
            else
            {
                registerUser(loginName, password);
                this->close();
            }
        }
        else
        { ui->errorLabel->setText("Enter a login!");}
    }



}

