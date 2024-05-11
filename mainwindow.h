#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QUrlQuery>
#include <QTimer>
#include <QtCharts>
#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include "qcustomplot.h"
#include "form.h"
#define BASEURL "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/table_infer/tensilV1"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //两个创建主窗口的方法
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //初始化设置方法
    void setupsystem();
    void setupHeatmap(QCustomPlot *customPlot);

    QVector<double> labelPositions(const QVector<QString> &labels, double offset = 0);//暂时不知道干嘛用的

    //对热力图进行输出
    void dynamicHeatmap(int x , int y , double z);

private slots:

    //浏览按钮
    void on_BrowseButton_clicked();

    void on_SingleButton_clicked();

    void on_getAccessToken();

    void accessTokenResult(QNetworkReply* pReply);

    void forcastResult(QNetworkReply* pReply);

    void multiforcastResult(QNetworkReply* pReply);

    void loopEvent();

    void singleHelp();

    void multiHelp();

    void heatmapHelp();

    void editWaitTime_slow();

    void editWaitTime_routine();

    void editWaitTime_fast();

    void openTable();

    // void outputPix();

    void outputHeatMap();

    void outputLine();

    void outputData();

    void onMouseMove(QMouseEvent* event);

private:
    //主界面
    Ui::MainWindow *ui;

    //AccessToken相关对象
    QMetaObject::Connection getAccessToken;
    QNetworkAccessManager *get_accessToken;
    QString accessToken;

    //预测结果相关对象
    QMetaObject::Connection getForcastResult;
    QNetworkAccessManager *get_forcastResult;
    QString focastresult;

    int num=1;

    QLineSeries* series;

    bool flag=0;

    void openActionSlot();

    QCustomPlot *customPlot;

    QEventLoop loop;

    int Xposision=-1;

    int Yposision=-1;

    int waitTime=2000;

    //表格对象
    Form *myWidget;

};
#endif // MAINWINDOW_H
