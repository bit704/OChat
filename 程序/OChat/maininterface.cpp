#include "maininterface.h"
#include "ui_maininterface.h"
#include <QDebug>

MainInterface::MainInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainInterface)
{
    ui->setupUi(this);
}

MainInterface::MainInterface(QTcpSocket *sock,QString myname,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainInterface)
{
    ui->setupUi(this);

    fd = sock;
    MyName = myname;
    clicknum=0;
    curName="";
    connect(fd,SIGNAL(readyRead()),this,SLOT(mainReadyRead()));

    ui->listWidget->addItem("");
    ui->listWidget->addItem("");
    ui->listWidget->addItem("");

   // connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(removeSubTab(int))); //接受删除信号，删除tab页面
    ui->tabWidget->setMovable(true); //设置tab页面可拖动
    ui->tabWidget->clear();
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);
}

MainInterface::~MainInterface()
{
    delete ui;
}

void MainInterface::mousePressEvent(QMouseEvent *event)
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
void MainInterface::mouseMoveEvent(QMouseEvent *event)
{
    if (m_moving && (event->buttons() && Qt::LeftButton == true)
        && (event->globalPos()-m_lastPos).manhattanLength() > QApplication::startDragDistance())
    {
        move(event->globalPos()-m_lastPos);
        m_lastPos = event->globalPos() - pos();
    }
    return QWidget::mouseMoveEvent(event);
}
void MainInterface::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() && Qt::LeftButton ==false)
        m_moving=false;
}

void MainInterface::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
        on_pushButton_delete_clicked();
}
void MainInterface::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
        on_pushButton_delete_clicked();
}

void MainInterface::mainReadyRead()
{
    QByteArray rcvBuf = fd->readAll();
    QString rcvStr(rcvBuf);

    if(rcvStr.left(3)=="REC")
    {
        int index=rcvStr.indexOf("|");
        QString msginfo=rcvStr.right(rcvStr.length()-index-1);
        emit revMsgSig(msginfo);
    }

    if(rcvStr.left(3)=="FRI")
    {
        if(rcvStr.length()==4)
            return;
        QStringList friList = rcvStr.split("|");
        int frinum;
        frinum=friList.length();
        for(int i=1;i<frinum;i++)
        {
            ui->listWidget->addItem(friList[i]);
        }
    }

    if(rcvStr.left(3)=="ADF")
    {
        int index=rcvStr.indexOf("|");
        QString addname=rcvStr.right(rcvStr.length()-index-1);
        QMessageBox::information(this,"好友添加成功",addname+" 已经成为你的好友");
        ui->listWidget->addItem(addname);
    }

    if(rcvStr.left(3)=="REF")
    {
        int index=rcvStr.indexOf("|");
        QString refName=rcvStr.right(rcvStr.length()-index-1);
        QMessageBox::information(this,"好友添加失败",refName+" 拒绝了你的好友申请");
    }

    if(rcvStr.left(3)=="FRR")
    {
        int index=rcvStr.indexOf("|");
        QString applyName=rcvStr.right(rcvStr.length()-index-1);
        int flag=QMessageBox::question(this,"好友申请",applyName+" 申请添加你为好友,是否同意？",QMessageBox::Yes,QMessageBox::No);
        if(flag==QMessageBox::Yes)
        {
            ui->listWidget->addItem(applyName);
            QString yesMsg = "ATF|1|"+applyName+"|"+MyName;
            fd->write(yesMsg.toStdString().c_str());
        }
        else
        {
            QString noMsg = "ATF|0|"+applyName+"|"+MyName;
            fd->write(noMsg.toStdString().c_str());
        }
    }

    if(rcvStr.left(3)=="UOE")
    {
        int index=rcvStr.indexOf("|");
        QString errName=rcvStr.right(rcvStr.length()-index-1);
        emit addorDelErrSig(errName);
    }

    if(rcvStr.left(3)=="SFI")
    {
        ui->textEdit_friInfo->setText("");
        QStringList friIfo=rcvStr.split("|");
        curName=friIfo[1];
        ui->textEdit_friInfo->append("性别："+friIfo[2]);
        ui->textEdit_friInfo->append("");
        ui->textEdit_friInfo->append("生日："+friIfo[4]);
        ui->textEdit_friInfo->append("");
        ui->textEdit_friInfo->append("所在地："+friIfo[3]);
    }

    if(rcvStr.left(3)=="DDD")
    {
        ui->listWidget->clear();
        ui->listWidget->addItem("");
        ui->listWidget->addItem("");
        ui->listWidget->addItem("");
        if(rcvStr.length()==4)
            return;
        QStringList friList = rcvStr.split("|");
        int frinum;
        frinum=friList.length();
        delName=friList[1];
        for(int i=2;i<frinum;i++)
        {
            ui->listWidget->addItem(friList[i]);
        }

        QMap<QString,QWidget *>::iterator friStr=fri_map.find(delName);
        if(friStr==fri_map.end())
            return;
        delete friStr.value();
        fri_map.erase(friStr);
        if(chatnum>0)
            chatnum--;
        if(chatnum==0)
        {
            ui->tabWidget->setMaximumWidth(290);
            ui->tabWidget->setMinimumWidth(270);
            ui->listWidget->setMaximumWidth(760);
            ui->listWidget->setMinimumWidth(740);
        }
    }
}



