#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "signupwindow.h"
#include "functions.h"
#include "messagewindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    SignUpWindow signUpForm;
    signUpForm.setModal(true);
    signUpForm.exec();
}


void MainWindow::on_pushButton_clicked()
{
    std::string logName = ui->lineEdit->text().toStdString();
    std::string password = ui->lineEdit_2->text().toStdString();
    if(loginUser(logName, password))
    {
        messageWindow *messageWin = new messageWindow(this, logName);
        connect(messageWin, &messageWindow::closed, this, &MainWindow::showMainWindow);
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        this->hide();
        messageWin->exec();
    }
    else
    {
        ui->errorLabel->setText("No user found. Check your login and password.");
    }
}

void MainWindow::showMainWindow() { this->show(); }

