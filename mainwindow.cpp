#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NetworkDetect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupsystem();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//以下是初始化界面的方法
void MainWindow::setupsystem(){
    this->setWindowTitle("超声检测管道性能预测系统 V2.0");
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
        //qDebug() << "Network Status Changed: " << (state ? "Online" : "Offline");
        if (!state && !networkDetector->isDialogVisible()) {
            // 在主线程中弹出提示框
            QMetaObject::invokeMethod(QCoreApplication::instance(), [=]() {
                    networkDetector->setDialogVisible(true);  // 设置对话框显示状态
                    QMessageBox::information(nullptr, "提示", "网络已掉线，请检查网络连接。", QMessageBox::Ok);
                    this->on_getAccessToken();
                    networkDetector->setDialogVisible(false);  // 重置对话框显示状态
                }, Qt::QueuedConnection);
        }
    });
    networkDetector->start();
    customPlot=ui->widget;//指针方便使用widget
    setupHeatmap(customPlot);
    connect(ui->action_S,&QAction::triggered,this,&MainWindow::singleHelp);
    connect(ui->action_M,&QAction::triggered,this,&MainWindow::multiHelp);
    connect(ui->action_H,&QAction::triggered,this,&MainWindow::heatmapHelp);
    connect(ui->action_fast,&QAction::triggered,this,&MainWindow::editWaitTime_fast);
    connect(ui->action_routine,&QAction::triggered,this,&MainWindow::editWaitTime_routine);
    connect(ui->action_slow,&QAction::triggered,this,&MainWindow::editWaitTime_slow);
    connect(ui->action_O,&QAction::triggered,this,&MainWindow::on_BrowseButton_clicked);
    connect(ui->action_N,&QAction::triggered,this,&MainWindow::openTable);
    connect(ui->action_D,&QAction::triggered,this,&MainWindow::inputTable);
    connect(ui->Output_line,&QAction::triggered,this,&MainWindow::outputLine);
    connect(ui->Output_hetmap,&QAction::triggered,this,&MainWindow::outputHeatMap);
    connect(ui->Output_data,&QAction::triggered,this,&MainWindow::outputData);
    connect(ui->widget, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);
}

