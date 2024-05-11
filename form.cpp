#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    this->setWindowTitle("数据导入表");
}

Form::~Form()
{
    delete ui;
}

void Form::on_saveButton_clicked()
{
    // 设置默认文件格式
    QString defaultFileFormat = "csv";  // 例如，CSV 文件

    // 设置默认文件名
    QString defaultFileName = "output";  // 例如，输出文件

    // 合并默认文件名和格式
    QString defaultFileFullName = defaultFileName + "." + defaultFileFormat;

    // 打开保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "选择一个文件",
        defaultFileFullName,
        "CSV Files (*.csv);;All Files (*)"
        );

    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else
    {
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this,"警告","无法打开文件："+file.errorString());
            return;
        }

        QTextStream out(&file);

        int rowCount = ui->tableWidget->rowCount();
        int columnCount = ui->tableWidget->columnCount();

        for (int row = 0; row < rowCount; ++row) {
            for (int col = 0; col < columnCount; ++col) {
                // 获取单元格文本值
                QTableWidgetItem *item = ui->tableWidget->item(row, col);
                if (item) {
                    QString cellText = item->text();
                    // 将数据写入文件，使用逗号分隔各个字段
                    out << cellText;
                    if (col < columnCount - 1) {
                        out << ",";
                    }
                }
            }
            // 每一行结束时换行
            out << "\n";
        }

        // 关闭文件
        file.close();
    }
}


void Form::on_pushButton_clicked()
{
    QString defaultFilePath = QCoreApplication::applicationDirPath();
    QString fileName = "/.temporateData.csv";

    QFile file(defaultFilePath + fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::warning(this, "警告", "无法打开文件：" + file.errorString());
        return;
    }

    QTextStream out(&file);

    #ifdef Q_OS_WIN
    // Set the file attributes to hidden
    SetFileAttributes((LPCWSTR)file.fileName().utf16(), FILE_ATTRIBUTE_HIDDEN);
    #endif

    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            // 获取单元格文本值
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item) {
                QString cellText = item->text();
                // 将数据写入文件，使用逗号分隔各个字段
                out << cellText;
                if (col < columnCount - 1) {
                out << ",";
            }
        }
    }
        // 每一行结束时换行
        out << "\n";
    }

    // 关闭文件
    file.close();

    emit customSignal();
    QCoreApplication::processEvents();

    Form::close();
}

void Form::setTableValues(const QVector<QVector<QString>> &values)
{
    // 清空表格
    ui->tableWidget->clear();

    // 设置表格的行数和列数
    int rowCount = values.size();
    int columnCount = values.isEmpty() ? 0 : values[0].size();
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(columnCount);

    // 遍历二维数组，为表格的每个单元格设置值
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            QTableWidgetItem *item = new QTableWidgetItem(values[row][column]);
            ui->tableWidget->setItem(row, column, item);
        }
    }
}
