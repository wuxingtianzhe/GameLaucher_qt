#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QString>
#include<QJsonDocument>
#include<QJsonObject>
#include<QDebug>
#include<QCryptographicHash>
#include<QtNetwork/QNetworkAccessManager>
#include<QtNetwork/QNetworkReply>
#include<QtNetwork/qnetworkrequest.h>
#include<QMessageBox>
#include <QDesktopServices>
#include<QProcess>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createroom_clicked()
{
    //1.取出用户名和密码
    QString username=ui->username->text();
    QString password=ui->password->text();
    //2.构建密码的签名
    QCryptographicHash md5_hash(QCryptographicHash::Md5);
    md5_hash.addData(password.toUtf8());
    QString md5_password(md5_hash.result().toHex());
    //3.拼登录请求(Json)
    //构建一个json对象，填入用户名和密码的签名值
    QJsonObject json;
    json["username"]=username;
    json["password"]=md5_password;
    //将json对象放到doc中
    QJsonDocument json_doc(json);

    QString output=json_doc.toJson();
    qDebug()<<output;
    //4.发给登录服务器
    QNetworkRequest req(QUrl("http://192.168.253.130:81/login/"));
    req.setHeader(QNetworkRequest::ContentLengthHeader,output.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
   m_reply=m_access_mng.post(req,output.toUtf8());
    //异步处理服务器的回复
    connect(m_reply,SIGNAL(finished()),this,SLOT(proc_login_reply()));
}
void MainWindow::proc_login_reply()
{
    //qDebug()<<m_reply->readAll();
    //判断登录结果
    QJsonDocument json_doc=QJsonDocument::fromJson(m_reply->readAll());
    QJsonObject json = json_doc.object();
    qDebug()<<json["login_result"].toString();
    if ("OK" == json["login_result"].toString())
    {

        qDebug()<<"login success";
        QProcess pro;
               QStringList argv;
               argv.push_back("192.168.253.130");
               argv.push_back("8899");
               pro.startDetached("client.exe",argv,".");
               qApp->exit();

    }
    else
    {
        //QMessageBox::information(this,"登录失败","用户名或者密码错误");
        QMessageBox::information(this,"登录失败","用户名或密码错误");
    }
}

void MainWindow::on_register_2_clicked()
{
  QDesktopServices::openUrl(QUrl("http://192.168.253.130:81"));
}
