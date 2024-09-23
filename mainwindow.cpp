#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostInfo>
#include <QDebug>
#include <string>
#include <QStringList>
#include <QtGlobal>
#include <vector>
#include <algorithm>
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client_turn = false;
    server_turn = false;
    client_steps = 0;
    server_steps = 0;
    has_server = false;
    has_client = false;
    for(int i=0;i<400;i++){
        server_mark[i] = false;
        client_mark[i] = false;
    }
    if(server_possible.size() != 0){
        server_possible.clear();
    }
    if(client_possible.size() != 0 ){
        client_possible.clear();
    }
    ran_num = QRandomGenerator::global()->bounded(1, 3);
    if(ran_num == 1){
        client_turn = true;
    }else{
        server_turn = true;
    }
    tcpserver = nullptr;
    tcpsocket = nullptr;
    this->setWindowTitle("Chess");
    local_Ip = getHostIpAddress();
    serverdia = new ServerDialog(this);
    serverdia->setWindowTitle("Create the Connection");
    serverdia->setIP(local_Ip);
    tcpserver = new QTcpServer(this);
    connect(serverdia, &ServerDialog::paint_board, this, &MainWindow::begin_server);
    connect(serverdia, &ServerDialog::paint_board, this, &MainWindow::show_chessboard);
    client_start = 0;
    server_start = 0;
    started = false;
    setWindowTitle("服务器: 8888");

    connect(tcpserver, &QTcpServer::newConnection,
                [=]()
                {
                    //取出建立好连接的套接字
                    tcpsocket = tcpserver->nextPendingConnection();

                    //获取对方的IP和端口
                    QString ip = tcpsocket->peerAddress().toString();
                    int port = tcpsocket->peerPort();
                    QString temp = QString("[%1:%2]:成功连接").arg(ip).arg(port);
                    qDebug() << temp;
                    connect(tcpsocket, &QTcpSocket::readyRead,
                            [=]()
                            {
                                //从通信套接字中取出内容
                                qDebug() << "readyRead";
                                QByteArray array = tcpsocket->readAll();
                                qDebug() << array.data();
                                QString str = array;
                                QStringList lst = str.split('.');
                                int a[5];
                                if(lst.size()==5){
                                    for(int i=0;i<5;i++){
                                        a[i] = lst[i].toInt();
                                        //qDebug() << a[i];
                                    }
                                    serverboard->chess[16-a[0]][16-a[1]]->set_type(1);
                                    serverboard->chess[16-a[2]][16-a[3]]->set_type(0);
                                    server_turn = true;
                                    emit server_time_play();
                                    //qDebug() << "client time_play";
                                    serverboard->current_operator->set_type(2);
                                    if(ran_num == 2){
                                        serverboard->set_turn(server_steps + 1);
                                    }
                                }else if(lst.size() == 1){
                                    if(str == "1"){
                                        client_start = 1;
                                        //qDebug() << "server_socket";
                                        if(server_start == 1){
                                            this->started = true;
                                            /*if(clientboard){
                                                clientboard->game_started();
                                                if(ran_num == 1){
                                                    clientboard->start_timer();
                                                }
                                            }*/
                                            if(serverboard){
                                                qDebug() <<"server board";
                                                serverboard->game_started();
                                                if(ran_num == 2){
                                                    server_turn = true;
                                                    serverboard->start_timer();
                                                }
                                            }
                                        }
                                    }else if(str == "r"){
                                        QString ran = QString("r.%1").arg(ran_num);
                                        tcpsocket->write(ran.toUtf8().data());
                                        //QMessageBox::information(serverboard,"hint","connected");
                                        this->connect_window = new IncorrectWidget(serverboard);
                                        connect_window->changeButton("OK");
                                        connect_window->changeText("connected");
                                        connect_window->changeTitle("hints");
                                        connect_window->show();
                                    }else if(str == "w"){
                                        serverboard->stop_timer();
                                        QMessageBox::information(serverboard, "game results", "You win, your opponent admits defeat");
                                        serverboard->close();
                                    }else if(str == "tw"){
                                        QMessageBox::information(serverboard, "game results", "You win, your oppennent timed out three times");
                                        //qDebug()<<"serversocket";
                                        serverboard->close();
                                    }else if(str == "o"){
                                        QMessageBox::information(serverboard, "game results", "You win, your opponent obeys rule");
                                        serverboard->close();
                                    }else if(str == "win"){
                                        QMessageBox::information(serverboard, "game results", "You lose");
                                        serverboard->close();
                                    }else if(str == "to"){
                                        server_turn = true;
                                        emit server_time_play();
                                        serverboard->current_operator->set_type(2);
                                        if(ran_num == 2){
                                            serverboard->set_turn(server_steps + 1);
                                        }
                                    }else if(str == "c"){
                                        //QMessageBox::information(serverboard, "hint", "connected");
                                        this->connect_window = new IncorrectWidget(serverboard);
                                        connect_window->changeButton("OK");
                                        connect_window->changeText("connected");
                                        connect_window->changeTitle("hints");
                                        connect_window->show();
                                    }
                                }

                            }

                            );


                }

                );
    clientdia = new ClientDialog(this);
    clientdia->setWindowTitle("Connect to Server");
    incorrect = new IncorrectWidget(this);
    //incorrect->hide();
    serverboard = nullptr;
    clientboard = nullptr;
    connect(clientdia, &ClientDialog::not_correct,this, &MainWindow::show_incorrect_info);
    connect(clientdia, &ClientDialog::show_board, this, &MainWindow::begin_client);
    //connect(clientdia, &ClientDialog::show_board, this, &MainWindow::show_clientboard);
}

