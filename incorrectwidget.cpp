#include "incorrectwidget.h"
#include "ui_incorrectwidget.h"

IncorrectWidget::IncorrectWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IncorrectWidget)
{
    ui->setupUi(this);
}

IncorrectWidget::~IncorrectWidget()
{
    delete ui;
}

void IncorrectWidget::changeText(QString a){
    ui->label->setText(a);
}

void IncorrectWidget::changeButton(QString a){
    ui->cancel_Button->setText(a);
}

void IncorrectWidget::changeTitle(QString a){
    setWindowTitle(a);
}
