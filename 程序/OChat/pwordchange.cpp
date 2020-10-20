#include "pwordchange.h"
#include "ui_pwordchange.h"

pWordChange::pWordChange(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pWordChange)
{
    ui->setupUi(this);
}

pWordChange::~pWordChange()
{
    delete ui;
}

void pWordChange::on_pushButton_clicked()
{
    QString name=ui->lineEdit_name->text();
    QString oldpW=ui->lineEdit_oldpW->text();
    QString newpW=ui->lineEdit_newpW->text();
    QString renewpW=ui->lineEdit_reNewpW->text();
    if(newpW!=renewpW)
        QMessageBox::warning(this,"错误","两次输入密码不一致，请重新输入");
    else
    {
        QString msg="CPW|"+name+"|"+oldpW+"|"+newpW;
        emit changepWSig(msg);
    }

}

void pWordChange::sendpWRes(QString res)
{
    if(res == "ERR")
    {        
        QMessageBox::warning(this,"错误",res.right(res.length()-5));
    }
    if(res == "SUC")
    {
        QMessageBox::information(this,"提示","修改成功，请重新登录");
    }
}
