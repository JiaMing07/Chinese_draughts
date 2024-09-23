#include "chessboard.h"
#include "ui_chessboard.h"
//#include <QDebug>
ChessBoard::ChessBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChessBoard)
{
    ui->setupUi(this);
    this->resize(800, 600);
    this->setWindowTitle("游戏界面");
    in_home = 10;
    in_end = 0;
    time_out = 0;
    current_operator = new Cell(this);
    my_color = new Cell(this);
    enemy_color = new Cell(this);
    timeclock = new QTimer(this);
    ui->lcdNumber->display(20);
    time_show=20;
    connect(this->timeclock, &QTimer::timeout,
            [=](){
                time_show--;
                ui->lcdNumber->display(time_show);
                //qDebug() << time_show;
                if(time_show == 0){
                    if(timeclock->isActive()){
                        timeclock->stop();
                        time_show = 20;
                        time_out++;
                        if(time_out == 3){
                            //QMessageBox::information(this, "game results", "You lose, you have timed out three times.");
                            emit time_out_three();
                            this->close();
                        }else{
                            emit time_out_signal();
                            ui->lcdNumber->display(time_show);
                        }
                    }
                }
            }
            );
    //ui->pushButton->setStyleSheet("QPushButton{border-image: url(:/new/prefix1/image/blue.png);}");
    for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            chess[i][j] = nullptr;
            if(j >=0 && j<4){
                if(i >= 12 - j && i <= 12){
                    chess[i][j] = new Cell(this);
                    chess[i][j]->set_type(0);
                    chess[i][j]->set_position(i, j);
                    connect(chess[i][j],&Cell::chosen, this, &ChessBoard::clicked_chess);
                }
            }else if(j >= 4 && j < 8){
                if(i >= 4 && i <= 16 - (j - 4)){
                    chess[i][j] = new Cell(this);
                    chess[i][j]->set_type(0);
                    chess[i][j]->set_position(i, j);
                    connect(chess[i][j],&Cell::chosen, this, &ChessBoard::clicked_chess);
                }
            }else if(j >= 8 && j <= 12){
                if(i >= 4 - (j - 8) && i <= 12){
                    chess[i][j] = new Cell(this);
                    chess[i][j]->set_type(0);
                    chess[i][j]->set_position(i, j);
                    connect(chess[i][j],&Cell::chosen, this, &ChessBoard::clicked_chess);
                }
            }else if(j > 12 && j <= 16){
                if(i >= 4 && i <= 7 - (j - 13)){
                    chess[i][j] = new Cell(this);
                    chess[i][j]->set_type(0);
                    chess[i][j]->set_position(i, j);
                    connect(chess[i][j],&Cell::chosen, this, &ChessBoard::clicked_chess);
                }
            }
        }
    }

}

ChessBoard::~ChessBoard()
{
    delete ui;
}

void ChessBoard::start_timer(){
    if(!timeclock->isActive()){
        timeclock->start(1000);
    }
}

void ChessBoard::stop_timer(){
    timeclock->stop();
    ui->lcdNumber->display(20);
    time_show =20;
}

void ChessBoard::game_started(){
    //QMessageBox::about(this,"Hint.","Game Started");
    this->start_window = new IncorrectWidget(this);
    start_window->changeButton("OK");
    start_window->changeText("Game Started");
    start_window->changeTitle("hints");
    start_window->show();
}

int ChessBoard::check_results(int step){
    if(step == 20){
        if(in_home > 5){
            if(timeclock->isActive()){
                timeclock->stop();
            }
            QMessageBox::information(this, "game results", "You lose. There are more than 5 chesses in your end in the 20th round.");
            //emit obey_rule();
            this->close();
            return 1;
        }
    }else if(step == 25){
        if(in_home > 2){
            if(timeclock->isActive()){
                timeclock->stop();
            }
            QMessageBox::information(this, "game results", "You lose. There are more than 2 chesses in your end in the 25th round.");
            //emit obey_rule();
            this->close();
            return 1;
        }
    }else if(step == 30){
        if(in_home > 0){
            if(timeclock->isActive()){
                timeclock->stop();
            }
            QMessageBox::information(this, "game results", "You lose. There are more than 0 chesses in your end in the 30th round.");
            //emit obey_rule();
            this->close();
            return 1;
        }
    }
    if(in_end == 10){
        if(timeclock->isActive()){
            timeclock->stop();
        }
        QMessageBox::information(this, "game results", "You win");
        this->close();
        return 2;
    }
    return 0;
}

void ChessBoard::set_turn(int step){
    ui->turns->setText(QString("<html><head/><body><p><span style=\" font-size:12pt;\">%1</span></p></body></html>").arg(step));
}
