#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *even);

private:
    bool    m_moving=false;
    QPoint  m_lastPos;

    QString sex;
    typedef struct usermsg
    {
        QString sex;
        QString location;
        QString birthday;
    }usermsg;

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

private slots:
    void on_pushButton_clicked();

    void on_radioButtonmale_clicked();

    void on_radioButton_female_clicked();

    void sendRegRes(QString);

signals:
    void sendRegSig(QString);

private:
    Ui::Register *ui;
};

#endif // REGISTER_H
