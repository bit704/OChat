#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class AddFriend;
}

class AddFriend : public QWidget
{
    Q_OBJECT

private:
     QString MyName;

public:
    explicit AddFriend(QWidget *parent = nullptr);
    explicit AddFriend(QString myname,QWidget *parent = nullptr);
    ~AddFriend();

signals:
    void addFriendSig(QString);
    void delFriendSig(QString);

private slots:
    void on_pushButton_add_clicked();
    void on_pushButton_del_clicked();

    void sendErr(QString);

private:
    Ui::AddFriend *ui;
};

#endif // ADDFRIEND_H
