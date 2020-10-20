#ifndef PORTRAIT_H
#define PORTRAIT_H

#include <QWidget>
#include <QImage>
#include <QString>
namespace Ui {
class Portrait;
}

class Portrait : public QWidget
{
    Q_OBJECT

public:
    explicit Portrait(QWidget *parent = 0);
    explicit Portrait(QString name,QWidget *parent = 0);
    ~Portrait();

private:
    Ui::Portrait *ui;
};

#endif // PORTRAIT_H
