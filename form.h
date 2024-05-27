#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <Windows.h>
#include <QInputDialog>

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

    void filePathChanged(const QString &parameter);

private slots:

    void on_saveButton_clicked();
    void on_pushButton_clicked();
    void showContextMenu(const QPoint &pos);
    void addRow();
    void deleteRow();
    void addColumn();
    void deleteColumn();
    void renameColumn();

private:
    Ui::Form *ui;
    QAction *addRowAction;
    QAction *deleteRowAction;
    QAction *addColumnAction;
    QAction *deleteColumnAction;
    QAction *renameColumnAction;
    QString outputFilePath;
};

#endif // FORM_H