MainWindow::~MainWindow()
{
    delete ui;
}


QString MainWindow::getHostIpAddress()
{
    QString IP;
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    int ListSize = ipList.size();
    for (int i = 0; i < ListSize; ++i)
    {
           if (ipList[i] != QHostAddress::LocalHost && ipList[i].protocol() == QAbstractSocket::IPv4Protocol) {
               bool isok = false;
               ipList[i].toIPv4Address(&isok);
               if (isok){
                IP = ipList[i].toString();
               }
               break;
           }
     }
     if (IP.isEmpty())
        IP = QHostAddress(QHostAddress::LocalHost).toString();
     //qDebug()<< IP;
     return IP;
}

void MainWindow::on_actionCreate_the_Connection_triggered() //打开显示hostIP窗口
{
    serverdia->show();
}


void MainWindow::on_actionConnect_to_Server_triggered()//打开连接服务器窗口
{
    clientdia->show();
}

void MainWindow::show_incorrect_info(QString a){
    incorrect->changeText(a);
    incorrect->show();
}

void MainWindow::begin_server(){
    tcpserver->listen(QHostAddress(local_Ip),qint16(8888));
    has_server = true;
}

void MainWindow::show_chessboard(){
    serverboard = new ChessBoard(this);
    connect(this, &MainWindow::server_time_play, serverboard, &ChessBoard::start_timer);
    connect(this, &MainWindow::server_time_stop, serverboard, &ChessBoard::stop_timer);
    connect(serverboard, &ChessBoard::time_out_three, this, &MainWindow::server_time_out_lose);
    connect(serverboard, &ChessBoard::time_out_signal, this, &MainWindow::server_time_out);
    for(int i=9; i<13;i++){
        for(int j=0;j<4;j++){
            if(i >= 12 - j && i <= 12){
                serverboard->chess[i][j]->set_type(1);
            }
        }
    }
    for(int i=4;i<8;i++){
        for(int j=13;j<=16;j++){
            if(i >= 4 && i <= 7 - (j - 13)){
                serverboard->chess[i][j]->set_type(2);
            }
        }
    }
    serverboard->current_operator->set_type(ran_num);
    serverboard->current_operator->move_to_place(120,220);;
    serverboard->my_color->set_type(2);
    serverboard->my_color->move_to_place(140,340);
    serverboard->enemy_color->set_type(1);
    serverboard->enemy_color->move_to_place(140,400);
    connect(serverboard, &ChessBoard::clicked_chess, this, &MainWindow::server_deal_with_chess);
    serverboard->show();
}

