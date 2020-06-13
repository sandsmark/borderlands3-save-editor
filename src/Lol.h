#pragma once

#include <QObject>
#include <QWidget>
#include <QBasicTimer>
#include <QVector4D>
#include <QVector2D>

class Lol : public QWidget
{
    Q_OBJECT

public:
    Lol(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *e) override;

private:
    QBasicTimer m_repaintTimer;
    int m_step = 0;

    QVector<QVector4D> m_vertices;
    QVector<QVector2D> m_edges;
};
