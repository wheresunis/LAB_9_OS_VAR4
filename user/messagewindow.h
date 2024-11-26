#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <QDialog>
namespace Ui {
class messageWindow;
}

class messageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit messageWindow(QWidget *parent = nullptr, std::string data = "default");
    ~messageWindow();

private slots:

    void on_sendButton_clicked();

    void on_exitButton_clicked();

signals: void closed();
private:
    Ui::messageWindow *ui;
};

#endif // MESSAGEWINDOW_H
