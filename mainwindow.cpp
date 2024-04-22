#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NetworkDetect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupsystem();

    connect(ui->action_S,&QAction::triggered,this,&MainWindow::singleHelp);
    connect(ui->action_M,&QAction::triggered,this,&MainWindow::multiHelp);
    connect(ui->action_H,&QAction::triggered,this,&MainWindow::heatmapHelp);
    connect(ui->action_fast,&QAction::triggered,this,&MainWindow::editWaitTime_fast);
    connect(ui->action_routine,&QAction::triggered,this,&MainWindow::editWaitTime_routine);
    connect(ui->action_slow,&QAction::triggered,this,&MainWindow::editWaitTime_slow);
    connect(ui->action_O,&QAction::triggered,this,&MainWindow::on_BrowseButton_clicked);
    connect(ui->action_N,&QAction::triggered,this,&MainWindow::openTable);
    connect(ui->Output_line,&QAction::triggered,this,&MainWindow::outputLine);
    connect(ui->Output_hetmap,&QAction::triggered,this,&MainWindow::outputHeatMap);
    connect(ui->Output_data,&QAction::triggered,this,&MainWindow::outputData);
    // connect(myWidget,SIGNAL(&Form::customSignal()),this,SLOT(handleCustomSignal()));

    connect(ui->widget, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_BrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择一个文件",
                                                    QCoreApplication::applicationFilePath(),
                                                    "*");
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else
    {
        ui->FilelineEdit->setText(fileName);
    }

}

void MainWindow::on_getAccessToken(){

    ui->statuLable->setText("正在获取Token");

    //密钥放置位置
    QString apiKey = "ptph1iw537m59p5P7Pzuh1jm";
    QString secretKey = "COCZFa3Hvn84zsRlKl3ixGtVMLScl5kT";

    QNetworkRequest request;

    //http包格式设置
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("charset", "UTF-8");

    //url构建
    QUrl url("https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=" + apiKey + "&client_secret=" + secretKey);
    request.setUrl(url);

    QByteArray data;

    get_accessToken =  new QNetworkAccessManager(this);
    connect(get_accessToken, SIGNAL(finished(QNetworkReply*)), this, SLOT(accessTokenResult(QNetworkReply*)));
    get_accessToken->post(request, data);
}


void MainWindow::accessTokenResult(QNetworkReply* pReply){

    if(pReply->error() == QNetworkReply::NoError){
        QByteArray string = pReply->readAll();
        //获取内容
        QJsonParseError jsonErr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(string, &jsonErr);
        if (jsonDoc.isObject()){
            QJsonObject jsonObj = jsonDoc.object();
            accessToken = jsonObj.take("access_token").toString();
            qDebug()<<"access_token:"+accessToken;
            ui->statuLable->setText("获取Token成功，可以开始推测！");
            disconnect(getAccessToken);
        }
    }
}

void MainWindow::forcastResult(QNetworkReply* pReply){

    if(pReply->error() == QNetworkReply::NoError){
        // ui->textBrowser->setText("开始预测");
        QByteArray string = pReply->readAll();
        qDebug()<<string;
        //获取内容
        QJsonParseError jsonErr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(string, &jsonErr);
        if (jsonDoc.isObject()){
            // qDebug()<<"1";
            QJsonObject jsonObj = jsonDoc.object();
            QJsonArray batchResultArray = jsonObj.value("batch_result").toArray();
            QJsonObject firstElement = batchResultArray.at(0).toObject();
            double col5Value = firstElement.value("col5").toDouble();
            // qDebug() << "col5 value:" << col5Value;
            focastresult = QString::number(col5Value);
            ui->textBrowser->insertPlainText("预测拉力值： "+focastresult);
            ui->statuLable->setText("预测完成，可以输入下一次预测值，或者输入待预测文件。");
            disconnect(getForcastResult);
            // qDebug()<< focastresult;
        }
    }
    else{
        ui->statuLable->setText("网络出错，请检查网络！");
    }
}

