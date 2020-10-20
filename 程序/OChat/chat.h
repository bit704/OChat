#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include <customtabstyle.h>
#include <QTcpSocket>

namespace Ui {
class Chat;
}

class Chat : public QDialog
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    explicit Chat(QTcpSocket *sock, QWidget *parent = 0); //从开始页面接受通信句柄

    ~Chat();

private:
    Ui::Chat *ui;
    QTcpSocket *fd; //这里使用引用的话后面会出现“拷贝构造函数错误”

};

#endif // CHAT_H
