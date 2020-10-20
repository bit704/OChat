#ifndef CHATUI_H
#define CHATUI_H

#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QKeyEvent>

namespace Ui {
class ChatUI;
}

class ChatUI : public QWidget
{
    Q_OBJECT


protected:

    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void keyReleaseEvent(QKeyEvent *ev);

private:
    QString curName;
    QString MyName;

public:
    explicit ChatUI(QWidget *parent = nullptr);
    explicit ChatUI(QString name,QString myname,QWidget *parent = nullptr);
    ~ChatUI();

private slots:
    void on_pushButton_send_clicked();
    void MsgOutPut(QString);

signals :
    void sendButClickSig(QString);
    void closeTableSig();

private:
    Ui::ChatUI *ui;
};

#endif // CHATUI_H
