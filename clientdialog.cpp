#include "clientdialog.h"
#include "ui_clientdialog.h"
#include <QStringList>
//#include <QDebug>
ClientDialog::ClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientDialog)
{
    ui->setupUi(this);
    tcpsocket = nullptr;
    tcpsocket = new QTcpSocket(this);
}

ClientDialog::~ClientDialog()
{
    delete ui;
}

void ClientDialog::on_OK_Button_clicked()
{
    QString IP = ui->IP_Edit->text();
    if(IP.isEmpty()){
        QString str = QString("You do not enter IP.");
        emit not_correct(str);
    }else{
        QStringList num = IP.split('.');
        if(num.size() != 4){
            QString str = QString("IP is not correct.");
            emit not_correct(str);
        }else{
            bool isok = true;
            for(auto i : num){
                bool ok = false;
                int temp = i.toInt(&ok);
                if(!ok || temp<0 ||temp>255){
                    isok = false;
                    QString str = QString("IP is not correct.");
                    emit not_correct(str);
                }
            }
            if(isok == true){
                /*IP = "127.0.0.1";
                qDebug()<<"connect to Host";
                tcpsocket->connectToHost(QHostAddress(IP),qint16(8000));
                qDebug() <<qint16(8000);
                connect(tcpsocket, &QTcpSocket::connected,
                        [=](){
                            QString str = "connected_to";
                            tcpsocket->write(str.toUtf8().data());
                        }
                        );
                connect(tcpsocket, &QTcpSocket::readyRead,
                        [=](){
                            QByteArray array=tcpsocket->readAll();
                            qDebug()<<array.data();
                        }
                );*/
                QString ip = ui->IP_Edit->text();
                emit show_board(ip);
                this->close();
            }
        }
    }
}


void ClientDialog::on_Cancel_Button_clicked()
{
    close();
}

