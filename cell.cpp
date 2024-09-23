#include "cell.h"
//#include <QDebug>
Cell::Cell(QWidget* parent):QPushButton(parent)
{
    this->setStyleSheet("Cell{border-image: url(:/new/prefix1/image/white.png)}");
    this->resize(31,26);
    mychess = false;
}

void Cell::set_type(int a){
    type = a;
    if(a == 0){
        this->setStyleSheet("Cell{border-image: url(:/new/prefix1/image/white.png)}");
    }else if(a == 1){
        this->setStyleSheet("Cell{border-image: url(:/new/prefix1/image/blue.png)}");
    }else if(a == 2){
        this->setStyleSheet("Cell{border-image: url(:/new/prefix1/image/yellow.png)}");
    }
}

void Cell::set_position(int a, int b){
    this->x = a;
    this->y = b;
    this->move(40 + this->x *40 + this->y *20, 10 + this->y * 30);
}

void Cell::is_mine(){
    mychess=true;
}

void Cell::mousePressEvent(QMouseEvent *ev){
    if(ev->button() == Qt::LeftButton){
        //qDebug() << "left "<<this->x<< " "<<this->y;
        emit chosen(this->type, this->x, this->y);
    }
}

int Cell::get_x(){
    return this->x;
}

int Cell::get_y(){
    return this->y;
}

void Cell::move_to_place(int a, int b){
    this->move(a,b);
}

int Cell::get_type(){
    return type;
}
