#include "register.h"
#include "ui_register.h"
#include "widget.h"
#include "QMessageBox"
#include "uishadow.h"
#include <QRegExpValidator>
#include <user.h>
#include <protocol.h>

extern QString hostIp;
extern int hostPort;

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket;
    //restrictions on username and password: contains 1-10 letters or numbers
    QRegExp regex("^[A-Za-z0-9]{1,10}$");
    QRegExpValidator *uservalidator = new QRegExpValidator(regex, ui->regUsernameInput);
    QRegExpValidator *pswvalidator = new QRegExpValidator(regex, ui->regPwdInput);
    ui->regUsernameInput->setValidator(uservalidator);
    ui->regPwdInput->setValidator(pswvalidator);

    //create shadow on widget
    Shadow *shadow = new Shadow;
    shadow->applyToWidget(ui->widget);

}

Register::~Register()
{
delete ui;
}

void Register::on_confirmBtn_clicked()
{
    if(ui->regUsernameInput->text()!=""&& ui->regPwdInput->text()!=""&& ui->regRepeatPwdInput->text()!="")
    {
        if(ui->regPwdInput->text()==ui->regRepeatPwdInput->text())
        {
            tcpSocket->abort();//取消已有链接
            tcpSocket->connectToHost(hostIp, hostPort);//链接服务器

            if(!tcpSocket->waitForConnected(30000))
            {
                QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
                this->close();
//                user.islogin = false;
                Widget *cli = new Widget();
                cli->show();
            }
            else
            {//服务器连接成功
                Protocol sendinfo;
                sendinfo.function_name = QString("register").trimmed();
                sendinfo.username = ui->regUsernameInput->text();
                sendinfo.userpwd = ui->regPwdInput->text();
                sendinfo.sendStructData(tcpSocket,sendinfo);

                QString ip = tcpSocket->peerAddress().toString().section(":",3,3);
                int port = tcpSocket->peerPort();
                QString str = QString("[%1:%2]").arg(ip).arg(port);
                qDebug() << str ;

                connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                    Protocol recvinfo;
                    recvinfo.recvStructData(tcpSocket,recvinfo);

                    if(recvinfo.msg == QString("register successed"))
                    {//注册成功
                        this->close();
                        Widget *cli = new Widget();
                        cli->show();
                    }
                    else if(recvinfo.msg==QString("register error!!same_name"))
                    {
                        QMessageBox::warning(this, "Warning!", "昵称有重复", QMessageBox::Yes);
                        ui->regUsernameInput->clear();
                        ui->regUsernameInput->setFocus();
                    }

                });
            }
        }
        else
        {//两次密码不一样
            QMessageBox::warning(this, "Warning!", "两次密码不一样", QMessageBox::Yes);
            ui->regPwdInput->clear();
            ui->regRepeatPwdInput->clear();
            ui->regPwdInput->setFocus();
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning!", "用户名或密码不为空", QMessageBox::Yes);
        ui->regUsernameInput->clear();
        ui->regPwdInput->clear();
        ui->regRepeatPwdInput->clear();
        ui->regUsernameInput->setFocus();
    }
//    if(ui->regPwdInput->text() != ui->regRepeatPwdInput->text()){
//        QMessageBox::warning(this,tr("Warning"),tr("Differnet passwords"),QMessageBox::Yes);
//        ui->regPwdInput->clear();
//        ui->regRepeatPwdInput->clear();
//    }
//    //else if() username repetiton
//    else{
//        int id = 1001;
//        QString stringid = QString(id);
//        QString username = ui->regUsernameInput->text();
//        QString password = ui->regPwdInput->text();
//        //pass data to database

//        QMessageBox::information(this,tr("Note"),tr("Register successfully"));
//        this->close();
//        Widget *login = new Widget;
//        login->show();

//    }

}

void Register::on_cancelBtn_clicked()
{
    this->close();
    Widget *login = new Widget;
    login->show();

}
