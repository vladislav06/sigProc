//
// Created by vm on 24.30.10.
//

#include <QApplication>
#include "mainWindow/mainWindow.h"
#include "src/threadPool/threadPool.h"

int main(int argc, char *argv[]) {
    ThreadPool::get().start();
    QApplication app(argc, argv);
    MainWindow mainWidget;
    mainWidget.show();
    QIcon::setThemeName("breeze-dark");

    return QApplication::exec();
}