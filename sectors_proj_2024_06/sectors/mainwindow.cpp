#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    MyWidget *centralWidget = new MyWidget(this);
    setGeometry(50, 50, 650, 650);
    setCentralWidget(centralWidget);
    centralWidget->setHideZone({{66, 30}});
}
