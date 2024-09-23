#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <QTcpSocket>
namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = nullptr);
    ~ClientDialog();

signals:
    void not_correct(QString a);

    void show_board(QString a);



private slots:

    void on_OK_Button_clicked();

    void on_Cancel_Button_clicked();



private:
    Ui::ClientDialog *ui;
    QTcpSocket *tcpsocket;
};

#endif // CLIENTDIALOG_H
