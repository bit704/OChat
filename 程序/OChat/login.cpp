#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    mySock.connectToHost("10.194.181.175",1234);
    connect(&mySock,SIGNAL(connected()),this,SLOT(hadConnect()));

}

Login::~Login()
{
    delete ui;
}

void Login::mousePressEvent(QMouseEvent *event)
{ //鼠标按键被按下
    if (event->button() == Qt::LeftButton)
    {
        m_moving = true;
        //记录下鼠标相对于窗口的位置
        //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
        //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
        m_lastPos = event->globalPos() - pos();
    }
    return QDialog::mousePressEvent(event);  //
}
void Login::mouseMoveEvent(QMouseEvent *event)
{
    if (m_moving && (event->buttons() && Qt::LeftButton == true)
        && (event->globalPos()-m_lastPos).manhattanLength() > QApplication::startDragDistance())
    {
        move(event->globalPos()-m_lastPos);
        m_lastPos = event->globalPos() - pos();
    }
    return QDialog::mouseMoveEvent(event);
}
void Login::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() && Qt::LeftButton ==false)
        m_moving=false;
}

void Login::hadConnect()  //成功连接之后的操作
{
    ui->pushButton_log->setEnabled(true);
    ui->pushButton_reg->setEnabled(true);
    connect(&mySock,SIGNAL(readyRead()),this,SLOT(hadReadyRead()));
}

void Login::hadReadyRead() //收到信息之后的操作
{
    QByteArray rcvBuf = mySock.readAll();
    QString rcvStr(rcvBuf);

    if(rcvStr.left(1)=="1") //注册回馈
    {
        emit sendRegMsgSig(rcvStr);
    }
    if(rcvStr.left(1)=="2") //登录回馈
    {
        if(rcvStr.mid(2,3) == "ERR")
        {
            QMessageBox::warning(this,"错误",rcvStr.right(rcvStr.length()-5));
        }
        if(rcvStr.mid(2,3) == "SUC")
        {
            mf = new MainInterface(&mySock,myname);
            mf->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
            mf->show();    //显示主界面
            this->hide();  //隐藏开始界面
            disconnect(&mySock,SIGNAL(readyRead()),this,SLOT(hadReadyRead())); //后两个参数写0的话，会断开失败
        }
    }
    if(rcvStr.left(1)=="3")
    {
        QString changepWmsg=rcvStr.mid(2,3);
        emit sendpWResSig(changepWmsg);
    }
}


void Login::on_pushButton_log_clicked()
{
    myname = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();
    QString send_1 = "LOG|"+myname+"|"+password;

    mySock.write(send_1.toStdString().c_str());
}

void Login::on_pushButton_reg_clicked()
{
    rg=new Register();
    rg->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    connect(rg,SIGNAL(sendRegSig(QString)),this,SLOT(sendRegApply(QString)));
    connect(this,SIGNAL(sendRegMsgSig(QString)),rg,SLOT(sendRegRes(QString)));

    rg->show();

}

void Login::on_pushButton_changepW_clicked()
{
    pc = new pWordChange();
    connect(pc,SIGNAL(changepWSig(QString)),this,SLOT(sendChangepWMsg(QString)));
    connect(this,SIGNAL(sendpWResSig(QString)),pc,SLOT(sendpWRes(QString)));
    pc->show();
}

void Login::sendChangepWMsg(QString msg)
{
    mySock.write(msg.toStdString().c_str());
}

void Login::sendRegApply(QString msg)
{
    mySock.write(msg.toStdString().c_str());
}

void Login::on_pushButton_3_clicked()
{
    rg->close();
    pc->close();
}
