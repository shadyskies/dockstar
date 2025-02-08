#include <QApplication>
#include "dock/dock.h"
#include <signal.h>

Dock* globalDock = nullptr; // Global pointer to access dock from signal handler

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        if (globalDock) {
            globalDock->saveDockConfig();
        }
        QApplication::quit();
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set up signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    Dock dock;
    globalDock = &dock;
    dock.show();
    
    int result = app.exec();
    
    globalDock = nullptr;
    return result;
}