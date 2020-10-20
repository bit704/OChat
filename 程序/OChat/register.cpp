#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    ui->groupBox->setStyleSheet("QGroupBox{border:none}");
    sex="";
}

Register::~Register()
{
    delete ui;
}

void Register::mousePressEvent(QMouseEvent *event)
{ //鼠标按键被按下
    if (event->button() == Qt::LeftButton)
    {
        m_moving = true;
        //记录下鼠标相对于窗口的位置
        //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
        //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
        m_lastPos = event->globalPos() - pos();
    }
    return QWidget::mousePressEvent(event);  //
}
void Register::mouseMoveEvent(QMouseEvent *event)
{
    if (m_moving && (event->buttons() && Qt::LeftButton == true)
        && (event->globalPos()-m_lastPos).manhattanLength() > QApplication::startDragDistance())
    {
        move(event->globalPos()-m_lastPos);
        m_lastPos = event->globalPos() - pos();
    }
    return QWidget::mouseMoveEvent(event);
}
void Register::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() && Qt::LeftButton ==false)
        m_moving=false;
}


void Register::on_pushButton_clicked()
{
    usermsg user;
    QString name=ui->lineEdit_name->text();
    QString pword=ui->lineEdit_pWord->text();
    QString repword=ui->lineEdit_repWord->text();
    user.sex=sex;
    user.location=ui->lineEdit_location->text();
    QDate birthday=ui->dateEdit_birthday->date();
    user.birthday=birthday.toString("yyyy-MM-dd");
    if(name=="")
    {
        QMessageBox::warning(this,"注册失败","用户名不能为空");
        return;
    }
    if(pword!=repword)
    {
        QMessageBox::warning(this,"注册失败","两次输入密码不一致");
        return;
    }
    if(user.sex=="" or user.location=="" or pword=="")
    {
        QMessageBox::warning(this,"注册失败","请完善用户信息");
        return;
    }
    QString msg="REG|"+name+"|"+pword+"|"+user.sex+"|"+user.location+"|"+user.birthday;
    emit sendRegSig(msg);
}

void Register::on_radioButtonmale_clicked()
{
    sex="男";
}

void Register::on_radioButton_female_clicked()
{
    sex="女";
}

void Register::sendRegRes(QString msg)
{
    if(msg.mid(2,3) == "ERR")
    {
        QMessageBox::warning(this,"错误",msg.right(msg.length()-5));
    }
    if(msg.mid(2,3) == "SUC")
    {
        QMessageBox::information(this,"提示","注册成功，请登录");
        this->close();
    }
}
