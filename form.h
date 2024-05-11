#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <Windows.h>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:

    explicit Form(QWidget *parent = nullptr);
    void setTableValues(const QVector<QVector<QString>> &values);
    ~Form();

signals:

    void customSignal();

private slots:

    void on_saveButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::Form *ui;
};

#endif // FORM_H
