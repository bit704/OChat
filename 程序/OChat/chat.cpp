#include "chat.h"
#include "ui_chat.h"

Chat::Chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::Chat(QTcpSocket *sock,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);

    fd = sock;

    connect(fd,SIGNAL(readyRead()),this,SLOT(mainReadyRead()));

    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(removeSubTab(int))); //接受删除信号，删除tab页面
    ui->tabWidget->setMovable(true); //设置tab页面可拖动
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);
}

Chat::~Chat()
{
    delete ui;
}
