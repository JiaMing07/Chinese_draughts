#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>
#include <QString>
namespace Ui {
class ServerDialog;
}

class ServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerDialog(QWidget *parent = nullptr);
    ~ServerDialog();
    void setIP(QString Ip);
signals:
    void paint_board();

private slots:
    void on_OK_Button_clicked();

private:
    Ui::ServerDialog *ui;
    QString IP;
};

#endif // SERVERDIALOG_H
