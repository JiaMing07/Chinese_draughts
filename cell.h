#ifndef CELL_H
#define CELL_H

#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>
class Cell : public QPushButton
{
        Q_OBJECT
public:
    explicit Cell(QWidget *parent=0);
    void set_type(int a);
    void set_position(int x, int y);
    int get_type();
    void is_mine();
    int get_x();
    int get_y();
    void move_to_place(int a, int b);
signals:
    void chosen(int t, int a, int b);
protected:
    void mousePressEvent(QMouseEvent *ev);

private:
    int type;
    int x;
    int y;
    bool mychess;
};

#endif // CELL_H
