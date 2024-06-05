#include "sector.h"

SectorClass::SectorClass(qreal size, qreal row, qreal sector, QColor color, double power, qreal max_sectors) {
    this->row = row;
    this->sector = sector;
    this->color = color;
    this->power = power;
    outer_circle = row * (size / 20);
    inner_circle = outer_circle + (size / 20);
    outer_diameter = size - (row * size / 10);
    inner_diameter = outer_diameter - (size / 10);
    angle = 360 / max_sectors;
    start_x = (size / 2) + (cos(qDegreesToRadians(sector * angle))) * (outer_diameter / 2); //- angle / 2 если центр сектора по середине
    start_y = (size / 2) - (sin(qDegreesToRadians(sector * angle))) * (outer_diameter / 2); //- angle / 2 если центр сектора по середине
}

void SectorClass::draw(QPainter& qp) {
    QBrush brush(color);
    qp.setPen(color);
    qp.setBrush(brush);
    QPainterPath path;
    path.moveTo(start_x, start_y);
    path.arcTo(
        outer_circle,
        outer_circle,
        outer_diameter,
        outer_diameter,
        sector * angle, //- angle / 2 если центр сектора по середине
        angle
    );
    path.arcTo(
        inner_circle,
        inner_circle,
        inner_diameter,
        inner_diameter,
        (sector + 1) * angle, //- angle / 2 если центр сектора по середине
        - angle
    );
    path.lineTo(start_x, start_y);
    qp.drawPath(path);
}

