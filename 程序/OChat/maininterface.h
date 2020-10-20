#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QWidget>
#include <QTcpSocket>
#include <QMouseEvent>
#include <customtabstyle.h>
#include <qlistwidget.h>
#include <chatui.h>
#include <addfriend.h>
#include <QMessageBox>
#include <portrait.h>
#include <QTextEdit>

namespace Ui {
class MainInterface;
}

class MainInterface : public QWidget
{
    Q_OBJECT

public:
    explicit MainInterface(QWidget *parent = nullptr);
    explicit MainInterface(QTcpSocket *sock, QString myname,QWidget *parent = 0); //从开始页面接受通信句柄

    ~MainInterface();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *even);

    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void keyReleaseEvent(QKeyEvent *ev);

private:
    Ui::MainInterface *ui;

    bool    m_moving=false;
    QPoint  m_lastPos;

    QTcpSocket *fd; //这里使用引用的话后面会出现“拷贝构造函数错误”
    int chatnum=0;
    QMap<QString,QWidget *> fri_map;
    QMap<int,QString> clue;
    QString MyName;
    AddFriend *af;
    QString curName;

    QString delName;

    int clicknum;

private slots:

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void sendMsg(QString);

    void mainReadyRead();

    void on_pushButton_delete_clicked();
    void on_pushButton_addordelete_clicked();

    void sendAddApply(QString);
    void sendDelApply(QString);

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

signals:
    void revMsgSig(QString);
    void addorDelErrSig(QString);

};

#endif // MAININTERFACE_H
