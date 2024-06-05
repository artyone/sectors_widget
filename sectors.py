from PyQt5.QtWidgets import (
    QWidget, QApplication, QPushButton, QVBoxLayout, 
    QMainWindow, QHBoxLayout,  QSlider
)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPainter, QBrush, QPainterPath, QColor
from PyQt5.QtNetwork import QUdpSocket, QHostAddress
import math
import sys


#TODO сделать чтобы секторы обновлялись сразу все

class Sector:
    """
    Класс сектора круга
    """
    def __init__(self, size: float, sector: int, row: int, color: QColor) -> None:
        # инициализатор класса
        self.sector = sector # номер сектора на окружности
        self.row = row # номер ряда окружности
        self.color = color # цвет сектора
        self.outer_circle = row * size / 20 # стартовая точка внешней окружности (квадрата в которой вписывается окружность)
        self.inner_circle = self.outer_circle + size / 20 # стартовая точка внутренней окружности (квадрата в которой вписывается окружность)
        self.outer_diameter = size - row * size / 10 # диаметр внешней окружности сектора (сторона квадрата)
        self.inner_diameter = self.outer_diameter - size / 10 # диаметр внутренней окружности сектора (сторона квадрата)
        self.angle = 360 / (18 - row) # угол сектора
        self.start_x = size / 2 + math.cos(math.radians(sector * self.angle)) * (self.outer_diameter / 2) # стартовая точка х начала рисования
        self.start_y = size / 2 - math.sin(math.radians(sector * self.angle)) * (self.outer_diameter / 2) # стартовая точка у начала рисования

    def draw(self, qp):
        # рисуем сектор
        brush = QBrush(self.color) # кисть для заливки
        qp.setPen(self.color) # устанавливаем цвет контура
        qp.setBrush(brush) # устанавливаем цвет заливки
        path = QPainterPath() # создает объект рисования пути
        path.moveTo(self.start_x, self.start_y) # перемещаемся на стартовую точку рисования
        path.arcTo(
            self.outer_circle, 
            self.outer_circle, 
            self.outer_diameter, 
            self.outer_diameter, 
            self.sector * self.angle, 
            self.angle
        ) # рисуем внушнюю другу сектора
        path.arcTo(
            self.inner_circle, 
            self.inner_circle, 
            self.inner_diameter, 
            self.inner_diameter, 
            (self.sector + 1) * self.angle, 
            - self.angle
        ) # рисуем внутреннюю дугу сектора (две дуги соединяются автоматически)
        path.lineTo(self.start_x, self.start_y) # дорисовываем линию до стартовой точки
        qp.drawPath(path) # применяем на форму наш путь и цвета


class SectorWidget(QWidget):
    """ 
    Класс виджета сектора, наследник QWidget
    """
    def __init__(self) -> None:
        super().__init__() # вызываем инициализатор базового класса, чтобы не потерять свойства из базового класса
        self.sectors = [] # создаем пустой список секторов, для удобного последующего к ним обращения
        self.initSectors() # инициализируем секторы, чтобы они были уже с самого начала
        self.min_power = 0
        self.max_power = 500
        self.colormap = self.get_colormap()
        self.initUI()
    
    def initUI(self):
        self.slider_min = QSlider(Qt.Horizontal)
        self.slider_min.setMinimum(0)
        self.slider_min.setMaximum(480)
        self.slider_min.setValue(self.min_power)
        self.slider_min.valueChanged.connect(self.update_min_power)

        self.slider_max = QSlider(Qt.Horizontal)
        self.slider_max.setMinimum(20)
        self.slider_max.setMaximum(500)
        self.slider_max.setValue(self.max_power)
        self.slider_max.valueChanged.connect(self.update_max_power)

        layout = QVBoxLayout(self)
        layout.addWidget(self.slider_min)
        layout.addWidget(self.slider_max)
        layout.setAlignment(Qt.AlignBottom)
    
    def update_min_power(self, value):
        if self.slider_max.value() <= value + 20:
            self.slider_max.setValue(value + 20)
            self.max_power = value + 20
        self.min_power = value

    def update_max_power(self, value):
        if self.slider_min.value() >= value - 20:
            self.slider_min.setValue(value - 20)
            self.max_power = value - 20
        self.max_power = value


    def initSectors(self) -> None:
        for row in range(8):
            self.sectors.append([])  # итерируемся с 0 до 7, всего 8 рядов
            for sector in range(18 - row): # итерируемся с 0 до 17 - row, 0 ряд - 18 секторов, 7 ряд 11 секторов
                self.sectors[row].append(Sector(500, sector, row, QColor(255, 255, 255))) # добавляем в словарь секторы, где ключ это (ряд, сектор), а значение сам объект сектор

    def paintEvent(self, e) -> None:
        qp = QPainter(self) # создаем объект QPainter для рисования
        qp.setRenderHint(QPainter.Antialiasing) # устанавливаем сглаживание
        for row in self.sectors:
            for sector in row:
                sector.draw(qp) # рисуем сектор через метод класса


    def update_sector_color(self, data) -> None:
        for row, value in enumerate(self.sectors):
            for col, sector in enumerate(value):
                sector.color = QColor(*self.get_color(data[row][col]))
        self.update() # меняем цвет атрибут цвет у класса


    def get_color(self, power):
        # Обрезаем значения power, чтобы они находились в диапазоне [min_power, max_power]
        power = max(self.min_power, min(self.max_power, power))
        if self.max_power == self.min_power:
            return self.colormap[0]
        # Вычисляем индекс для выбора цвета из карты
        index = int(
            (power - self.min_power) / 
            (self.max_power - self.min_power) * 
            (len(self.colormap) - 1)
        )

        return self.colormap[index]

    @staticmethod
    def get_colormap():
        colormap = [(255, 255, 250 - i) for i in range(1, 251)]
        colormap += [(255, 255-i, 0) for i in range(1, 251)]

        return colormap



