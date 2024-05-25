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
#include "CalculationThread.h"
#define BASEURL "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/table_infer/MultipleUltra"

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

    //对热力图进行输出
    void dynamicHeatmap(int x , int y , double z);
    QVector<double> labelPositions(const QVector<QString> &labels, int interval, double offset);
    // QVector<double> labelPositions(const QVector<QString> &labels, double offset = 0);//为热力图重整结果格式

    QString fineMax(QVector<QString> rowData);

private slots:

    /*
     *
     * 这部分是按钮的相关函数
     *
    */
    void on_BrowseButton_clicked();//浏览按钮
    void on_SingleButton_clicked();//推断按钮

    /*
     *
     * 这部分是Table的相关函数
     *
    */
    void openTable();
    void inputTable();

    /*
     *
     * 这部分是输出数据的相关函数
     *
    */
    void outputHeatMap();
    void outputData();

    /*
     *
     * 显示鼠标所处位置的值
     *
    */
    void onMouseMove(QMouseEvent* event);

    /*
     *
     * 帮助信息
     *
    */
    void singleHelp();
    void multiHelp();
    void heatmapHelp();
private:
    //主界面
    Ui::MainWindow *ui;

    QVector<QVector<double>> results;

    bool flag=0;//用来标志是否预测完成

    void openActionSlot();

    QCustomPlot *customPlot;

    QEventLoop loop;

    int waitTime=2000;

    //输出文本内容
    QString statusOutput;

    QVector<QCPItemLine*> drawLines;//热力图中的线

    //表格对象
    Form *myWidget;
signals:
    void heatmapUpdateFinished();
};
#endif // MAINWINDOW_H
