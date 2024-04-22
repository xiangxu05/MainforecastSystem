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

    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void setupsystem();

    void setupHeatmap(QCustomPlot *customPlot);

    QVector<double> labelPositions(const QVector<QString> &labels, double offset = 0);

    void dynamicHeatmap(int x , int y , double z);


public slots:

    void handleCustomSignal();

private slots:

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

    Ui::MainWindow *ui;

    QMetaObject::Connection getAccessToken;

    QNetworkAccessManager *get_accessToken;

    QString accessToken;

    QMetaObject::Connection getForcastResult;

    QNetworkAccessManager *get_forcastResult;

    QString focastresult;//预测结果

    int num=1;

    QLineSeries* series;

    bool flag=0;

    void openActionSlot();

    QCustomPlot *customPlot;

    QEventLoop loop;

    int Xposision=-1;

    int Yposision=-1;

    int waitTime=2000;

    Form *myWidget;

};
#endif // MAINWINDOW_H
