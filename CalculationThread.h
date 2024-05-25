#ifndef CALCULATIONTHREAD_H
#define CALCULATIONTHREAD_H

#include <QThread>
#include <QVector>
#include <QString>
#include <QEventLoop>
#include <QDebug>
#include <qwaitcondition.h>
#include <QMutex>

class CalculationThread : public QThread
{
    Q_OBJECT
public:
    CalculationThread(QObject *parent = nullptr);
    void setParameters(const QVector<QVector<QString>> &lines, int xStep, int yStep,double threshold,double standard);

signals:
    void updateHeatmap(int x, int y, double value);
    void calculationFinished();

protected:
    void run() override;

private:
    QVector<QVector<QString>> m_lines;
    int m_xStep;
    int m_yStep;
    double m_threshold;
    double m_standard;
    QEventLoop loop;
    QWaitCondition waitCondition;
    QMutex mutex;

public slots:
    void onHeatmapUpdateFinished();

};

#endif // CALCULATIONTHREAD_H
