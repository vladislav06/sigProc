//
// Created by vm on 24.30.10.
//

#include <QApplication>
#include "mainWindow/mainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow mainWidget;
    mainWidget.show();

    return QApplication::exec();
}