void MainInterface::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item->text()=="")
        return;
    curName="";
    ui->textEdit_friInfo->setGeometry(730,190,0,0);
    ui->label->setGeometry(810,10,121,51);
    QString name=item->text();
    if(fri_map.find(name)==fri_map.end())
    {
        ChatUI *temp = new ChatUI(name,MyName);
        ui->tabWidget->insertTab(ui->tabWidget->currentIndex()+1,temp,"");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(temp));
        chatnum++;

        ui->tabWidget->setMinimumWidth(770);
        ui->listWidget->setMaximumWidth(260);
        ui->listWidget->setMinimumWidth(240);
        ui->tabWidget->setMaximumWidth(790);

        Portrait *pt = new Portrait(name);
        ((QTabBar* )ui->tabWidget->tabBar())->setTabButton(ui->tabWidget->currentIndex(), QTabBar::RightSide, pt);

        fri_map.insert(name,temp);
        clue.insert(ui->tabWidget->currentIndex(),name);

        connect(temp,SIGNAL(sendButClickSig(QString)),this,SLOT(sendMsg(QString)));
        connect(this,SIGNAL(revMsgSig(QString)),temp,SLOT(MsgOutPut(QString)));
        connect(temp,SIGNAL(closeTableSig()),this,SLOT(on_pushButton_delete_clicked()));
    }
    else
    {
        ui->textEdit_friInfo->setGeometry(730,190,0,0);
        ui->tabWidget->setMinimumWidth(770);
        ui->listWidget->setMaximumWidth(260);
        ui->listWidget->setMinimumWidth(240);
        ui->tabWidget->setMaximumWidth(790);
        ui->label->setGeometry(810,10,121,51);
    }
}

void MainInterface::sendMsg(QString txt)
{
    fd->write(txt.toStdString().c_str());
}

void MainInterface::on_pushButton_delete_clicked()
{
     QMap<int,QString>::iterator clueStr=clue.find(ui->tabWidget->currentIndex());
     if(clueStr==clue.end())
         return;
     QMap<QString,QWidget *>::iterator friStr=fri_map.find(clueStr.value());
     delete friStr.value();
     fri_map.erase(friStr);
     clue.erase(clueStr);
     if(chatnum>0)
         chatnum--;
     if(chatnum==0)
     {
         ui->tabWidget->setMaximumWidth(290);
         ui->tabWidget->setMinimumWidth(270);
         ui->listWidget->setMaximumWidth(760);
         ui->listWidget->setMinimumWidth(740);

     }

}

void MainInterface::on_pushButton_addordelete_clicked()
{
    af = new AddFriend(MyName);
    connect(af,SIGNAL(addFriendSig(QString)),this,SLOT(sendAddApply(QString)));
    connect(af,SIGNAL(delFriendSig(QString)),this,SLOT(sendDelApply(QString)));
    connect(this,SIGNAL(addorDelErrSig(QString)),af,SLOT(sendErr(QString)));
    af->show();
}

void MainInterface::sendAddApply(QString Msg)
{
    fd->write(Msg.toStdString().c_str());
}

void MainInterface::sendDelApply(QString Msg)
{
    fd->write(Msg.toStdString().c_str());
}

void MainInterface::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString name=item->text();
    QString msg="GFI|"+name;
    if(curName=="")
    {
        ui->tabWidget->setMaximumWidth(290);
        ui->tabWidget->setMinimumWidth(270);
        ui->listWidget->setMaximumWidth(760);
        ui->listWidget->setMinimumWidth(740);
        ui->label->setGeometry(300,10,121,51);
        ui->textEdit_friInfo->setGeometry(730,190,250,330);
    }
    if(curName==name)
    {
        ui->textEdit_friInfo->setGeometry(730,190,0,0);
        ui->tabWidget->setMinimumWidth(770);
        ui->listWidget->setMaximumWidth(260);
        ui->listWidget->setMinimumWidth(240);
        ui->tabWidget->setMaximumWidth(790);
        ui->label->setGeometry(810,10,121,51);
        curName="";
    }
    if(curName!=name and curName!="")
    {
        ui->tabWidget->setMaximumWidth(290);
        ui->tabWidget->setMinimumWidth(270);
        ui->listWidget->setMaximumWidth(760);
        ui->listWidget->setMinimumWidth(740);
        ui->label->setGeometry(300,10,121,51);
        ui->textEdit_friInfo->setGeometry(730,190,250,330);
    }
    fd->write(msg.toStdString().c_str());
}

void MainInterface::on_pushButton_clicked()
{
    af->close();
}