void MainWindow::setupHeatmap(QCustomPlot *customPlot)
{

    // 设置横坐标和纵坐标的范围
    customPlot->xAxis->setRange(0, 5);
    customPlot->yAxis->setRange(0, 5);

    // 设置横坐标和纵坐标的刻度标签
    QVector<QString> Xaxis;
    QVector<QString> Yaxis;

    for (int i = 0; i < 5; ++i) {
        Xaxis.append(QString::number(i));
    }
    for (int i = 0; i < 5; ++i) {
        Yaxis.append(QString::number(i));
    }

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
            heatmap->data()->setAlpha(x, y, 0);// 如果z不为0，则设置颜色值的位置
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

//以下是几个按钮的方法
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

//这个函数控制了点击推断后的程序的主要逻辑
void MainWindow::on_SingleButton_clicked()
{
    ui->SingleButton->setDisabled(true);
    QString waveHight = ui->waveHightEdit->text();
    QString dB = ui->dBEdit->text();
    QString xStep= ui->xStepEdit->text();
    QString yStep= ui->yStepEdit->text();
    QString filePath = ui->FilelineEdit->text();
    if(waveHight.isEmpty() || dB.isEmpty() || xStep.isEmpty() || yStep.isEmpty() || filePath.isEmpty()){
        QMessageBox::warning(this,"警告","缺少相关参数或文件路径！");
        ui->SingleButton->setEnabled(true);
        return;
    }
    ui->textBrowser->clear();//清理历史处理记录
    ui->BrowseButton->setDisabled(true); //浏览按钮关闭

    series->clear();//清除序列剩余数值
    for (auto line : drawLines) {
        //ui->widget->removeItem(line);
        delete line;
    }
    // 移除完成后清空容器
    drawLines.clear();

    //网络通讯相关对象
    QString baseUrl = BASEURL;
    QUrl url(baseUrl);// URL
    QUrlQuery query;
    query.addQueryItem("access_token",accessToken);
    url.setQuery(query);// 设置url参数
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("Content-Type:application/json;charset=UTF-8"));
    request.setUrl(url);// 构造请求
    get_forcastResult =  new QNetworkAccessManager(this);//与网站通信的对象
    connect(get_forcastResult, SIGNAL(finished(QNetworkReply*)), this, SLOT(multiforcastResult(QNetworkReply*)));//连接网络回传信号，对响应进行处理

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
        //判断file是否为空
        //感觉可以暂时不写
        //初始化热力图
        auto *colorMap = static_cast<QCPColorMap *>(ui->widget->plottable(0));
        // 获取 colorMap 的数据
        QCPColorMapData *data = colorMap->data();
        // 遍历数据，并将 Alpha 值设置为 0
        for (int xIndex = 0; xIndex < data->keySize(); ++xIndex) {
            for (int yIndex = 0; yIndex < data->valueSize(); ++yIndex) {
                // 设置当前点的 Alpha 值为 0
                data->setAlpha(xIndex, yIndex, 0);
            }
        }
        int xMax = 5,yMax =5;
        QVector<QString> lines;
        while(!in.atEnd()){
            QString line = in.readLine();//读一行
            QStringList values = line.split(',', Qt::SkipEmptyParts);//以逗号分解
            xMax = std::max(xMax,values[0].toInt());
            yMax = std::max(yMax,values[1].toInt());
            lines.append(line);
        }
        //qDebug()<<xMax<<yMax;

        // 设置横坐标和纵坐标的范围
        ui->widget->xAxis->setRange(0, xMax+1);
        ui->widget->yAxis->setRange(0, yMax+1);
        // 设置横坐标和纵坐标的刻度标签
        QVector<QString> Xaxis;
        QVector<QString> Yaxis;
        for (int i = 0; i < xMax+1; ++i) {
            Xaxis.append(QString::number(i));
        }
        for (int i = 0; i < yMax+1; ++i) {
            Yaxis.append(QString::number(i));
        }
        colorMap->data()->setSize(Xaxis.size(), Yaxis.size());   // 设置颜色图数据维度，其内部维护着一个一维数组（一般表现为二维数组），这里可以理解为有多少个小方块
        colorMap->data()->setRange(QCPRange(0.5, Xaxis.size() - 0.5), QCPRange(0.5, Yaxis.size() - 0.5));  // 颜色图在x、y轴上的范围
        QSharedPointer<QCPAxisTickerText> xTicker(new QCPAxisTickerText);
        QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
        xTicker->setTicks(labelPositions(Xaxis, 0.5), Xaxis);
        yTicker->setTicks(labelPositions(Yaxis, 0.5), Yaxis);
        xTicker->setSubTickCount(1);
        yTicker->setSubTickCount(1);
        customPlot->xAxis->setTicker(xTicker);
        customPlot->yAxis->setTicker(yTicker);
        customPlot->xAxis->grid()->setPen(Qt::NoPen);
        customPlot->yAxis->grid()->setPen(Qt::NoPen);
        customPlot->xAxis->grid()->setSubGridVisible(true);
        customPlot->yAxis->grid()->setSubGridVisible(true);
        customPlot->xAxis->setSubTicks(true);
        customPlot->yAxis->setSubTicks(true);
        customPlot->xAxis->setTickLength(0);
        customPlot->yAxis->setTickLength(0);
        customPlot->xAxis->setSubTickLength(6);
        customPlot->yAxis->setSubTickLength(6);
        customPlot->xAxis->setRange(0, Xaxis.size());
        customPlot->yAxis->setRange(0, Yaxis.size());

        //设置Z值来显示颜色
        for (int x = 0; x < Xaxis.size(); ++x) {
            for (int y = 0; y < Yaxis.size(); ++y) {
                colorMap->data()->setAlpha(x, y, 0);
            }
        }

        for(QString line : lines)
        {
            flag=1;
            QStringList values = line.split(',', Qt::SkipEmptyParts);//以逗号分解
            bool ok;
            double firstValue = values.at(0).toDouble(&ok);
            if(!ok){
                ui->textBrowser->append("x,y,dB,ultra_fusion*20,forecast_force");
                continue;
            }

            if(values.size() == 23){
                for(auto value:values){
                    value.toDouble(&ok);
                    if(!ok){
                        QString prompt = "第"+QString::number(num)+"行"+value+"格式错误，请检查!";
                        QMessageBox::warning(this,"警告",prompt);
                        ui->textBrowser->clear();
                        series->clear();
                        flag=0;
                        num=1;//num用来记录处理到第几行
                        file.close();
                        //series->setName("拉力预测值");
                        Xposision=-1;
                        Yposision=-1;
                        //预测结束
                        ui->statuLable->setText("预测出错，请重新调整待预测文件数据格式。");
                        ui->BrowseButton->setEnabled(true);
                        ui->SingleButton->setEnabled(true);
                        return;
                    }
                }
                if((values[0].toInt()>=0) && (values[1].toInt()>=0)){

                    Xposision=values[0].toInt();
                    Yposision=values[1].toInt();

                    statusOutput = values[0]+","+values[1]+","+values[2]+",";
                    QJsonObject jsonBody;
                    QJsonObject dataObject;
                    QJsonArray dataArray;
                    dataObject["db"] = values[2];
                    for(int i=1;i<21;i++){
                        dataObject["ultra_fusion_"+QString::number(i)] = values[i+2];
                        statusOutput += values[i+2]+",";
                    }
                    dataArray.append(dataObject);
                    jsonBody["data"] = dataArray;
                    QJsonDocument jsonDocument(jsonBody);
                    QByteArray postData = jsonDocument.toJson();

                    // 发送请求
                    get_forcastResult->post(request, postData);
                    ui->statuLable->setText("预测中，请稍等。");

                    //等待回复
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
                    num++;
                }
                else{
                    QString prompt = "第"+QString::number(num)+"行坐标出现负值!";
                    QMessageBox::warning(this,"警告",prompt);
                    ui->textBrowser->clear();
                    series->clear();
                    flag=0;
                    break;
                }
            }
            else
            {
                QString prompt = "第"+QString::number(num)+"行格式错误，请检查!";
                QMessageBox::warning(this,"警告",prompt);
                ui->textBrowser->clear();
                series->clear();
                flag=0;
                break;
            }
        }

        //预测结束
        num=1;//num用来记录处理到第几行
        file.close();
        Xposision=-1;
        Yposision=-1;
        if(flag){
            ui->statuLable->setText("预测完成，可以输入下一次参数及待预测文件。");
        }else{
            ui->statuLable->setText("预测出错，请重新调整待预测文件数据格式。");
        }
        ui->BrowseButton->setEnabled(true);
        ui->SingleButton->setEnabled(true);
    }
}