void MainWindow::multiforcastResult(QNetworkReply* pReply){

    //返回没问题才开始处理
    if(pReply->error() == QNetworkReply::NoError){

        //读取所有返回值
        QByteArray string = pReply->readAll();
        qDebug()<<string;

        //获取内容
        QJsonParseError jsonErr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(string, &jsonErr);

        //如果返回的是json格式才继续
        if (jsonDoc.isObject()){

            // qDebug()<<"1";//测试是否跑到这的语句

            //获取预测值
            QJsonObject jsonObj = jsonDoc.object();
            QJsonArray batchResultArray = jsonObj.value("batch_result").toArray();
            QJsonObject firstElement = batchResultArray.at(0).toObject();
            double col5Value = firstElement.value("col5").toDouble();
            // qDebug() << "col5 value:" << col5Value;
            focastresult = QString::number(col5Value);

            //将预测值输出
            ui->textBrowser->insertPlainText("预测拉力值： "+focastresult);
            ui->statuLable->setText("预测完成，等待预测下一个值。");
            disconnect(getForcastResult);
            // qDebug()<< focastresult;

            //将预测值添加到表格中
            // Widget::series->append(num,focastresult.toFloat());
            series->append(num,focastresult.toFloat());

            // 获取 QChart 对象
            QChart *chart = ui->chartView->chart();
            // 获取当前的 X 轴对象
            QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axisX());
            // 修改 X 轴范围，例如设置范围为 [0, 20]
            axisX->setRange(1, num);
            // 获取当前的 Y 轴对象
            QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axisY());
            // 修改 X 轴范围，例如设置范围为 [0, 20]

            qreal maxValue = std::numeric_limits<qreal>::lowest();

            // 遍历系列中的数据点，找到最大值
            foreach (const QPointF &point, series->points()) {
                maxValue = std::max(maxValue, std::max(point.x(), point.y()));
            }

            axisY->setRange(0, maxValue+50);

            if( Xposision>=0 && Yposision >= 0 ){
                dynamicHeatmap(Xposision,Yposision,col5Value);
            }
            num++;
        }
    }
}