void MainWindow::show_clientboard(){
    clientboard = new ChessBoard(this);
    connect(this, &MainWindow::client_time_play, clientboard, &ChessBoard::start_timer);
    connect(this, &MainWindow::client_time_stop, clientboard, &ChessBoard::stop_timer);
    connect(clientboard, &ChessBoard::time_out_three, this, &MainWindow::client_time_out_lose);
    connect(clientboard, &ChessBoard::time_out_signal, this, &MainWindow::client_time_out);
    for(int i=9; i<13;i++){
        for(int j=0;j<4;j++){
            if(i >= 12 - j && i <= 12){
                clientboard->chess[i][j]->set_type(2);
            }
        }
    }
    for(int i=4;i<8;i++){
        for(int j=13;j<=16;j++){
            if(i >= 4 && i <= 7 - (j - 13)){
                clientboard->chess[i][j]->set_type(1);
            }
        }
    }
    clientboard->current_operator->set_type(ran_num);
    clientboard->current_operator->move_to_place(120,220);;
    clientboard->my_color->set_type(1);
    clientboard->my_color->move_to_place(140,340);
    clientboard->enemy_color->set_type(2);
    clientboard->enemy_color->move_to_place(140,400);
    connect(clientboard, &ChessBoard::clicked_chess, this, &MainWindow::client_deal_with_chess);
    clientboard->show();
}

void MainWindow::begin_client(QString a){
    clientsocket = new QTcpSocket(this);
    has_client = true;
    setWindowTitle("客户端");
    QString ip = a;
    qint16 port = 8888;
    clientsocket->connectToHost(QHostAddress(ip), port);
    connect(clientsocket, &QTcpSocket::connected,
                [=]()
                {
                    //qDebug() << "success";
                    if(!has_server){
                        //qDebug() << "!has_server";
                        //QMessageBox::information(clientboard, "hint", "!hasServer");
                        QString str = "r";
                        clientsocket->write(str.toUtf8().data());
                        //qDebug() <<str;
                        //QMessageBox::information(clientboard, "hint", "connected");
                    }else{
                        QString str = "c";
                        clientsocket->write(str.toUtf8().data());
                        show_clientboard();
                        //QMessageBox::information(clientboard, "hint", "connected");
                    }
                    //clientsocket->write("success_read");
                }
                );
    connect(clientsocket, &QTcpSocket::readyRead,
                [=]()
                {
                    //获取对方发送的内容
                    QByteArray array = clientsocket->readAll();
                    //追加到编辑区中
                    qDebug() << array.data();
                    QString str = array;
                    QStringList lst = str.split('.');
                    int a[5];
                    if(lst.size()==5){
                        for(int i=0;i<5;i++){
                            a[i] = lst[i].toInt();
                            //qDebug() << a[i];
                        }
                        clientboard->chess[16-a[0]][16-a[1]]->set_type(2);
                        clientboard->chess[16-a[2]][16-a[3]]->set_type(0);
                        client_turn = true;
                        emit client_time_play();
                        clientboard->current_operator->set_type(1);
                        if(ran_num == 1){
                            clientboard->set_turn(client_steps + 1);
                        }
                    }else if(lst.size() == 1){
                        if(str == "1"){
                            server_start = 1;
                            //qDebug() << "client_socket";
                            if(client_start == 1){
                                this->started = true;
                                if(clientboard){
                                    clientboard->game_started();
                                    if(ran_num == 1){
                                        client_turn = true;
                                        clientboard->start_timer();
                                    }
                                }
                                /*if(serverboard){
                                    serverboard->game_started();
                                    if(ran_num == 2){
                                        serverboard->start_timer();
                                    }
                                }*/
                            }
                        }else if(str == "w"){
                            clientboard->stop_timer();
                            QMessageBox::information(clientboard, "game results", "You win, your opponent admits defeat");
                            clientboard->close();
                        }else if(str == "tw"){
                            QMessageBox::information(clientboard, "game results", "You win, your oppennent timed out three times");
                            //qDebug()<<"client socket";
                            clientboard->close();
                        }else if(str == "o"){
                            QMessageBox::information(clientboard, "game results", "You win, your opponent obeys rule");
                            clientboard->close();
                        }else if(str == "win"){
                            QMessageBox::information(clientboard, "game results", "you lose");
                            clientboard->close();
                        }else if(str == "to"){
                            client_turn = true;
                            emit client_time_play();
                            clientboard->current_operator->set_type(1);
                            if(ran_num == 1){
                                clientboard->set_turn(client_steps + 1);
                            }
                        }
                    }else if(lst.size() == 2){
                        //QMessageBox::information(clientboard,"hint", "r.");
                        ran_num = lst[1].toInt();
                        //qDebug() << ran_num;
                        if(ran_num == 1){
                            client_turn = true;
                        }else{
                            server_turn = true;
                        }
                        show_clientboard();
                    }
                }

                );
}

