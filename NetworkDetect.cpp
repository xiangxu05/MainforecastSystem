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
        QNetworkRequest request(QUrl("http://www.baidu.com"));
        QNetworkReply *reply = networkManager.get(request);

        // 等待响应完成
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        qDebug() << "HTTP Status Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << "HTTP Reason Phrase:" << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

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


// void NetworkDetect::run()
// {
//     QString network_cmd = "ping www.baidu.com -w 500 -n 5";
//     QString result;
//     network_process = new QProcess();    //不要加this
//     while(flagRunning)
//     {
//         network_process->start(network_cmd);   //调用ping 指令
//         network_process->waitForFinished();    //等待指令执行完毕
//         result = network_process->readAll();   //获取指令执行结果
//         qDebug() << "Ping Result:" << result;
//         qDebug() << "Ping Process Exit Code:" << network_process->exitCode();
//         //qDebug() << result;
//         if(result.contains(QString("TTL=")) || result.contains(QString("ttl=")))   //若包含TTL=字符串则认为网络在线
//         {
//             emit sig_netStatusChanged(true);  //在线
//         }
//         else
//         {
//             emit sig_netStatusChanged(false); //离线
//         }
//         sleep(2);  //加sleep降低CPU占用率
//     }
// }

void NetworkDetect::stop()
{
    flagRunning = false;
}