//这个函数控制了点击推断后的程序的主要逻辑
void MainWindow::on_SingleButton_clicked()
{
    ui->SingleButton->setDisabled(true);
    QString Fire = ui->FirelineEdit->text();
    QString Draw = ui->DrawlineEdit->text();
    QString Height= ui->HeightlineEdit->text();
    QString dB= ui->dBlineEdit->text();
    QString filePath = ui->FilelineEdit->text();

    ui->FirelineEdit->clear();
    ui->DrawlineEdit->clear();
    ui->HeightlineEdit->clear();
    ui->dBlineEdit->clear();
    ui->FilelineEdit->clear();

    if((Fire.isEmpty() && Draw.isEmpty() && Height.isEmpty() && dB.isEmpty() && !filePath.isEmpty()) ||
        (filePath.isEmpty() && !Fire.isEmpty() && !Draw.isEmpty() && !Height.isEmpty() && !dB.isEmpty())){

        auto *colorMap = static_cast<QCPColorMap *>(ui->widget->plottable(0));
        for (int x = 0; x < 20; ++x) {
            for (int y = 0; y < 5; ++y) {
                colorMap->data()->setAlpha(x, y, 0);  // z为0，设置为透明
            }
        }

        // URL
        QString baseUrl = BASEURL;
        QUrl url(baseUrl);

        // 设置url参数
        QUrlQuery query;
        query.addQueryItem("access_token",accessToken);
        url.setQuery(query);

        // 构造请求
        QNetworkRequest request;
        request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("Content-Type:application/json;charset=UTF-8"));
        request.setUrl(url);

        //与网站通信的对象
        get_forcastResult =  new QNetworkAccessManager(this);

        //单次推断的情况
        if(filePath.isEmpty()){

            if(flag==1){
                ui->textBrowser->clear();
                series->clear();
            }
            flag=0;

            bool enFire,enDraw,enHeight,endB;
            Fire.toDouble(&enFire);
            Draw.toDouble(&enDraw);
            Height.toDouble(&enHeight);
            dB.toDouble(&endB);
            if(enFire && enDraw && enHeight && endB){

                connect(get_forcastResult, SIGNAL(finished(QNetworkReply*)), this, SLOT(forcastResult(QNetworkReply*)));
                // 表单数据
                QJsonObject jsonBody;
                QJsonObject dataObject;
                QJsonArray dataArray;
                dataObject["col1"] = Fire;
                dataObject["col2"] = Draw;
                dataObject["col3"] = Height;
                dataObject["col4"] = dB;
                dataArray.append(dataObject);
                jsonBody["data"] = dataArray;
                QJsonDocument jsonDocument(jsonBody);
                QByteArray postData = jsonDocument.toJson();

                // 发送请求
                get_forcastResult->post(request, postData);
                ui->statuLable->setText("预测中，请稍等。");
                ui->textBrowser->append("基准灵敏度(dB): "+Fire+";耦合灵敏度(dB): "+Draw+";波幅： "+Height+"%;dB值： "+dB+";");

            }
            else{
                QMessageBox::warning(this,"警告","错误的输入值");
            }

        }

        //多次推断的情况
        else{

            if(flag==0){
                ui->textBrowser->clear();
            }
            flag=1;

            ui->BrowseButton->setDisabled(true); //关闭浏览文件功能
            ui->textBrowser->clear(); //去除浏览框的路径内容

            series->clear();//清除序列剩余数值

            //连接网络回传信号，并响应进行处理
            connect(get_forcastResult, SIGNAL(finished(QNetworkReply*)), this, SLOT(multiforcastResult(QNetworkReply*)));

            //打开文件
            ui->textBrowser->append("待预测文件路径："+filePath);
            QFile file(filePath);
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QMessageBox::warning(this,"警告","该文件无法读取！");
            }
            //打开后开始处理
            else{

                QJsonArray dataArray;
                QTextStream in(&file);

                while (!in.atEnd())
                {

                    QString line = in.readLine();
                    QStringList values = line.split(',', Qt::SkipEmptyParts);

                    if (values.size() == 4)
                    {
                        bool enValue1,enValue2,enValue3,enValue4;
                        values[0].toDouble(&enValue1);
                        values[1].toDouble(&enValue2);
                        values[2].toDouble(&enValue3);
                        values[3].toDouble(&enValue4);
                        if(enValue1 && enValue2 && enValue3 && enValue4){

                            QJsonObject jsonBody;
                            QJsonObject dataObject;
                            QJsonArray dataArray;
                            dataObject["col1"] = values[0];
                            dataObject["col2"] = values[1];
                            dataObject["col3"] = values[2];
                            dataObject["col4"] = values[3];
                            dataArray.append(dataObject);
                            jsonBody["data"] = dataArray;
                            QJsonDocument jsonDocument(jsonBody);
                            QByteArray postData = jsonDocument.toJson();

                            // 发送请求
                            get_forcastResult->post(request, postData);
                            ui->statuLable->setText("预测中，请稍等。");
                            ui->textBrowser->append("基准灵敏度(dB): "+values[0]+";耦合灵敏度(dB): "+values[1]+";波幅： "+values[2]+"%;dB值： "+values[3]+";");

                            connect(get_forcastResult, SIGNAL(finished(QNetworkReply*)), this, SLOT(loopEvent()));

                            loop.exec();

                            QEventLoop loop1;

                            QTimer timer;
                            timer.setSingleShot(true);
                            timer.start(waitTime);
                            QObject::connect(&timer, &QTimer::timeout, [&]() {
                                loop1.quit();  // 退出事件循环
                            });
                            loop1.exec();

                        }
                        else{
                            QString prompt = "第"+QString::number(num)+"行存在内容错误，出现非数值!";
                            QMessageBox::warning(this,"警告",prompt);
                            ui->textBrowser->clear();
                            series->clear();
                            break;
                        }
                    }
                    else if(values.size() == 6){

                        bool enValue1,enValue2,enValue3,enValue4,enValue5,enValue6;
                        values[0].toInt(&enValue1);
                        values[1].toInt(&enValue2);
                        values[2].toDouble(&enValue3);
                        values[3].toDouble(&enValue4);
                        values[4].toDouble(&enValue5);
                        values[5].toDouble(&enValue6);
                        if(enValue1 && enValue2 && enValue3 && enValue4 && enValue5 &&enValue6 && (values[0].toInt()>=0) && (values[1].toInt()>=0)){

                            Xposision=values[0].toInt();
                            Yposision=values[1].toInt();
                            QJsonObject jsonBody;
                            QJsonObject dataObject;
                            QJsonArray dataArray;
                            dataObject["col1"] = values[2];
                            dataObject["col2"] = values[3];
                            dataObject["col3"] = values[4];
                            dataObject["col4"] = values[5];
                            dataArray.append(dataObject);
                            jsonBody["data"] = dataArray;
                            QJsonDocument jsonDocument(jsonBody);
                            QByteArray postData = jsonDocument.toJson();

                            // 发送请求
                            get_forcastResult->post(request, postData);
                            ui->statuLable->setText("预测中，请稍等。");
                            // ui->textBrowser->append("("+QString::number(values[0].toInt()+1)+","+QString::number(values[1].toInt()+1)+");基准灵敏度(dB): "
                            //                         +values[2]+";耦合灵敏度(dB): "+values[3]+";波幅： "+values[4]+"%;dB值： "+values[5]+";");
                            ui->textBrowser->append("基准灵敏度(dB): "
                                                    +values[2]+";耦合灵敏度(dB): "+values[3]+";波幅： "+values[4]+"%;dB值： "+values[5]+";");

                            connect(get_forcastResult, SIGNAL(finished(QNetworkReply*)), this, SLOT(loopEvent()));

                            loop.exec();

                            QEventLoop loop2;

                            QTimer timer;
                            timer.setSingleShot(true);
                            timer.start(waitTime);
                            QObject::connect(&timer, &QTimer::timeout, [&]() {
                                loop2.quit();  // 退出事件循环
                            });
                            loop2.exec();

                        }
                        else{
                            QString prompt = "第"+QString::number(num)+"行存在内容错误，出现非数值!";
                            QMessageBox::warning(this,"警告",prompt);
                            ui->textBrowser->clear();
                            series->clear();
                            break;
                        }
                    }
                    else
                    {
                        ui->textBrowser->append("第"+QString::number(num)+"行格式错误，请检查!");
                        num++;
                    }
                }

                //num用来记录处理到第几行
                num=1;
                file.close();
                series->setName("拉力预测值");

                Xposision=-1;
                Yposision=-1;


                //预测结束
                ui->statuLable->setText("预测完成，可以输入下一次预测值，或者输入待预测文件。");
                ui->BrowseButton->setEnabled(true);
            }
        }
    }
    else{
        QMessageBox::warning(this,"警告","请输入一条完整数据在上方，或是单独导入数据文件！");
    }
    ui->SingleButton->setEnabled(true);
}

