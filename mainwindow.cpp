#include "mainwindow.h"
#include "ui_mainwindow.h"

QNetworkReply *reply;
QNetworkAccessManager *manager;		//定义网络请求对象
QString CurVersion = "V1.1";	//定义当前软件的版本号
QString Version;
QFile *myfile;
QString file;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myfile = new QFile(this);
    manager = new QNetworkAccessManager(this);          //新建QNetworkAccessManager对象
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//关联信号和槽

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chkUpdate_clicked()
{
    QNetworkRequest quest;
    quest.setUrl(QUrl("http://info.sokestudio.online/software_update.json")); //包含最新版本软件的下载地址
    quest.setHeader(QNetworkRequest::UserAgentHeader,"RT-Thread ART");
    manager->get(quest);    //发送get网络请求
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    QString str = reply->readAll();//读取接收到的数据
    //    qDebug() << str;
    parse_UpdateJSON(str);
    //文件保存到本地
/*
    QFile file("software_update.json");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) //append 内容追加在文件后面
    {
        QMessageBox::critical(this, "错误", "文件打开失败，信息未写入", "确定");
        return;
    }
    QTextStream out(&file);
    out << str;     //输出到文件
    file.close();   //关闭文件
    qDebug() << "文件保存成功!";
//    file.remove();
//    qDebug() << "文件已经删除";
*/
    reply->deleteLater();               //销毁请求对象
}

int MainWindow::parse_UpdateJSON(QString str)
{
    //    QMessageBox msgBox;
    QJsonParseError err_rpt;
    QJsonDocument  root_Doc = QJsonDocument::fromJson(str.toUtf8(),&err_rpt);//字符串格式化为JSON
    if(err_rpt.error != QJsonParseError::NoError)
    {
//        qDebug() << "root格式错误";
        QMessageBox::critical(this, "检查失败", "服务器地址错误或JSON格式错误!");
        return -1;
    }
    if(root_Doc.isObject())
    {
        QJsonObject  root_Obj = root_Doc.object();   //创建JSON对象，不是字符串
        QJsonObject PulseValue = root_Obj.value("PulseSensor").toObject();
        QString Version = PulseValue.value("LatestVersion").toString();  //V1.0
        QString Url = PulseValue.value("Url").toString();        //https://wcc-blog.oss-cn-beijing.aliyuncs.com/QtUpdate/uFun_Pulse_v1.0.exe
        QString UpdateTime = PulseValue.value("UpdateTime").toString();
        QString ReleaseNote = PulseValue.value("ReleaseNote").toString();
        if(Version > CurVersion)
        {
            QString warningStr =  "检测到新版本!\n版本号：" + Version + "\n" + "更新时间：" + UpdateTime + "\n" + "更新说明：" + ReleaseNote;
            int ret = QMessageBox::warning(this, "检查更新",  warningStr, "去下载", "不更新");
            if(ret == 0)    //点击更新
            {
                reply = manager->get(QNetworkRequest(Url));
                //更新进度条
                connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64)));
                connect(reply,&QNetworkReply::readyRead,this,&MainWindow::doProcessReadyRead);                //可读
                connect(reply,&QNetworkReply::finished,this,&MainWindow::doProcessFinished);                  //结束
                //QStringList list = url.split("/");
                QString filename = "MyNewSoft";
                file = "D:/" + filename + Version + ".exe";
                myfile->setFileName(file);
                bool ret = myfile->open(QIODevice::WriteOnly|QIODevice::Truncate);    //创建文件
                if(!ret)
                {
                    QMessageBox::warning(this,"warning","打开失败");
                    return false;
                }
            }
        }
        else
            QMessageBox::information(this, "检查更新", "当前已经是最新版本!");
    }
    return 0;
}

void MainWindow::updateDataReadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    float i = bytesReceived * 1.0 / bytesTotal * 100;
    qDebug()<<i;
    ui->progressBar->setValue(i);
}

void MainWindow::doProcessReadyRead()
{
    while(!reply->atEnd())
        {
            QByteArray ba = reply->readAll();
            myfile->write(ba);
        }
}

void MainWindow::doProcessFinished()
{
    myfile->close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}
