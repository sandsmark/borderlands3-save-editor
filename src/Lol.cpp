#include "Lol.h"
#include <QPainter>
#include <QTimerEvent>
#include <QtMath>
#include <QMatrix4x4>
#include <cmath>
#include <QResizeEvent>

Lol::Lol(QWidget *parent) : QWidget(parent)
{
    m_vertices = {
        { 1, 1, 1, 1 },
        { 1, 1, 1,-1 },
        { 1, 1,-1, 1 },
        { 1, 1,-1,-1 },
        { 1,-1, 1, 1 },
        { 1,-1, 1,-1 },
        { 1,-1,-1, 1 },
        { 1,-1,-1,-1 },
        {-1, 1, 1, 1 },
        {-1, 1, 1,-1 },
        {-1, 1,-1, 1 },
        {-1, 1,-1,-1 },
        {-1,-1, 1, 1 },
        {-1,-1, 1,-1 },
        {-1,-1,-1, 1 },
        {-1,-1,-1,-1 }
    };

    m_edges = {
        { 0,  1}, { 0,  2}, { 0,  4}, { 0,  8},
        { 1,  3}, { 1,  5}, { 1,  9},
        { 2,  3}, { 2,  6}, { 2, 10},
        { 3,  7}, { 3, 11},
        { 4,  5}, { 4,  6}, { 4, 12},
        { 5,  7}, { 5, 13},
        { 6,  7}, { 6, 14},
        { 7, 15},
        { 8,  9}, { 8, 10}, { 8, 12},
        { 9, 11}, { 9, 13},
        {10, 11}, {10, 14},
        {11, 15},
        {12, 13}, {12, 14},
        {13, 15},
        {14, 15}
    };

    m_repaintTimer.start(16, this);
}

void Lol::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(palette().color(QPalette::Disabled, QPalette::WindowText));

    float scale = qMin(width(), height()) / 4. - contentsMargins().left()*2;

    const float sine = std::sin(m_step/100.);
    const float cosine = std::cos(m_step/100.);
    const float width = this->width();
    const float height = this->height();

    QVector<QVector2D> vertices;
    for (int i=0; i<m_vertices.count(); i++) {
        float x = m_vertices[i].x();
        float y = m_vertices[i].y();
        float z = m_vertices[i].z();
        float w = m_vertices[i].w();
        float tmp;

        tmp = cosine * x + sine * y;
        y = -sine * x + cosine * y;
        x = tmp;
        tmp = cosine * x + sine * z;
        z = -sine * x + cosine * z;
        x = tmp;
        tmp = cosine * x + sine * w;
        w = -sine * x + cosine * w;
        x = tmp;
        tmp = cosine * y + sine * z;
        z = -sine * y + cosine * z;
        y = tmp;
        tmp = cosine * y - sine * w;
        w = sine * y + cosine * w;
        y = tmp;
        z = cosine * z - sine * w;

        vertices.append({
            width / 2.f + (0.90f + z * 0.30f) * scale * x,
            height / 2.f - (0.90f + z * 0.30f) * scale * y
        });
    }

    for (int i=0; i<m_edges.count(); i++) {
        p.drawLine(vertices[m_edges[i][0]].x(), vertices[m_edges[i][0]].y(),
                vertices[m_edges[i][1]].x(), vertices[m_edges[i][1]].y()
                );
    }

    p.setPen(palette().color(QPalette::WindowText));
    p.drawText(rect(), Qt::AlignCenter, "© martin");
}

void Lol::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != m_repaintTimer.timerId()) {
        QWidget::timerEvent(e);
        return;
    }

    m_step++;
    update();
}
