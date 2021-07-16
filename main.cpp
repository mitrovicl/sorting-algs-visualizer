#include "sortvisualizer.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.show();
    //qDebug() << window.size() << "\n";
    window.setFixedSize(window.size());

    return a.exec();
}
