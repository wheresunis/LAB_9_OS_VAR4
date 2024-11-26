#include "messagewindow.h"
#include "ui_messagewindow.h"
#include "functions.h"
std::string logWin;
messageWindow::messageWindow(QWidget *parent, std::string data)
    : QDialog(parent)
    , ui(new Ui::messageWindow)
{
    ui->setupUi(this);
    logWin=data;
    ui->label_2->text().append(logWin);
}

messageWindow::~messageWindow()
{
    delete ui;
}

void messageWindow::on_sendButton_clicked()
{
    ui->messageSent->clear();
    std::string message;
    if(!ui->messageEdit->text().isEmpty())
    {
        message = ui->messageEdit->text().toStdString();
    }
    ui->messageEdit->clear();
    message.append(" | user: " + logWin);
    if(sendMessage(message))
    {
        ui->messageSent->setText("Message was sent to forum!");
    }
}


void messageWindow::on_exitButton_clicked()
{
    emit closed();
    this->close();
}

