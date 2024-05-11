#include "NetworkDetect.h"

NetworkDetect::NetworkDetect()
{
    flagRunning = true;
    dialogVisible = false;
}

void NetworkDetect::run()
{
    while (flagRunning)
    {
        QNetworkAccessManager manager;
        QNetworkProxy proxy(QNetworkProxy::NoProxy);
        manager.setProxy(proxy);
        QNetworkRequest request(QUrl("http://www.baidu.com"));
        QNetworkReply *reply = manager.get(request);

        // 等待响应完成
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        //qDebug() << "HTTP Status Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        //qDebug() << "HTTP Reason Phrase:" << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        if (reply->error() == QNetworkReply::NoError)
        {
            emit sig_netStatusChanged(true);  // 在线
        }
        else
        {
            emit sig_netStatusChanged(false); // 离线
        }

        delete reply;

        sleep(1);  // 加入睡眠以降低 CPU 使用率
    }
}

bool NetworkDetect::isDialogVisible() const {
    return dialogVisible;
}

void NetworkDetect::setDialogVisible(bool visible) {
    dialogVisible = visible;
}

void NetworkDetect::stop()
{
    flagRunning = false;
}