void MainWindow::client_deal_with_chess(int t, int a,int b){
    //qDebug() <<server_turn <<" "<<client_turn;
    if(started && client_turn){
        if(t==0){
            if(this->clicked_chess_x != 0 &&this->clicked_chess_y != 0){
                //qDebug() << "empty " << a << " " << b << "vectorsize" <<client_possible.size();
                bool can_move = false;
                /*for(int i=0;i<client_possible.size();i++){
                    qDebug() << "client" <<client_possible[i];
                }*/
                std::vector<int> ::iterator search = std::find(client_possible.begin(),client_possible.end(),a * 20 +b);
                if(search!=client_possible.end()){
                    can_move = true;
                }
                if(can_move){
                    clientboard->chess[a][b]->set_type(1);
                    clientboard->chess[this->clicked_chess_x][this->clicked_chess_y]->set_type(0);
                    this->client_steps++;
                    QString str;
                    str.clear();
                    str = QString("%1.%2.%3.%4.2").arg(a).arg(b).arg(this->clicked_chess_x).arg(this->clicked_chess_y);
                    if(clicked_chess_y>12 && b<=12 && b>=4){
                        clientboard->in_home--;
                    }else if(clicked_chess_y<=12 && clicked_chess_y>=4 && b>12){
                        clientboard->in_home++;
                    }else if(clicked_chess_y>=4 && clicked_chess_y<=12 && b<4){
                        clientboard->in_end++;
                    }else if(clicked_chess_y<4 && b>=4 && b<=12){
                        clientboard->in_end--;
                    }else if(clicked_chess_y>12 && b<4){
                        clientboard->in_home--;
                        clientboard->in_end++;
                    }else if(clicked_chess_y<4 && b>12){
                        clientboard->in_home++;
                        clientboard->in_end--;
                    }
                    //qDebug() << "client steps"<<client_steps;
                    int ret = clientboard->check_results(client_steps);
                    this->clicked_chess_x = 0;
                    this->clicked_chess_y = 0;
                    emit client_time_stop();
                    client_turn = false;
                    if(ret == 0){
                        clientsocket->write(str.toUtf8().data());
                        clientboard->current_operator->set_type(2);
                    }else if(ret == 1){
                        QString obey_rules = "o";
                        //client_time_stop();
                        clientsocket->write(obey_rules.toUtf8().data());
                    }else if(ret == 2){
                        QString winner = "win";
                        clientsocket->write(winner.toUtf8().data());
                    }
                    if(ran_num == 2){
                        clientboard->set_turn(client_steps+1);
                    }
                    //qDebug() <<"client run"<<str;
                }
            }
        }else if(t == 1){
            //qDebug() << "chess " << a << " " << b;
            this->clicked_chess_x = a;
            this->clicked_chess_y = b;
            client_walk(a, b);
            client_jump(a, b);
            /*if(client_steps == 0){
                emit client_time_play();
            }*/
        }else{
            this->clicked_chess_x = 0;
            this->clicked_chess_y = 0;
        }
    }
}