void MainWindow::setupHeatmap(QCustomPlot *customPlot)
{
    QVector<QString> Xaxis = {"1", "2", "3", "4", "5", "6", "7",
                              "9", "10", "11","12","13",
                              "14", "15", "16", "17", "18", "19",
                              "20"};//横坐标lable
    QVector<QString> Yaxis = {"1", "2", "3",
                              "4", "5"};//纵坐标lable

    QVector<QVector<double>> data = {{0,0,0},{0,1,0},{0,2,0},{0,3,0},{0,4,0},{0,5,0},{0,6,0},{0,7,0},{0,8,0},{0,9,0},{0,10,0},{0,11,0},{0,12,0},{0,13,0},{0,14,0},{0,15,0},{0,16,0},{0,17,0},{0,18,0},
                                     {0,19,0},{0,20,0},{1,0,0},{1,1,0},{1,2,0},{1,3,0},{1,4,0},{1,5,0},{1,6,0},{1,7,0},{1,8,0},{1,9,0},{1,10,0},{1,11,0},{1,12,0},{1,13,0},{1,14,0},{1,15,0},
                                     {1,16,0},{1,17,0},{1,18,0},{1,19,0},{2,0,0},{2,1,0},{2,2,0},{2,3,0},{2,4,0},{2,5,0},{2,6,0},{2,7,0},{2,8,0},{2,9,0},{2,10,0},{2,11,0},{2,12,0},
                                     {2,13,0},{2,14,0},{2,15,0},{2,16,0},{2,17,0},{2,18,0},{2,19,0},{3,0,0},{3,1,0},{3,2,0},{3,3,0},{3,4,0},{3,5,0},{3,6,0},{3,7,0},{3,8,0},
                                     {3,9,0},{3,10,0},{3,11,0},{3,12,0},{3,13,0},{3,14,0},{3,15,0},{3,16,0},{3,17,0},{3,18,0},{3,19,0},{4,0,0},{4,1,0},{4,2,0},{4,3,0},{4,4,0},
                                     {4,5,0},{4,6,0},{4,7,0},{4,8,0},{4,9,0},{4,10,0},{4,11,0},{4,12,0},{4,13,0},{4,14,0},{4,15,0},{4,16,0},{4,17,0},{4,18,0},{4,19,0}
                                     };

    QCPColorMap *heatmap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);  // 构造一个颜色图
    heatmap->data()->setSize(Xaxis.size(), Yaxis.size());   // 设置颜色图数据维度，其内部维护着一个一维数组（一般表现为二维数组），这里可以理解为有多少个小方块
    heatmap->data()->setRange(QCPRange(0.5, Xaxis.size() - 0.5), QCPRange(0.5, Yaxis.size() - 0.5));  // 颜色图在x、y轴上的范围

    // 设置轴的显示，这里使用文字轴
    QSharedPointer<QCPAxisTickerText> xTicker(new QCPAxisTickerText);
    QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);//刻度文本
    xTicker->setTicks(labelPositions(Xaxis, 0.5), Xaxis);
    yTicker->setTicks(labelPositions(Yaxis, 0.5), Yaxis);//刻度位置设置
    xTicker->setSubTickCount(1);
    yTicker->setSubTickCount(1);//子刻度数目
    customPlot->xAxis->setTicker(xTicker);
    customPlot->yAxis->setTicker(yTicker);
    customPlot->xAxis->grid()->setPen(Qt::NoPen);
    customPlot->yAxis->grid()->setPen(Qt::NoPen);//不显示网格线
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);//显示子网络
    customPlot->xAxis->setSubTicks(true);
    customPlot->yAxis->setSubTicks(true);//子刻度显示
    customPlot->xAxis->setTickLength(0);
    customPlot->yAxis->setTickLength(0);//刻度线长度为0
    customPlot->xAxis->setSubTickLength(6);
    customPlot->yAxis->setSubTickLength(6);//子刻度线长度为6
    customPlot->xAxis->setRange(0, Xaxis.size());
    customPlot->yAxis->setRange(0, Yaxis.size());//XY轴范围设定

    //设置Z值来显示颜色
    for (int x = 0; x < Xaxis.size(); ++x) {
        for (int y = 0; y < Yaxis.size(); ++y) {
            int z = data.at(Xaxis.size() * y + x).at(2);
            if (z) heatmap->data()->setCell(x, y, z);     // 如果z不为0，则设置颜色值的位置
            else heatmap->data()->setAlpha(x, y, 0);  // z为0，设置为透明
        }
    }

    QCPColorScale *colorScale = new QCPColorScale(customPlot);  // 构造一个色条
    colorScale->setType(QCPAxis::atBottom);   // 水平显示
    customPlot->plotLayout()->addElement(1, 0, colorScale); // 在颜色图下面显示，色条被放置在布局的第一行第一列
    heatmap->setColorScale(colorScale);
    QCPColorGradient gradient;  // 色条使用的颜色渐变
    gradient.setColorStopAt(0.0, QColor("#0000ff"));   // 设置色条开始时的颜色f6efa6
    gradient.setColorStopAt(0.25, QColor("#87CEEB"));
    gradient.setColorStopAt(0.375, QColor("#87cefa"));
    gradient.setColorStopAt(0.5, QColor("#ffff00"));
    gradient.setColorStopAt(1.0, QColor("#ff0000"));  // 设置色条结束时的颜色bf444c
    heatmap->setGradient(gradient);
    // heatmap->rescaleDataRange();        // 自动计算数据范围，数据范围决定了哪些数据值映射到QCPColorGradient的颜色渐变当中
    heatmap->setDataRange(QCPRange(0, 200));     // 手动设置数据范围
    heatmap->setInterpolate(false);         // 设置是否使用插值

    // 保持色条与轴矩形边距一致
    QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    colorScale->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

}

