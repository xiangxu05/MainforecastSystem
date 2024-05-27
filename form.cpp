#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    this->setWindowTitle("数据导入表");

    addRowAction = new QAction("增加行", this);
    deleteRowAction = new QAction("删除行", this);
    addColumnAction = new QAction("增加列", this);
    deleteColumnAction = new QAction("删除列", this);
    renameColumnAction = new QAction("修改列名", this);

    // 连接信号
    connect(addRowAction, &QAction::triggered, this, &Form::addRow);
    connect(deleteRowAction, &QAction::triggered, this, &Form::deleteRow);
    connect(addColumnAction, &QAction::triggered, this, &Form::addColumn);
    connect(deleteColumnAction, &QAction::triggered, this, &Form::deleteColumn);
    connect(renameColumnAction, &QAction::triggered, this, &Form::renameColumn);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &Form::showContextMenu);

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
        outputFilePath = fileName;

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
    emit filePathChanged(outputFilePath);
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

void Form::showContextMenu(const QPoint &pos) {
    QMenu contextMenu(this);

    contextMenu.addAction(addRowAction);
    contextMenu.addAction(deleteRowAction);
    contextMenu.addAction(addColumnAction);
    contextMenu.addAction(deleteColumnAction);
    contextMenu.addAction(renameColumnAction);

    contextMenu.exec(ui->tableWidget->mapToGlobal(pos));
}

void Form::addRow() {
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow >= 0) {
        // Insert a row below the currently selected row
        ui->tableWidget->insertRow(currentRow + 1);
    } else {
        // If no row is selected, insert at the end
        int currentRowCount = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(currentRowCount);
    }
}

void Form::deleteRow() {
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow >= 0) {
        ui->tableWidget->removeRow(currentRow);
    }
}

void Form::addColumn() {
    int currentColumnCount = ui->tableWidget->currentColumn();
    if(currentColumnCount >= 0){
        ui->tableWidget->insertColumn(currentColumnCount+1);
    }else{
        currentColumnCount = ui->tableWidget->columnCount();
        ui->tableWidget->insertColumn(currentColumnCount);
    }
}

void Form::deleteColumn() {
    int currentColumn = ui->tableWidget->currentColumn();
    if (currentColumn >= 0) {
        ui->tableWidget->removeColumn(currentColumn);
    }
}

void Form::renameColumn() {
    int currentColumn = ui->tableWidget->currentColumn();
    if (currentColumn >= 0) {
        // 检查并初始化列头项
        QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(currentColumn);
        if (!headerItem) {
            headerItem = new QTableWidgetItem(QString("Column %1").arg(currentColumn + 1));
            ui->tableWidget->setHorizontalHeaderItem(currentColumn, headerItem);
        }

        bool ok;
        QString newColumnName = QInputDialog::getText(this, tr("修改列名"),
                                                      tr("请输入新的列名:"), QLineEdit::Normal,
                                                      headerItem->text(), &ok);
        if (ok && !newColumnName.isEmpty()) {
            headerItem->setText(newColumnName);
        }
    }
}


