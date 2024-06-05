#include "mywidget.h"
#include <QColor>
#include <QtWidgets>
#include <QColor>
#include <QPainter>
#include <QMapIterator>

QList<QList<SectorClass>> MyWidget::initSectors()
{

    QList<QList<SectorClass>> sectors_list;
    for (int row = 0; row < MAX_ROWS; ++row) {
        QList<SectorClass> new_sector_list;
        sectors_list.append(new_sector_list);
        for (int col = 0; col < sectors_in_rows[row]; ++col) {
            sectors_list[row].append(SectorClass(CIRCLE_SIZE, row, col, QColor(DEFAULT_COLOR), 0, sectors_in_rows[row]));
        }
    }
    return sectors_list;
}

void MyWidget::updateSectorColorFromData(QList<QList<double>> data)
{

    if (sectors_list.size() > data.size()) {
        error_label->setText("Ошибка длины в данных входящего массива. sectors_list.size() > data.size()");
        resetCircle();
    }
    else {
        for (int row = 0; row < MAX_ROWS; ++row) {
            if (sectors_list[row].size() > data[row].size()) {
                error_label->setText("Ошибка длины в данных входящего массива. sectors_list[row].size() > data[row].size()");
                resetCircle();
            } else {
                for (int col = 0; col < sectors_in_rows[row]; ++col) {
                    sectors_list[row][col].color = getColor(data[row][col]);
                    sectors_list[row][col].power = data[row][col];

                    error_label->setText("");
                }
            }
        }
        QDateTime currentTime = QDateTime::currentDateTime();
        last_update_label->setText("Последнее обновление: " + currentTime.toString("hh:mm:ss"));
        update();
    }

}

void MyWidget::resetCircle()
{
    sectors_list = initSectors();
    update();
}

void MyWidget::processUdpData() {
    while (udp_socket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udp_socket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udp_socket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString data = QString::fromUtf8(datagram);

        QStringList dataParts = data.split(':');

        QList<QList<double>> parsedData;
        for (const QString& part : dataParts) {
            QStringList values = part.split(',');
            QList<double> row;
            for (const QString& value : values) {
                bool ok;
                double doubleValue = value.toDouble(&ok);
                if (ok) {
                    row.append(doubleValue);
                } else {
                    // Handle conversion error if needed
                }
            }
            parsedData.append(row);
        }
        angle = qrand() % 361;
        updateSectorColorFromData(parsedData);
    }
}

QList<QColor> MyWidget::getColormap()
{
    QList<QColor> colormap;

    for (int i = 1; i <= 250; ++i) {
        colormap.append(QColor(0, 0 + i, 0));
    }

    for (int i = 1; i <= 250; ++i) {
        colormap.append(QColor(0, 255 - i, 0 + i));
    }


    return colormap;
}

QColor MyWidget::getColor(double power)
{
    power = qMax(min_power, qMin(max_power, power));
    if (max_power - min_power == 0) {
        return colormap[0];
    }

    int index = qRound((power - min_power) / (max_power - min_power) * (colormap.size() - 1));
    return colormap[index];
}

void MyWidget::updateColorOnSliderMove()
{
    for (int row = 0; row < MAX_ROWS; ++row) {
        for (int col = 0; col < sectors_in_rows[row]; ++col) {
            sectors_list[row][col].color = getColor(sectors_list[row][col].power);
        }
    }

    update();
}

void MyWidget::updateMinPower(int value)
{
    if (slider_max->value() <= value + 20) {
        slider_max->setValue(value + 20);
        max_power = value + 20;
    }
    min_power = value;
    updateColorOnSliderMove();
}

void MyWidget::updateMaxPower(int value)
{
    if (slider_min->value() >= value - 20) {
        slider_min->setValue(value - 20);
        min_power = value - 20;
    }
    max_power = value;
    updateColorOnSliderMove();
}

void MyWidget::initUI()
{
    setStyleSheet("color: white;");
    error_label = new QLabel(this);

    angle_label = new QLabel(this);
    angle_label->setText("Угол: None");
    QFont font = angle_label->font();
    font.setPointSize(16);
    angle_label->setFont(font);


    last_update_label = new QLabel(this);
    last_update_label->setText("Последнее обновление: None");

    slider_min = new QSlider(Qt::Horizontal);
    slider_min->setMinimum(0);
    slider_min->setMaximum(480);
    slider_min->setValue(min_power);
    connect(slider_min, &QSlider::valueChanged, this, &MyWidget::updateMinPower);

    slider_max = new QSlider(Qt::Horizontal);
    slider_max->setMinimum(20);
    slider_max->setMaximum(500);
    slider_max->setValue(max_power);
    connect(slider_max, &QSlider::valueChanged, this, &MyWidget::updateMaxPower);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(error_label);
    layout->addWidget(angle_label);
    layout->addWidget(last_update_label);
    layout->addWidget(slider_max);
    layout->addWidget(slider_min);
    layout->setAlignment(Qt::AlignBottom);
}

