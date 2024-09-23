#include "serverdialog.h"
#include "ui_serverdialog.h"

ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

void ServerDialog::setIP(QString Ip){
    IP = Ip;
    ui->ip->setText(IP);
}

void ServerDialog::on_OK_Button_clicked()
{
    this->close();
    qDebug() <<"send paint_board";
    emit paint_board();
}

