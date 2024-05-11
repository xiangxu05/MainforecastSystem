#ifndef NETWORKDETECT_H
#define NETWORKDETECT_H
#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEvent>
#include <QEventLoop>
#include <QNetworkProxy>

class NetworkDetect : public QThread
{
    Q_OBJECT
public:
    NetworkDetect();

    virtual void run();
    void stop();

    bool isDialogVisible() const;
    void setDialogVisible(bool visible);
signals:
    void sig_netStatusChanged(bool state);

private:
    bool flagRunning;   //线程运行标志
    QProcess *network_process;
    QNetworkAccessManager networkManager;
    bool dialogVisible;

};


#endif // NETWORKDETECT_H