void MainWindow::server_deal_with_chess(int t, int a,int b){
    //qDebug() <<server_turn <<" "<<client_turn;
    if(started && server_turn){
        if(t==0){
            if(this->clicked_chess_x != 0 &&this->clicked_chess_y != 0){
                qDebug() << "empty " << a << " " << b;
                bool can_move = false;
                /*for(int i=0;i<server_possible.size();i++){
                    qDebug() << "server" <<server_possible[i];
                }*/
                std::vector<int> ::iterator search = std::find(server_possible.begin(),server_possible.end(),a * 20 +b);
                if(search!=server_possible.end()){
                    can_move = true;
                }
                if(can_move){
                    serverboard->chess[a][b]->set_type(2);
                    serverboard->chess[this->clicked_chess_x][this->clicked_chess_y]->set_type(0);
                    this->server_steps++;
                    QString str;
                    str.clear();
                    str = QString("%1.%2.%3.%4.1").arg(a).arg(b).arg(this->clicked_chess_x).arg(this->clicked_chess_y);
                    if(clicked_chess_y>12 && b<=12 && b>=4){
                        serverboard->in_home--;
                    }else if(clicked_chess_y<=12 && clicked_chess_y>=4 && b>12){
                        serverboard->in_home++;
                    }else if(clicked_chess_y>=4 && clicked_chess_y<=12 && b<4){
                        serverboard->in_end++;
                    }else if(clicked_chess_y<4 && b>=4 && b<=12){
                        serverboard->in_end--;
                    }else if(clicked_chess_y>12 && b<4){
                        serverboard->in_home--;
                        serverboard->in_end++;
                    }else if(clicked_chess_y<4 && b>12){
                        serverboard->in_home++;
                        serverboard->in_end--;
                    }
                    //qDebug() << "server steps"<<server_steps;
                    int ret = serverboard->check_results(server_steps);
                    this->clicked_chess_x = 0;
                    this->clicked_chess_y = 0;
                    //qDebug() <<" run";
                    emit server_time_stop();
                    server_turn = false;
                    if(ret == 0){
                        tcpsocket->write(str.toUtf8().data());
                        serverboard->current_operator->set_type(1);
                    }else if(ret == 1){
                        QString obey_rules = "o";
                        //server_time_stop();
                        tcpsocket->write(obey_rules.toUtf8().data());
                    }else if(ret == 2){
                        QString winner = "win";
                        tcpsocket->write(winner.toUtf8().data());
                    }
                    if(ran_num == 1){
                        serverboard->set_turn(server_steps + 1);
                    }
                    //qDebug() <<"server run "<<str;
                }
            }
        }else if(t == 2){
            qDebug() << "chess " << a << " " << b;
            this->clicked_chess_x = a;
            this->clicked_chess_y = b;
            server_walk(a,b);
            server_jump(a,b);
            /*if(steps == 0){
                emit server_time_play();
            }*/
        }else{
            this->clicked_chess_x = 0;
            this->clicked_chess_y = 0;
        }
    }
}



