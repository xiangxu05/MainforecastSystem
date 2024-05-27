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
    this->setWindowTitle("超声检测管道性能预测系统 V3.2");
    ui->statuLable->setText("系统初始化中");

    //热力图指针定义
    customPlot=ui->widget;//指针方便使用widget
    setupHeatmap(customPlot);

    connect(ui->action_H,&QAction::triggered,this,&MainWindow::heatmapHelp);
    connect(ui->action_O,&QAction::triggered,this,&MainWindow::openTable);
    connect(ui->action_N,&QAction::triggered,this,&MainWindow::newTable);
    connect(ui->action_D,&QAction::triggered,this,&MainWindow::inputTable);
    connect(ui->Output_hetmap,&QAction::triggered,this,&MainWindow::outputHeatMap);
    connect(ui->Output_data,&QAction::triggered,this,&MainWindow::outputData);
    connect(ui->widget, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);
    ui->statuLable->setText("初始化完成，可以开始理论值计算。");

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
    xTicker->setTicks(labelPositions(Xaxis,1, 0.5), Xaxis);
    yTicker->setTicks(labelPositions(Yaxis,1, 0.5), Yaxis);//刻度位置设置
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
    heatmap->setDataRange(QCPRange(0, 300));     // 手动设置数据范围
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
    QString standardTensile = ui->standardEdit->text();
    QString threshold = ui->thresholdEdit->text();
    QString dB = ui->dBEdit->text();
    QString xStep= ui->xStepEdit->text();
    QString yStep= ui->yStepEdit->text();
    QString filePath = ui->FilelineEdit->text();
    if(standardTensile.isEmpty() || threshold.isEmpty() || dB.isEmpty() || xStep.isEmpty() || yStep.isEmpty() || filePath.isEmpty()){
        QMessageBox::warning(this,"警告","缺少相关参数或文件路径！");
        ui->SingleButton->setEnabled(true);
        return;
    }
    ui->BrowseButton->setDisabled(true); //浏览按钮关闭

    for (auto line : drawLines) {
        //ui->widget->removeItem(line);
        delete line;
    }
    // 移除完成后清空容器
    drawLines.clear();

    //打开文件
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
        int xMax = 1,yMax =1;
        QVector<QVector<QString>> lines;
        int num=1;
        while(!in.atEnd()){
            QString line = in.readLine();//读一行
            QStringList values = line.split(',', Qt::SkipEmptyParts);//以逗号分解
            bool ok;
            for(auto value : values){
                value.toDouble(&ok);
                if(!ok || value.toDouble()<0){
                    QString prompt = "第"+QString::number(num)+"行"+value+"格式错误，请检查!";
                    QMessageBox::warning(this,"警告",prompt);
                    ui->BrowseButton->setEnabled(true);
                    ui->SingleButton->setEnabled(true);
                    return;
                }
            }
            lines.append(values);
            num++;
        }

        xMax = lines[0].size();
        yMax = lines.size();
        ui->statuLable->setText("共有"+QString::number(yMax)+"行"+QString::number(xMax)+"列数据，开始计算！");
        results = QVector<QVector<double>>(yMax, QVector<double>(xMax, 999));
        // 设置横坐标和纵坐标的范围
        ui->widget->xAxis->setRange(0, xMax-1);
        ui->widget->yAxis->setRange(0, yMax-1);
        // 设置横坐标和纵坐标的刻度标签
        QVector<QString> Xaxis;
        QVector<QString> Yaxis;
        for (int i = 0; i < xMax; ++i) {
            Xaxis.append(QString::number(i));
        }
        for (int i = 0; i < yMax; ++i) {
            Yaxis.append(QString::number(i));
        }
        colorMap->data()->setSize(xMax,yMax);   // 设置颜色图数据维度，其内部维护着一个一维数组（一般表现为二维数组），这里可以理解为有多少个小方块
        colorMap->data()->setRange(QCPRange(0.5, xMax - 0.5), QCPRange(0.5, yMax - 0.5));  // 颜色图在x、y轴上的范围
        QSharedPointer<QCPAxisTickerText> xTicker(new QCPAxisTickerText);
        QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
        // xTicker->setTicks(labelPositions(Xaxis, 0.5), Xaxis);
        QVector<double> positions = labelPositions(Xaxis, 10, 0.5);

        QVector<QString> labels;
        for (int i = 0; i < Xaxis.size(); i += 10) {
            labels.append(Xaxis[i]);
        }

        xTicker->setTicks(positions, labels);
        yTicker->setTicks(labelPositions(Yaxis,1, 0.5), Yaxis);
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
        //初始化所有格的alpha值
        for (int x = 0; x < xMax ; ++x) {
            for (int y = 0; y <yMax; ++y) {
                colorMap->data()->setAlpha(x, y, 0);
            }
        }
        ui->widget->replot();

        CalculationThread *calcThread = new CalculationThread();
        calcThread->setParameters(lines, xStep.toInt(), yStep.toInt(),threshold.toDouble(),standardTensile.toDouble());

        connect(calcThread, &CalculationThread::updateHeatmap, this, &MainWindow::dynamicHeatmap);
        connect(calcThread, &CalculationThread::calculationFinished, this, [=]() {
            ui->statuLable->setText("预测完成，可以输入下一次参数及待预测文件。");
            ui->BrowseButton->setEnabled(true);
            ui->SingleButton->setEnabled(true);
            calcThread->deleteLater();
        });

        calcThread->start();

    }
}

QVector<double> MainWindow::labelPositions(const QVector<QString> &labels, int interval, double offset)
{
    QVector<double> positions;
    for (int i = 0; i < labels.size(); i += interval) {
        positions.append(i + offset);
    }
    return positions;
}

