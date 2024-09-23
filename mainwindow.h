#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "serverdialog.h"
#include "clientdialog.h"
#include "incorrectwidget.h"
#include <QTcpServer>
#include <QTcpSocket>
#include "chessboard.h"
#include <vector>
#include <algorithm>
#include <QTime>
#include <QRandomGenerator>
#include <QMessageBox>
#include "incorrectwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void client_walk(int a, int b);

    void server_walk(int a, int b);

    void client_jump(int a, int b);

    void server_jump(int a, int b);

    QString getHostIpAddress();

private slots:
    void on_actionCreate_the_Connection_triggered();

    void on_actionConnect_to_Server_triggered();

    void show_incorrect_info(QString a);

    void begin_server();

    void show_chessboard();

    void show_clientboard();

    void begin_client(QString a);

    void client_deal_with_chess(int t, int a, int b);

    void server_deal_with_chess(int t, int a, int b);

    void on_actionStart_triggered();

    void on_actionAdmit_Defeat_triggered();

    void client_time_out_lose();

    void server_time_out_lose();

    void client_time_out();

    void server_time_out();

signals:
    void client_time_play();

    void server_time_play();

    void client_time_stop();

    void server_time_stop();

    void start_game();

private:
    Ui::MainWindow *ui;
    QString local_Ip;
    ServerDialog *serverdia;
    ClientDialog *clientdia;
    IncorrectWidget *incorrect;
    QTcpServer *tcpserver;
    QTcpSocket *tcpsocket;
    QTcpSocket *clientsocket;
    ChessBoard *serverboard;
    ChessBoard *clientboard;
    int clicked_chess_x;
    int clicked_chess_y;
    int server_steps;
    int client_steps;
    int ran_num;
    int server_start;
    int client_start;
    bool has_server;
    bool has_client;
    bool started;
    bool client_turn;
    bool server_turn;
    std::vector<int> server_possible;
    std::vector<int> client_possible;
    bool server_mark[400];
    bool client_mark[400];
    IncorrectWidget *connect_window;
    //IncorrectWidget *server_start_window;
    //IncorrectWidget *client_start_window;
};
#endif // MAINWINDOW_H