void MainWindow::on_actionStart_triggered()
{
    //qDebug() << has_server <<" "<< has_client;
    if(has_server){
        if(server_start == 1){
            if(has_client){
                client_start = 1;
                started = true;
                if(clientboard){
                    clientboard->game_started();
                }
                if(serverboard){
                    serverboard->game_started();
                }
                if(ran_num == 1){
                    if(clientboard){
                        clientboard->start_timer();
                    }
                }else{
                    if(serverboard){
                        serverboard->start_timer();
                    }
                }
            }
        }else{
            server_start = 1;
            if(!has_client){
                QString str = "1";
                tcpsocket->write(str.toUtf8().data());
            }
            if(client_start == 1){
                started = true;
                if(serverboard){
                    serverboard->game_started();
                    if(ran_num == 2){
                        serverboard->start_timer();
                    }
                }
                if(clientboard){
                    clientboard->game_started();
                    if(ran_num == 1){
                        clientboard->start_timer();
                    }
                }
            }
        }
    }else if(!has_server){
        if(has_client){
            client_start = 1;
            QString str = "1";
            clientsocket->write(str.toUtf8().data());
            if(server_start == 1){
                started = true;
                if(clientboard){
                    clientboard->game_started();
                    if(ran_num == 1){
                        clientboard->start_timer();
                    }
                }
            }
        }
    }
}


void MainWindow::on_actionAdmit_Defeat_triggered()
{

    if(has_server){
        if(server_steps > 20 || (server_steps == 20 && ran_num == 1) || (server_steps == 20 && server_turn == true)){
        //QMessageBox::StandardButtons re = QMessageBox::question(serverboard, "Admit defeat", "Do you really want to admit defeat", QMessageBox::Yes | QMessageBox::No);
            serverboard->stop_timer();
            QString str = "w";
            tcpsocket->write(str.toUtf8().data());
            QMessageBox::information(serverboard, "game results", "You lose");
            serverboard->close();
        }
    }
    if(has_client){
        if(client_steps >20 || (client_steps == 20 && ran_num == 2) || (client_steps == 20 && client_turn == true)){
        //int ret = QMessageBox::question(clientboard, "Admit defeat", "Do you really want to admit defeat", QMessageBox::Yes | QMessageBox::No);
            clientboard->stop_timer();
            QString str = "w";
            clientsocket->write(str.toUtf8().data());
            QMessageBox::information(clientboard, "game results", "You lose");
            clientboard->close();
        }
    }

}

void MainWindow::client_time_out_lose(){
    if(has_server){
        QMessageBox::information(serverboard, "game results", "You win, your opponent timed out three times");
        //qDebug()
        serverboard->close();
    }else if(!has_server){
        QString str = "tw";
        clientsocket->write(str.toUtf8().data());
    }
    QMessageBox::information(clientboard, "game results", "You lose, you have timed out three times.");
}

void MainWindow::server_time_out_lose(){
    if(has_client){
        //qDebug()<<"server_time_out_lose";
        QMessageBox::information(clientboard, "game results", "You win, your opponent timed out three times");
        clientboard->close();
    }else if(!has_client){
        QString str = "tw";
        tcpsocket->write(str.toUtf8().data());
    }
    QMessageBox::information(serverboard, "game results", "You lose, you have timed out three times.");
}

void MainWindow::client_walk(int a, int b){
    //qDebug() <<"client walk";
    for(int i=0;i<client_possible.size();i++){
        client_mark[client_possible[i]] = false;
    }
    client_possible.clear();
    if(clientboard->chess[a][b - 1]){
        if(clientboard->chess[a][b - 1]->get_type() == 0){
            client_possible.push_back(a * 20 + b - 1);
        }
    }
    if(clientboard->chess[a + 1][b - 1]){
        if(clientboard->chess[a + 1][b - 1]->get_type() == 0){
            client_possible.push_back((a + 1) * 20 + b - 1);
            //qDebug() << "has push";
        }
    }
    if(clientboard->chess[a - 1][b]){
        if(clientboard->chess[a - 1][b]->get_type() == 0){
            client_possible.push_back((a - 1) * 20 + b);
        }
    }
    if(clientboard->chess[a + 1][b]){
        if(clientboard->chess[a + 1][b]->get_type() == 0){
            client_possible.push_back((a + 1) * 20 + b);
        }
    }
    if(clientboard->chess[a - 1][b + 1]){
        if(clientboard->chess[a - 1][b + 1]->get_type() == 0){
            client_possible.push_back((a - 1) * 20 + b + 1);
        }
    }
    if(clientboard->chess[a][b + 1]){
        if(clientboard->chess[a][b + 1]->get_type() == 0){
            client_possible.push_back(a * 20 + b + 1);
        }
    }
}

