#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QMessageBox>
#include <maininterface.h>
#include <pwordchange.h>
#include <register.h>
#include <QIcon>
#include <QPixmap>


QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QDialog
{
    Q_OBJECT

private:
    bool    m_moving=false;
    QPoint  m_lastPos;
    QTcpSocket mySock;
    QString myname;

    MainInterface *mf;
    pWordChange *pc;
    Register *rg;



public:
    Login(QWidget *parent = nullptr);
    ~Login();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *even);

private slots:
    void hadConnect();
    void hadReadyRead();

    void on_pushButton_log_clicked();
    void on_pushButton_reg_clicked();
    void on_pushButton_changepW_clicked();

    void sendChangepWMsg(QString);

    void sendRegApply(QString);

    void on_pushButton_3_clicked();

signals:
    void sendpWResSig(QString);
    void sendRegMsgSig(QString);

private:
    Ui::Login *ui;
};
#endif // LOGIN_H
