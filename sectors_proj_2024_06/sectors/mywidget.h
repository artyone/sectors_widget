#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QLabel>
#include "sector.h"
#include <QUdpSocket>
#include <QHostAddress>
#include <cmath>

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = nullptr);
    void updateSectorColorFromData(QList<QList<double>> data); //основная функция передачи данных в виджет
    //массив data - двумерный массив, где его длина равна количеству кругов, 0 круг -- внешний. Длина каждого элемента массива
    //равняется количеству секторов в круге. Т.е. если в 0 круге 18 секторов, то в 1 будет уже 17 итд. 0 сектор начинается вверх от 3 часов против часовой стрелки.
    void resetCircle(); //сбросить круг
    void drawArrow(QPainter& qp, double angle); //рисование стрелки 0 - 3часа, 90 - 12 часов, 180 - 9 часов, 270 - 6 часов.
    void removeArrow(); //удалить стрелку

    int CIRCLE_SIZE = 500;  //общий размер круга
    int COLORMAP_SIZE = 20; //толщина палитры

    int MAX_ROWS = 5;  //максимальное количество кругов
    QList<int> sectors_in_rows = {9, 8, 7, 6, 5}; //количество секторов в каждом круге, 0 индекс внешний круг
    void set_sectors_in_rows(QList<int>); //задать количество секторов в каждом круге

    QColor DEFAULT_COLOR_ARROW = QColor(255, 215, 0); //цвет стрелки
    int ARROW_SIZE = 2; //толщина стрелки
    void setAngle(double val) {angle = val;} // функция задания угла стрелки.


    QColor DEFAULT_COLOR = QColor(0, 0, 0); //стартовый цвет фона виджета


    QColor DEFAULT_COLOR_HIDE_ZONE = QColor("purple"); //цвет по умолчанию для зон скрытия
    void setHideZone(QList<QList<float>> array); //функция задания зон для скрытия. Например: array = {{0, 30}, {90, 10}, {66, 30}}; где 0 элемент угол, 1 элемент ширина сектора


private:
    void initUI();
    QList<QList<SectorClass>> initSectors();
    QList<QColor> getColormap(); //статичная фунция возвращающая массив цветов
    QColor getColor(double power);  //функция которая выдает цвет в зависимости от числа на вход (числа от 0 до 500, где 0 нет звука, 500 есть звук)
    QList<QList<float>> hide_zone;
    void processUdpData(); // временная функция для отладки поступления данных
    void updateMinPower(int value); //функция для слайдера минимума
    void updateMaxPower(int value); //функция для слайдера максимума
    void updateColorOnSliderMove(); //функция обновления цветов при изменениях на слайдерах
    void drawColormap(QPainter& qp, QList<double> powers); //рисование цветовой палитры
    void drawHideZone(QPainter& qp);
    QList<QList<SectorClass>> sectors_list; //двумерный массив, в котором хранятся данные по каждому сектору
    QUdpSocket udp_socket;
    double power;
    double min_power;
    double max_power;

    QList<QColor> colormap;
    QSlider* slider_min;
    QSlider* slider_max;
    QLabel* last_update_label;
    QLabel* error_label;
    QLabel* angle_label;
    double angle;

protected:
    void paintEvent(QPaintEvent* event);
};

#endif // MYWIDGET_H