void MyWidget::drawColormap(QPainter& qp, QList<double> powers)
{
    qSort(powers.begin(), powers.end(), qGreater<double>());

    QLinearGradient gradient(CIRCLE_SIZE + 50, 0, CIRCLE_SIZE + 50 + COLORMAP_SIZE, CIRCLE_SIZE);

    int numColors = powers.size();
    for (int i = 0; i < numColors; ++i) {
        double fraction = static_cast<double>(i) / (numColors - 1);
        QColor color = getColor(powers[i]);
        gradient.setColorAt(fraction, color);
    }

    qp.setBrush(gradient);
    qp.drawRect(CIRCLE_SIZE + 50, 0, COLORMAP_SIZE, CIRCLE_SIZE);
}

void MyWidget::drawArrow(QPainter& qp, double angle)
{
    if (angle != -99999) {
        int centerX = CIRCLE_SIZE / 2;
        int centerY = CIRCLE_SIZE / 2;
        int arrowLength = CIRCLE_SIZE / 2;

        QColor color = DEFAULT_COLOR_ARROW;
        QBrush brush(color);
        qp.setPen(QColor(0, 0, 0));
        qp.setBrush(brush);

        qp.translate(centerX, centerY);
        qp.rotate(-angle);

        QPolygonF arrow;
        arrow << QPointF(0, -ARROW_SIZE) << QPointF(0, ARROW_SIZE) << QPointF(arrowLength, ARROW_SIZE) << QPointF(arrowLength, -ARROW_SIZE);
        qp.drawPolygon(arrow);

        qp.rotate(angle);
        qp.translate(-centerX, -centerY);

        angle_label->setText("Угол: " + QString::number(angle));
    }
}

void MyWidget::removeArrow()
{
    angle = -99999;
    angle_label->setText("Угол: None");
    update();

}

void MyWidget::setHideZone(QList<QList<float>> array)
{
    hide_zone = array;
    update();
}

void MyWidget::drawHideZone(QPainter &qp)
{
    qp.setBrush(DEFAULT_COLOR_HIDE_ZONE);
    qp.setPen(DEFAULT_COLOR_HIDE_ZONE);
    QRectF rectangle(0, 0, CIRCLE_SIZE, CIRCLE_SIZE);

    float angle;
    float width;
    int startAngle; // Начальный угол (в 1/16 градуса)
    int spanAngle; // Угол охвата (в 1/16 градуса)

    for (int i = 0; i < hide_zone.size(); i++) {
        if (hide_zone[i].size() < 2) {
            error_label->setText("Неправильно задан массив данных для зон скрытия");
        }
        angle = hide_zone[i][0];
        width = hide_zone[i][1];
        if (width > 0) {
            startAngle = (angle - width / 2) * 16;
            spanAngle = (width) * 16;
            qp.drawPie(rectangle, startAngle, spanAngle);
        }
    }
}

void MyWidget::set_sectors_in_rows(QList<int> sectors)
{
    if (sectors.size() < 1) {
        error_label->setText("Количество рядов не может быть меньше 1");
        return;
    }
    MAX_ROWS = sectors.size();
    sectors_in_rows = sectors;
    sectors_list = initSectors();
    update();
}


MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{
    sectors_list = initSectors();
    connect(&udp_socket, &QUdpSocket::readyRead, this, &MyWidget::processUdpData);
    udp_socket.bind(QHostAddress::Any, 35393);
    min_power = 0;
    max_power = 500;
    colormap = getColormap();
    angle = -99999;
    initUI();
}

void MyWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter qp(this);
    qp.setRenderHint(QPainter::Antialiasing);
    qp.fillRect(qp.viewport(), QBrush(DEFAULT_COLOR));
    QList<double> powers;
    for (QList<SectorClass>& row : sectors_list) {
        for (SectorClass& value : row) {
            value.draw(qp);
            powers.append(value.power);
        }
    }

    qp.setPen(QColor("black"));
    qp.setBrush(Qt::NoBrush);
    qp.drawEllipse(0, 0, CIRCLE_SIZE, CIRCLE_SIZE);
    drawColormap(qp, powers);
    drawHideZone(qp);
    drawArrow(qp, angle);
}