void MainWindow::dynamicHeatmap(int x , int y , double z)
{
    auto *colorMap = static_cast<QCPColorMap *>(ui->widget->plottable(0));
    // 检查边界条件，防止越界访问
    int keySize = colorMap->data()->keySize();
    int valueSize = colorMap->data()->valueSize();

    // qDebug() << "keySize:" << keySize << " valueSize:" << valueSize;
    // qDebug() << "Setting z at (" << x << "," << y << ") to" << z;

    if (x >= 0 && x < keySize && y >= 0 && y < valueSize) {
            // 只在更新需要的时候才进行
        if (results[y][x] > z) {
            results[y][x] = z;

            // 更新热力图
            colorMap->data()->setCell(x, y, z);
            colorMap->data()->setAlpha(x, y, 255);
            // ui->widget->replot();
            ui->statuLable->setText("更新：第" + QString::number(y) + "行，第" + QString::number(x) + "列数值为" + QString::number(z) + "N");
        }
    } else {
        qDebug() << "索引超出范围: x=" << x << ", y=" << y << " keySize=" << keySize << " valueSize=" << valueSize;
    }

    if(z<200){
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
    if(y==0 || y == valueSize)
        ui->widget->replot();
}

void MainWindow::newTable(){
    Form *tableWindow =new Form;
    tableWindow->show();
    connect(tableWindow, &Form::filePathChanged, this, &MainWindow::handleParameterChange);
}

void MainWindow::openTable(){
    QVector<QVector<QString>> resDatas;

    Form *tableWindow =new Form;

    //获取待处理文件名
    QString fileName = QFileDialog::getOpenFileName(this, "选择一个文件",
                                                    QCoreApplication::applicationFilePath(),
                                                    "*");
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
        return;
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
            QTextStream in(&file);
            ui->statuLable->setText("读入数据中。");
            while(!in.atEnd()){
                QString line = in.readLine();//读一行
                QStringList values = line.split(',', Qt::SkipEmptyParts);//以逗号分解
                resDatas.append(values);
            }

            ui->statuLable->setText("读入数据完成。");
        }
    }
    ui->statuLable->setText("打开文件成功！");
    tableWindow->setTableValues(resDatas);
    tableWindow->show();
    connect(tableWindow, &Form::filePathChanged, this, &MainWindow::handleParameterChange);
}

void MainWindow::inputTable(){

    QVector<QVector<QString>> resDatas;

    Form *tableWindow =new Form;

    //获取待处理文件名
    QString fileName = QFileDialog::getOpenFileName(this, "选择一个文件",
                                                    QCoreApplication::applicationFilePath(),
                                                    "*");
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
        return;
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
            qDebug()<<maxFirstColumnValue<<maxSecondColumnValue;

            QVector<QString> resData;

            for(int y = 0 ; y<maxSecondColumnValue.toInt() ; y++){
                for(int x =0 ; x<maxFirstColumnValue.toInt() ; x++){
                    for(const auto& row : datas){
                        if(row[0].toInt()==x+1 && row[1].toInt() ==y+1){
                            resData.append(QString::number(qRound(row[2].toDouble()/255*200*100)/100));
                        }
                    }
                }
                resDatas.append(resData);
                resData.clear();
            }
        }
    }
    ui->statuLable->setText("导入数据成功，请查看是否需要修改！");
    tableWindow->setTableValues(resDatas);
    tableWindow->show();
    connect(tableWindow, &Form::filePathChanged, this, &MainWindow::handleParameterChange);
}

QString MainWindow::fineMax(QVector<QString> rowData){
    double numericData=0;
    for (int i = 2; i < rowData.size(); ++i) {
        double value = rowData[i].toDouble();
        if(value > numericData){
            numericData = value;
        }
    }
    return QString::number(numericData);
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
    QString text;
    for (const auto &row : results) {
        QStringList rowStrings;
        for (const auto &value : row) {
            rowStrings << QString::number(value);
        }
        text += rowStrings.join(",") + '\n';
    }

    if (!text.isEmpty()) {
        text.chop(1);
    }
    // QString text = ui->textBrowser->toPlainText();
    // //text=text.mid(text.indexOf("e")).mid(text.indexOf("\n"));
    // QStringList lines = text.split("\n", Qt::SkipEmptyParts);

    // QList<QStringList> values;
    // QStringList value;

    // foreach (const QString &line, lines) {
    //     QRegularExpression re("\\d+\\.?\\d*");
    //     QRegularExpressionMatchIterator matchIterator = re.globalMatch(line);
    //     value.clear();
    //     while (matchIterator.hasNext()) {
    //         QRegularExpressionMatch match = matchIterator.next();
    //         value << match.captured();
    //     }
    //     values << value;
    // }

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

    // // qDebug()<<text;
}

//以下是帮助信息
void MainWindow::heatmapHelp()
{
    // 创建消息框
    QMessageBox helpBox;

    // 设置消息框的图标为帮助图标
    helpBox.setIcon(QMessageBox::Information);

    // 设置消息框的标题
    helpBox.setWindowTitle("帮助信息");

    // 设置消息框的文本内容
    helpBox.setText("整体推断时，生成相应的拉力预测值与热力图。");

    // 添加详细信息
    helpBox.setDetailedText("目前支持输入csv进行推断，输入格式为：\n"
                            "每行每列中输入对应位置超声值，超声值位置对应坐标，数据之间用逗号“，”隔开。\n"
                            "原始数据可以通过导入来进行处理。");

    // 添加一个“确定”按钮
    helpBox.addButton(QMessageBox::Ok);

    // 显示消息框
    helpBox.exec();
}

void MainWindow::handleParameterChange(const QString &parameter)
{
    ui->FilelineEdit->setText(parameter);
}
