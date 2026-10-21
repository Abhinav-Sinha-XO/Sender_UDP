

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("UDP Packet Simulator");

    MainWindow window;
    window.show();

    return app.exec();
}
