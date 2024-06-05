#ifndef SECTOR_H
#define SECTOR_H

#include <QtWidgets>
#include <QColor>
#include <QBrush>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

class SectorClass {
public:
    SectorClass(qreal size, qreal sector, qreal row, QColor color, double power, qreal max_sectors);
    void draw(QPainter& qp);
    qreal sector;
    qreal row;
    QColor color;
    double power;
private:
    qreal outer_circle;
    qreal inner_circle;
    qreal outer_diameter;
    qreal inner_diameter;
    qreal angle;
    qreal start_x;
    qreal start_y;
};

#endif // SECTOR_H