class MainApp(QMainWindow):
    '''
    Класс главного окна
    '''
    def __init__(self):
        super().__init__() # вызываем инициализатор базового класса, чтобы не потерять свойства из базового класса
        self.sector_widgets = [] # создаем пустой список виджетов секторов (это сделано для того чтобы можно было добавить больше 1 виджета)
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Main Application') # устанавливаем заголовок
        self.setGeometry(100, 100, 550, 700)  # устанавливаем размеры

        central_widget = QWidget(self) # создаем виджет для размещения всех виджетов
        self.setCentralWidget(central_widget) # устанавливаем виджет главного окна
        layout = QVBoxLayout(central_widget) # создаем вертикальный макет

        widget_for_sectors = QWidget() # создаем виджет для размещения секторов
        layout_for_sectors = QHBoxLayout(widget_for_sectors) # создаем горизонтальный макет для размещения виджетов секторов

        # Создаем один (может быть больше) экземпляр SectorWidget и добавляем их в макет
        for _ in range(1):
            sector_widget = SectorWidget()
            layout_for_sectors.addWidget(sector_widget)
            self.sector_widgets.append(sector_widget) # добавляем в список виджетов, чтобы можно было обращаться к ним

        self.udp_socket = QUdpSocket(self) # создаем объект UDP-сокет
        self.is_receiving = False  # переменная состояния приема данных
        self.start_stop_button = QPushButton('Старт', self) # создаем кнопку старта
        self.start_stop_button.clicked.connect(self.toggle_receiving) # при нажатии на кнопку старт выполняется функция

        layout.addWidget(widget_for_sectors) # добавляем виджет, который содержит виджеты секторов
        layout.addWidget(self.start_stop_button) # добавляем кнопку
        central_widget.setLayout(layout) # устанавливаем макет главного окна

    def process_udp_data(self):
         # фукнция получения данных, это временная реализация всего лишь для проверки
        while self.udp_socket.hasPendingDatagrams():
            datagram, host, port = self.udp_socket.readDatagram(self.udp_socket.pendingDatagramSize())
            data = datagram.decode()
            try:
                data = data.split(':')
                data = [[int(x) for x in y.split(',')] for y in data]
                for widget in self.sector_widgets:
                    widget.update_sector_color(data)
            except ValueError:
                pass

    def toggle_receiving(self):
        if self.is_receiving:
            self.is_receiving = False
            self.udp_socket.close()
            self.start_stop_button.setText('Старт')
        else:
            self.is_receiving = True
            self.udp_socket.bind(QHostAddress.AnyIPv4, 35393)
            self.udp_socket.readyRead.connect(self.process_udp_data)
            self.start_stop_button.setText('Стоп')
    

if __name__ == '__main__':
    app = QApplication(sys.argv)
    main_app = MainApp()
    main_app.show()
    sys.exit(app.exec_())




