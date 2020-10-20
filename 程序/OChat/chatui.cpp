#include "chatui.h"
#include "ui_chatui.h"
#include <QDebug>

ChatUI::ChatUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatUI)
{
    ui->setupUi(this);
}


ChatUI::ChatUI(QString name, QString myname,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatUI)
{
    ui->setupUi(this);
    MyName=myname;
    curName=name;
    // this->grabKeyboard();
    /*
    ui->pushButton_send->setFocus();
    ui->pushButton_send->setDefault(true);
    ui->textEdit_input->installEventFilter(this);
    */
}

ChatUI::~ChatUI()
{
    delete ui;
}

void ChatUI::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Enter or event->key()==Qt::Key_Return)
        on_pushButton_send_clicked();
    if(event->key()==Qt::Key_Escape)
        emit closeTableSig();
}
void ChatUI::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Enter or  event->key()==Qt::Key_Return)
        on_pushButton_send_clicked();
    if(event->key()==Qt::Key_Escape)
        emit closeTableSig();
}

void ChatUI::on_pushButton_send_clicked()
{
    QString txt=ui->textEdit_input->toPlainText();
    if(txt=="")
        return;
    ui->textEdit_input->setText("");
    QString sendMsgIfo="CWH|"+curName+"|"+txt+"|"+MyName;
    emit sendButClickSig(sendMsgIfo);

    ui->textBrowser_output->setAlignment(Qt::AlignRight);
    ui->textBrowser_output->append(txt);
    ui->textBrowser_output->append("<font size=2> </font>");
    ui->textBrowser_output->moveCursor(QTextCursor::End);
}

void ChatUI::MsgOutPut(QString MsgInfo)
{
    QStringList Info = MsgInfo.split("|");
    QString name = Info[1];
    QString msg = Info[0];
    if(curName==name)
    {
        ui->textBrowser_output->append("<font color=\"#3333FF\">"+msg+"</font>");
        ui->textBrowser_output->setAlignment(Qt::AlignLeft);
        ui->textBrowser_output->append("<font size=2> </font>");
        ui->textBrowser_output->moveCursor(QTextCursor::End);
    }
}

