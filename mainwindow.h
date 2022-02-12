#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//网络相关头文件
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
//JSON相关头文件
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include<QMessageBox>
#include<QDesktopServices>
#include<QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int parse_UpdateJSON(QString str);

private slots:
    void on_chkUpdate_clicked();
    void replyFinished(QNetworkReply *reply);
    void updateDataReadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void doProcessReadyRead();
    void doProcessFinished();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