void MainWindow::server_walk(int a, int b){
    //qDebug() <<"server walk";
    for(int i=0;i<server_possible.size();i++){
        server_mark[server_possible[i]] = false;
    }
    server_possible.clear();
    if(serverboard->chess[a][b - 1]){
        if(serverboard->chess[a][b - 1]->get_type() == 0){
            server_possible.push_back(a * 20 + b - 1);
        }
    }
    if(serverboard->chess[a + 1][b - 1]){
        if(serverboard->chess[a + 1][b - 1]->get_type() == 0){
            server_possible.push_back((a + 1) * 20 + b - 1);
            //qDebug() <<"push back";
        }
    }
    if(serverboard->chess[a - 1][b]){
        if(serverboard->chess[a - 1][b]->get_type() == 0){
            server_possible.push_back((a - 1) * 20 + b);
        }
    }
    if(serverboard->chess[a + 1][b]){
        if(serverboard->chess[a + 1][b]->get_type() == 0){
            server_possible.push_back((a + 1) * 20 + b);
        }
    }
    if(serverboard->chess[a - 1][b + 1]){
        if(serverboard->chess[a - 1][b + 1]->get_type() == 0){
            server_possible.push_back((a - 1) * 20 + b + 1);
        }
    }
    if(serverboard->chess[a][b + 1]){
        if(serverboard->chess[a][b + 1]->get_type() == 0){
            server_possible.push_back(a * 20 + b + 1);
        }
    }
}

void MainWindow::client_jump(int a, int b){
    //qDebug() <<"client jump";
    if(clientboard->chess[a][b - 1] && clientboard->chess[a][b - 2] && !client_mark[a * 20 + b - 2]){
        if(clientboard->chess[a][b - 1]->get_type() != 0 && clientboard->chess[a][b - 2]->get_type() == 0){
            client_possible.push_back((a) * 20 + b - 2);
            client_mark[a * 20 + b - 2] = true;
            client_jump(a,b - 2);
        }
    }
    if(clientboard->chess[a + 1][b - 1] && clientboard->chess[a + 2][b - 2] && !client_mark[(a + 2) * 20 + b - 2]){
        if(clientboard->chess[a + 1][b - 1]->get_type() != 0 && clientboard->chess[a + 2][b - 2]->get_type() == 0){
            client_possible.push_back((a + 2) * 20 + b - 2);
            client_mark[(a + 2) * 20 + b - 2] = true;
            client_jump(a + 2, b - 2);
        }
    }
    if(clientboard->chess[a - 1][b] && clientboard->chess[a - 2][b] && !client_mark[(a - 2) * 20 + b]){
        if(clientboard->chess[a - 1][b]->get_type() != 0 && clientboard->chess[a - 2][b]->get_type() == 0){
            client_possible.push_back((a - 2) * 20 + b);
            client_mark[(a - 2) * 20 + b] = true;
            client_jump(a - 2, b);
        }
    }
    if(clientboard->chess[a + 1][b] && clientboard->chess[a + 2][b] && !client_mark[(a + 2) * 20 + b]){
        if(clientboard->chess[a + 1][b]->get_type() != 0 && clientboard->chess[a + 2][b]->get_type() == 0){
            client_possible.push_back((a + 2) * 20 + b);
            client_mark[(a + 2) * 20 + b] = true;
            client_jump(a + 2, b);
        }
    }
    if(clientboard->chess[a - 1][b + 1] && clientboard->chess[a - 2][b + 2] && !client_mark[(a - 2) * 20 + b + 2]){
        if(clientboard->chess[a - 1][b + 1]->get_type() != 0 && clientboard->chess[a - 2][b + 2]->get_type() == 0){
            client_possible.push_back((a - 2) * 20 + b + 2);
            client_mark[(a - 2) * 20 + b + 2] = true;
            client_jump(a - 2, b + 2);
        }
    }
    if(clientboard->chess[a][b + 1] && clientboard->chess[a][b + 2] && !client_mark[a * 20 + b + 2]){
        if(clientboard->chess[a][b + 1]->get_type() != 0 && clientboard->chess[a][b + 2]->get_type() == 0){
            client_possible.push_back(a * 20 + b + 2);
            client_mark[a * 20 + b + 2] = true;
            client_jump(a, b + 2);
        }
    }
}