void MainWindow::on_getAccessToken(){

    ui->statuLable->setText("正在获取Token");

    //密钥放置位置
    QString apiKey = "93HNPMtby3GJe3nAMK1hq3r4";
    QString secretKey = "qqwiFAqCQOaz6eNIM4N1eoEfP2AWeUfB";

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

void MainWindow::forcastResult(QNetworkReply* pReply){ //此方法目前无用

    if(pReply->error() == QNetworkReply::NoError){
        // ui->textBrowser->setText("开始预测");
        QByteArray string = pReply->readAll();
        qDebug()<<string;
        //获取内容
        QJsonParseError jsonErr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(string, &jsonErr);
        if (jsonDoc.isObject()){
            QJsonObject jsonObj = jsonDoc.object();
            QJsonArray batchResultArray = jsonObj.value("batch_result").toArray();
            QJsonObject firstElement = batchResultArray.at(0).toObject();
            double col5Value = firstElement.value("col5").toDouble();
            focastresult = QString::number(col5Value);
            ui->textBrowser->insertPlainText("预测拉力值： "+focastresult);
            ui->statuLable->setText("预测完成，可以输入下一次预测值，或者输入待预测文件。");
            disconnect(getForcastResult);
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
        //qDebug()<<string;

        //获取内容
        QJsonParseError jsonErr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(string, &jsonErr);

        //如果返回的是json格式才继续
        if (jsonDoc.isObject()){

            //获取预测值
            QJsonObject jsonObj = jsonDoc.object();
            QJsonArray batchResultArray = jsonObj.value("batch_result").toArray();
            QJsonObject firstElement = batchResultArray.at(0).toObject();
            double force = firstElement.value("force").toDouble();
            focastresult = QString::number(force);

            //将预测值输出
            statusOutput += focastresult;
            ui->textBrowser->append(statusOutput);
            ui->statuLable->setText("预测完成，等待预测下一个值。");
            disconnect(getForcastResult);

            //将预测值添加到表格中
            series->append(num,focastresult.toFloat());

            // 获取 QChart 对象
            QChart *chart = ui->chartView->chart();
            // 获取当前的 X 轴对象
            QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axisX());
            // 修改 X 轴范围
            axisX->setRange(1, num);
            // 获取当前的 Y 轴对象
            QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axisY());

            // 遍历系列中的数据点，找到最大值
            qreal maxValue = std::numeric_limits<qreal>::lowest();
            foreach (const QPointF &point, series->points()) {
                maxValue = std::max(maxValue, point.y());
            }
            axisY->setRange(0, maxValue+50);

            if( Xposision>=0 && Yposision >= 0 ){
                dynamicHeatmap(Xposision,Yposision,force);
            }
            num++;
        }
    }
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
    colorMap->data()->setAlpha(x,y,255);
    if(z<100){
        QCPItemLine *line1 = new QCPItemLine(customPlot);
        line1->start->setCoords(x+0.25, y+0.25);
        line1->end->setCoords(x+0.75, y+0.75);
        line1->setPen(QPen(Qt::red));
        drawLines.append(line1);
        QCPItemLine *line2 = new QCPItemLine(customPlot);
        line2->start->setCoords(x+0.25, y+0.75);
        line2->end->setCoords(x+0.75, y+0.25);
        line2->setPen(QPen(Qt::red));
        drawLines.append(line2);
    }
    ui->widget->replot();
}

void MainWindow::loopEvent(){
    loop.quit();
}