QVector<double> MainWindow::labelPositions(const QVector<QString> &labels, double offset)
{
    QVector<double> result(labels.size());
    for (int i = 0; i < labels.size(); ++i)
        result[i] = i + offset;
    return result;
}

void MainWindow::dynamicHeatmap(int x , int y , double z)
{
    auto *colorMap = static_cast<QCPColorMap *>(ui->widget->plottable(0));
    colorMap->data()->setCell(x, y, z);
    colorMap->data()->setAlpha(x,y,1000);
    if(z<100){
        QCPItemLine *line1 = new QCPItemLine(customPlot);
        line1->start->setCoords(x+0.25, y+0.25);
        line1->end->setCoords(x+0.75, y+0.75);
        line1->setPen(QPen(Qt::red));

        QCPItemLine *line2 = new QCPItemLine(customPlot);
        line2->start->setCoords(x+0.25, y+0.75);
        line2->end->setCoords(x+0.75, y+0.25);
        line2->setPen(QPen(Qt::red));
    }

    ui->widget->replot();
}

void MainWindow::loopEvent(){
    loop.quit();
}

void MainWindow::setupsystem(){
    this->setWindowTitle("超声检测管道性能预测系统");
    ui->statuLable->setText("系统初始化中");
    this->on_getAccessToken();
    ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);

    //表格序列对象创建
    series = new QLineSeries();
    series->setName("拉力预测值/N");
    // series->append(0,0);

    //表格处理
    QChart* chart = ui->chartView->chart();     // 获取QchartView内置的chart
    chart->addSeries(series);                   // 将创建好的折线图series添加进chart中
    // 创建 X 轴和 Y 轴
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;
    // 将坐标轴添加到图表中
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    // 将折线图关联到坐标轴
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    // 动态调整 X 轴范围
    axisX->setRange(1, 2);
    // 动态调整 Y 轴范围，假设 Y 轴的数据范围在 [0, 10]
    axisY->setRange(0, 100);
    chart->setTitle("超声数据预测");            // 设置标题
    ui->chartView->setRenderHint(QPainter::Antialiasing);  // 设置抗锯齿

    //网络状态检测
    NetworkDetect* networkDetector = new NetworkDetect();
    QObject::connect(networkDetector, &NetworkDetect::sig_netStatusChanged, [=](bool state) {
        qDebug() << "Network Status Changed: " << (state ? "Online" : "Offline");

        if (!state && !networkDetector->isDialogVisible()) {
            // 在主线程中弹出提示框
            QMetaObject::invokeMethod(QCoreApplication::instance(), [=]() {
                    networkDetector->setDialogVisible(true);  // 设置对话框显示状态
                    QMessageBox::information(nullptr, "提示", "网络已掉线，请检查网络连接。", QMessageBox::Ok);

                    // 用户关闭提示框后执行一些操作，可以在这里添加你的代码
                    qDebug() << "User closed the message box. Performing additional operations...";
                    this->on_getAccessToken();
                    networkDetector->setDialogVisible(false);  // 重置对话框显示状态
                }, Qt::QueuedConnection);
        }
    });
    networkDetector->start();

    customPlot=ui->widget;//指针方便使用widget

    setupHeatmap(customPlot);
}