void MainWindow::server_jump(int a, int b){
    //qDebug() <<"server jump";
    if(serverboard->chess[a][b - 1] && serverboard->chess[a][b - 2] && !server_mark[a * 20 + b - 2]){
        if(serverboard->chess[a][b - 1]->get_type() != 0 && serverboard->chess[a][b - 2]->get_type() == 0){
            server_possible.push_back(a * 20 + b - 2);
            server_mark[a * 20 + b - 2] = true;
            server_jump(a,b - 2);
        }
    }
    if(serverboard->chess[a + 1][b - 1] && serverboard->chess[a + 2][b - 2] && !server_mark[(a + 2) * 20 + b - 2]){
        if(serverboard->chess[a + 1][b - 1]->get_type() != 0 && serverboard->chess[a + 2][b - 2]->get_type() == 0){
            server_possible.push_back((a + 2) * 20 + b - 2);
            server_mark[(a + 2) * 20 + b - 2] = true;
            server_jump(a + 2, b - 2);
        }
    }
    if(serverboard->chess[a - 1][b] && serverboard->chess[a - 2][b] && !server_mark[(a - 2) * 20 + b]){
        if(serverboard->chess[a - 1][b]->get_type() != 0 && serverboard->chess[a - 2][b]->get_type() == 0){
            server_possible.push_back((a - 2) * 20 + b);
            server_mark[(a - 2) * 20 + b] = true;
            server_jump(a - 2, b);
        }
    }
    if(serverboard->chess[a + 1][b] && serverboard->chess[a + 2][b] && !server_mark[(a + 2) * 20 + b]){
        if(serverboard->chess[a + 1][b]->get_type() != 0 && serverboard->chess[a + 2][b]->get_type() == 0){
            server_possible.push_back((a + 2) * 20 + b);
            server_mark[(a + 2) * 20 + b] = true;
            server_jump(a + 2, b);
        }
    }
    if(serverboard->chess[a - 1][b + 1] && serverboard->chess[a - 2][b + 2] && !server_mark[(a - 2) * 20 + b + 2]){
        if(serverboard->chess[a - 1][b + 1]->get_type() != 0 && serverboard->chess[a - 2][b + 2]->get_type() == 0){
            server_possible.push_back((a - 2) * 20 + b + 2);
            server_mark[(a - 2) * 20 + b + 2] = true;
            server_jump(a - 2, b + 2);
        }
    }
    if(serverboard->chess[a][b + 1] && serverboard->chess[a][b + 2] && !server_mark[a * 20 + b + 2]){
        if(serverboard->chess[a][b + 1]->get_type() != 0 && serverboard->chess[a][b + 2]->get_type() == 0){
            server_possible.push_back(a * 20 + b + 2);
            server_mark[a * 20 + b + 2] = true;
            server_jump(a, b + 2);
        }
    }
}

void MainWindow::client_time_out(){
    client_turn = false;
    QString str = "to";
    clientsocket->write(str.toUtf8().data());
    client_steps++;
    clientboard->current_operator->set_type(2);
    if(ran_num == 2){
        clientboard->set_turn(client_steps + 1);
    }
}

void MainWindow::server_time_out(){
    server_turn = false;
    QString str = "to";
    tcpsocket->write(str.toUtf8().data());
    serverboard->current_operator->set_type(1);
    server_steps++;
    if(ran_num == 1){
        serverboard->set_turn(server_steps + 1);
    }
}
