#include "CalculationThread.h"

CalculationThread::CalculationThread(QObject *parent)
    : QThread(parent)
{
}

void CalculationThread::setParameters(const QVector<QVector<QString>> &lines, int xStep, int yStep,double threshold,double standard)
{
    m_lines = lines;
    m_xStep = xStep;
    m_yStep = yStep;
    m_threshold = threshold;
    m_standard = standard;
}

void CalculationThread::run()
{
    int xMax = m_lines[0].size();
    int yMax = m_lines.size();
    // qDebug() << xMax << yMax;

    int xStepCount = 20 / m_xStep;
    int yStepCount = 100 / m_yStep;

    for (int x = 0; x <= xMax - xStepCount; x += 1) {
        for (int y = 0; y <= yMax - yStepCount; y += 1) {
            double final = 0;
            double total = 0;

            // 优化边界检查
            if ((x + xStepCount) > xMax || (y + yStepCount) > yMax) {
                continue;
            }

            for (int i = 0; i < xStepCount; i++) {
                for (int j = 0; j < yStepCount; j++) {
                    // if(m_lines[y + j][x + i].toDouble()>m_threshold)
                    //     total ++;//商定的方法
                    total += m_lines[y + j][x + i].toDouble();
                }
            }

            final = total / (xStepCount * yStepCount);

            for (int i = 0; i < xStepCount; i++) {
                for (int j = 0; j < yStepCount; j++) {
                    // emit updateHeatmap(x + i, y + j, (m_standard * final));//商定的方法
                    emit updateHeatmap(x + i, y + j, (m_standard * final / m_threshold));
                    // qDebug()<<x + i<<" "<<y + j<<" "<<(m_standard * final / m_threshold);
                    QThread::msleep(1);
                    // loop.exec();
                    // QMutexLocker locker(&mutex); // 使用互斥锁保护临界区域
                    // waitCondition.wait(&mutex);
                }
            }
            // QThread::msleep(20);
        }
    }
    emit calculationFinished();
}

void CalculationThread::onHeatmapUpdateFinished() {
    // When heatmapUpdateFinished signal is emitted, exit the event loop
    // QThread::msleep(1);
    // qDebug()<<"1";
    // loop.quit();
    // waitCondition.wakeOne();
}