void MainWindow::singleHelp()
{
    // 创建消息框
    QMessageBox helpBox;

    // 设置消息框的图标为帮助图标
    helpBox.setIcon(QMessageBox::Information);

    // 设置消息框的标题
    helpBox.setWindowTitle("帮助信息");

    // 设置消息框的文本内容
    helpBox.setText("请分别输入四个参数进行推断。");

    // 添加一个“确定”按钮
    helpBox.addButton(QMessageBox::Ok);

    // 显示消息框
    helpBox.exec();
}

void MainWindow::multiHelp()
{
    // 创建消息框
    QMessageBox helpBox;

    // 设置消息框的图标为帮助图标
    helpBox.setIcon(QMessageBox::Information);

    // 设置消息框的标题
    helpBox.setWindowTitle("帮助信息");

    // 设置消息框的文本内容
    helpBox.setText("多次推断时，生成相应的拉力预测值折线图。");

    // 添加详细信息
    helpBox.setDetailedText("目前支持输入txt文件或csv文件进行推断，输入格式为：\n"
                            "基准灵敏度，耦合灵敏度，波幅，dB值。\n"
                            "一条数据对应一行。");

    // 添加一个“确定”按钮
    helpBox.addButton(QMessageBox::Ok);

    // 显示消息框
    helpBox.exec();
}

