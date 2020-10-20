#ifndef PWORDCHANGE_H
#define PWORDCHANGE_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class pWordChange;
}

class pWordChange : public QWidget
{
    Q_OBJECT

public:
    explicit pWordChange(QWidget *parent = nullptr);
    ~pWordChange();

private slots:
    void on_pushButton_clicked();

    void sendpWRes(QString);

signals:
    void changepWSig(QString);

private:
    Ui::pWordChange *ui;
};

#endif // PWORDCHANGE_H
