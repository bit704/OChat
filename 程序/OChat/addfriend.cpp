#include "addfriend.h"
#include "ui_addfriend.h"

AddFriend::AddFriend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFriend)
{
    ui->setupUi(this);
}

AddFriend::AddFriend(QString myname,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFriend)
{
    ui->setupUi(this);
    MyName=myname;
}

AddFriend::~AddFriend()
{
    delete ui;
}

void AddFriend::on_pushButton_add_clicked()
{
    QString username=ui->lineEdit->text();
    if(username=="")
    {
        QMessageBox::warning(this,"输入错误","请输入有效用户名");
    }
    else
    {
        QString msg="FAF|"+username+"|"+MyName;
        emit addFriendSig(msg);
    }
}


void AddFriend::on_pushButton_del_clicked()
{
    QString username=ui->lineEdit->text();
    if(username=="")
    {
        QMessageBox::warning(this,"输入错误","请输入有效用户名");
    }
    else
    {
        QString msg="DLF|"+username+"|"+MyName;
        emit delFriendSig(msg);
    }

}

void AddFriend::sendErr(QString name)
{
    QMessageBox::information(this,"请求失败",name+" 该用户不存在");
}