void MainWindow::heatmapHelp()
{
    // 创建消息框
    QMessageBox helpBox;

    // 设置消息框的图标为帮助图标
    helpBox.setIcon(QMessageBox::Information);

    // 设置消息框的标题
    helpBox.setWindowTitle("帮助信息");

    // 设置消息框的文本内容
    helpBox.setText("整体推断时，生成相应的拉力预测值折线图与热力图。");

    // 添加详细信息
    helpBox.setDetailedText("目前支持输入txt文件或csv进行推断，输入格式为：\n"
                            "X轴坐标，Y轴坐标，基准灵敏度，耦合灵敏度，波幅，dB值。\n"
                            "一条数据对应一行。");

    // 添加一个“确定”按钮
    helpBox.addButton(QMessageBox::Ok);

    // 显示消息框
    helpBox.exec();
}

void MainWindow::editWaitTime_fast(){
    waitTime=1000;
    QMessageBox::information(nullptr, "提示", "已将推断速度更改为快速", QMessageBox::Ok);
}

void MainWindow::editWaitTime_routine(){
    waitTime=2000;
    QMessageBox::information(nullptr, "提示", "已将推断速度更改为常规", QMessageBox::Ok);
}

void MainWindow::editWaitTime_slow(){
    waitTime=5000;
    QMessageBox::information(nullptr, "提示", "已将推断速度更改为缓慢", QMessageBox::Ok);
}

void MainWindow::openTable(){
    Form *tableWindow =new Form;
    tableWindow->show();
}

void MainWindow::handleCustomSignal(){
    qDebug()<<"1";
}
// 功能已经拆分，测试无问题后可删除
// void MainWindow::outputPix(){
//     // 设置默认文件格式
//     QString defaultFileFormat = "png";

//     // 设置默认文件名
//     QString defaultFileName = "chart_image";

//     // 合并默认文件名和格式
//     QString defaultFileFullName = defaultFileName + "." + defaultFileFormat;

//     // 打开保存文件对话框
//     QString fileName = QFileDialog::getSaveFileName(
//         nullptr,
//         "选择一个文件",
//         defaultFileFullName,
//         "All Files (*)"
//         );

//     if(fileName.isEmpty())
//     {
//         QMessageBox::warning(this,"警告","请选择一个文件");
//     }
//     else{
//         QPixmap chartImage = ui->chartView->grab();
//         chartImage.save(fileName);
//     }

//     defaultFileFormat = "png";
//     defaultFileName = "heatMap_image";

