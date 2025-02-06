// filepath: /home/shadyskies/Desktop/dockstar/dockstar/src/main.cpp
#include <QApplication>
#include "dock/dock.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Dock dock;
    dock.show();

    return app.exec();
}