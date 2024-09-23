#ifndef INCORRECTWIDGET_H
#define INCORRECTWIDGET_H

#include <QWidget>
#include <QString>
#include<QDialog>
namespace Ui {
class IncorrectWidget;
}

class IncorrectWidget : public QDialog
{
    Q_OBJECT

public:
    explicit IncorrectWidget(QWidget *parent = nullptr);
    ~IncorrectWidget();
    void changeText(QString a);
    void changeButton(QString a);
    void changeTitle(QString a);
private:
    Ui::IncorrectWidget *ui;
};

#endif // INCORRECTWIDGET_H
