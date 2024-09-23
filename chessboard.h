#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QMainWindow>
#include "cell.h"
#include <QTimer>
#include <QMessageBox>
#include "incorrectwidget.h"
namespace Ui {
class ChessBoard;
}

class ChessBoard : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();
    Cell *chess[20][20];
    int num_in_other;
    int in_home;
    int in_end;
    Cell *current_operator;
    Cell *my_color;
    Cell * enemy_color;
    QTimer * timeclock;
    void game_started();
    int check_results(int step);
    void set_turn(int step);
    //int get_timeout();
public slots:
    void start_timer();
    void stop_timer();
signals:
    void clicked_chess(int t, int x, int y);
    void time_out_three();
    void obey_rule();
    void time_out_signal();
private:
    Ui::ChessBoard *ui;
    int clicked_chess_x;
    int clicked_chess_y;
    int time_show;
    int time_out;
    IncorrectWidget *start_window;
};

#endif // CHESSBOARD_H
