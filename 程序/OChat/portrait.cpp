#include "portrait.h"
#include "ui_portrait.h"

Portrait::Portrait(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Portrait)
{
    ui->setupUi(this);
}

Portrait::Portrait(QString name,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Portrait)
{
    ui->setupUi(this);
    QString img = ":/new/prefix1/photo/";
        if(name == "小红")
        {
            img += "1.png";
        }
        else if(name == "小明")
        {
            img += "2.png";
        }
        else if(name == "小徐")
        {
            img += "3.png";
        }
        else if(name == "小王")
        {
             img += "4.png";
        }
        else if(name == "小刘")
        {
             img += "5.png";
        }

        ui->label_username->setText(name);
        ui->label_portrait->setPixmap(QPixmap(img));
        QString styleSheet = "border-radius: 0px;  border:1px solid black; background:white";
        ui->label_portrait->setStyleSheet(styleSheet);


}

Portrait::~Portrait()
{
    delete ui;
}