void MainWindow::openTable(){
    Form *tableWindow =new Form;
    tableWindow->show();
}

void MainWindow::inputTable(){
    QString waveHight = ui->waveHightEdit->text();
    QString dB = ui->dBEdit->text();
    QString xStep= ui->xStepEdit->text();
    QString yStep= ui->yStepEdit->text();
    int xNum = 20/xStep.toInt();
    QVector<QVector<QString>> resDatas;
    Form *tableWindow =new Form;
    if(waveHight.isEmpty() || dB.isEmpty() || xStep.isEmpty() || yStep.isEmpty()){
        QMessageBox::warning(this,"警告","请输入参数！");
        return;
    }

    //获取待处理文件名
    QString fileName = QFileDialog::getOpenFileName(this, "选择一个文件",
                                                    QCoreApplication::applicationFilePath(),
                                                    "*");
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else
    {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::warning(this,"警告","该文件无法读取！");
            return;
        }
        //打开后开始处理
        else{
            //处理数据，存入values;
            QMessageBox msgBox;
            msgBox.setWindowTitle("提示");
            msgBox.setText("开始处理数据！");
            msgBox.exec();
            // //处理数据
            QTextStream in(&file);
            QVector<QVector<QString>> datas;
            ui->statuLable->setText("读入数据中。");
            while(!in.atEnd()){
                QString line = in.readLine();//读一行
                QStringList values = line.split(',', Qt::SkipEmptyParts);//以逗号分解
                bool ok;
                values.at(0).toDouble(&ok);
                if(!ok){
                    continue;
                }
                QVector<QString> rowData;
                rowData.append(values[0]);
                rowData.append(values[1]);
                rowData.append(fineMax(values));
                datas.append(rowData);
            }
            ui->statuLable->setText("读入数据完成。");
            QString maxFirstColumnValue = datas[0][0]; // 初始化第一列最大值为第一行第一列的值
            QString maxSecondColumnValue = datas[0][1]; // 初始化第二列最大值为第一行第二列的值

            for (const auto& row : datas) {
                if (row.size() > 2) {
                    if (row[0].toInt() > maxFirstColumnValue.toInt()) {
                        maxFirstColumnValue = row[0];
                    }
                    if (row[1].toInt() > maxSecondColumnValue.toInt()) {
                        maxSecondColumnValue = row[1];
                    }
                }
            }
            ui->statuLable->setText("共存在"+maxSecondColumnValue+"行"+maxFirstColumnValue+"列数据");
            // qDebug()<<maxFirstColumnValue<<maxSecondColumnValue;

            for(int y = 0 ; y<maxSecondColumnValue.toInt()-1 ; y++){
                for(int x =0 ; x<(maxFirstColumnValue.toInt()-1)/20 ; x++){
                    QVector<QString> resData;
                    QString outputString;
                    resData.append(QString::number(x));
                    resData.append(QString::number(y));
                    resData.append(dB);
                    outputString += QString::number(x)+","+QString::number(y)+","+dB+",";
                    for(int k = 0 ; k<xNum;k++){
                        for(const auto& row : datas){
                            if(row[0].toInt()==x*xNum+k+1 && row[1].toInt() ==y+1){
                                resData.append(row[2]);
                                outputString += row[2] + ",";
                            }
                        }
                    }
                    //qDebug()<<resData;
                    outputString.chop(1);
                    ui->statuLable->setText("写入数据:"+outputString);
                    resDatas.append(resData);
                }
            }
        }
    }
    ui->statuLable->setText("导入数据成功，请查看是否需要修改！");
    tableWindow->setTableValues(resDatas);
    tableWindow->show();
}

QString MainWindow::fineMax(QVector<QString> rowData){
    QVector<double> numericData;
    for (int i = 3; i < rowData.size(); ++i) {
        bool ok;
        double value = rowData[i].toDouble(&ok);
        if (ok) {
            numericData.append(value);
        }
    }

    // 找到最大值
    double max = numericData.isEmpty() ? 0 : numericData.first();
    for (int i = 1; i < numericData.size(); ++i) {
        if (numericData[i] > max) {
            max = numericData[i];
        }
    }

    return QString::number(max);
}
//获取当前鼠标位置的方法
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
    ui->label_5->setStyleSheet("font-size: 10pt;");
    ui->label_5->setText("在（" + QString::number(x) +  ", " +QString::number(y) + ") 处的值为：" + QString::number(z));
    // qDebug() << "Z value at (" << x << ", " << y << ") is " << z;
}

//输出数据方法
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
    //text=text.mid(text.indexOf("e")).mid(text.indexOf("\n"));
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
        out<<text;
        file.close();
    }

    // qDebug()<<text;
}

//以下是设置查询速度的方法
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

//以下是帮助信息
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