//     // 合并默认文件名和格式
//     defaultFileFullName = defaultFileName + "." + defaultFileFormat;

//     // 打开保存文件对话框
//     fileName = QFileDialog::getSaveFileName(
//         nullptr,
//         "选择一个文件",
//         defaultFileFullName,
//         "All Files (*)"
//         );

//     if(fileName.isEmpty())
//     {
//         QMessageBox::warning(this,"警告","请选择一个文件");
//     }
//     else{
//         QPixmap pixmap = ui->widget->toPixmap();
//         pixmap.save(fileName);
//     }
// }

void MainWindow::onMouseMove(QMouseEvent* event)
{
    auto *colorMap = static_cast<QCPColorMap *>(ui->widget->plottable(0));
    // 获取鼠标的图形坐标
    double x = ui->widget->xAxis->pixelToCoord(event->pos().x());
    double y = ui->widget->yAxis->pixelToCoord(event->pos().y());

    // 获取鼠标所在位置的数据值
    double z = colorMap->data()->cell(x, y);

    // 在控制台输出或在 GUI 中显示 z 的值
    ui->label_5->setAlignment(Qt::AlignCenter);
    ui->label_5->setStyleSheet("font-size: 14pt;");
    ui->label_5->setText("-------------------在（" + QString::number(x) +  ", " +QString::number(y) + ") 处的值为：" + QString::number(z) + "-------------------");
    // qDebug() << "Z value at (" << x << ", " << y << ") is " << z;
}

void MainWindow::outputLine(){
    // 设置默认文件格式
    QString defaultFileFormat = "png";

    // 设置默认文件名
    QString defaultFileName = "chart_image";

    // 合并默认文件名和格式
    QString defaultFileFullName = defaultFileName + "." + defaultFileFormat;

    // 打开保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "选择一个文件",
        defaultFileFullName,
        "All Files (*)"
        );

    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else{
        QPixmap chartImage = ui->chartView->grab();
        chartImage.save(fileName);
    }
}
void MainWindow::outputHeatMap(){
    // 设置默认文件格式
    QString defaultFileFormat = "png";

    // 设置默认文件名
    QString defaultFileName = "heatMap_image";

    // 合并默认文件名和格式
    QString defaultFileFullName = defaultFileName + "." + defaultFileFormat;

    // 打开保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "选择一个文件",
        defaultFileFullName,
        "All Files (*)"
        );

    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else{
        QPixmap pixmap = ui->widget->toPixmap();
        pixmap.save(fileName);
    }
}
void MainWindow::outputData(){
    QString text = ui->textBrowser->toPlainText();
    text=text.mid(text.indexOf("基"));
    QStringList lines = text.split("\n", Qt::SkipEmptyParts);

    QList<QStringList> values;
    QStringList value;

    foreach (const QString &line, lines) {
        QRegularExpression re("\\d+\\.?\\d*");
        QRegularExpressionMatchIterator matchIterator = re.globalMatch(line);
        value.clear();
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            value << match.captured();
        }
        values << value;
    }

    // 设置默认文件格式
    QString defaultFileFormat = "csv";

    // 设置默认文件名
    QString defaultFileName = "outputData";

    // 合并默认文件名和格式
    QString defaultFileFullName = defaultFileName + "." + defaultFileFormat;

    // 打开保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "选择一个文件",
        defaultFileFullName,
        "All Files (*)"
        );

    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else{
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this,"警告","无法保存为该文件，请检查！");
        }
        QTextStream out(&file);
        out<<"Reference Sensitivity"<<","<<"Coupling Sensitivity"<<","<<"Amplitude"<<","<<"dB Value"<<","<<"Predicted Value"<<"\n";
        foreach (const QStringList &row, values) {
            // 将一行数据转换成 CSV 格式，并写入文件
            QString line;
            foreach(const QString &num, row){
                line +=num+",";
            }
            line=line.chopped(1);
            out<<line<<"\n";
            // out << row.join(",") << "\n";
        }
        file.close();
    }

    // qDebug()<<text;
}
