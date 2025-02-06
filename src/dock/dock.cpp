#include "dock.h"
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>

Dock::Dock(QWidget *parent) : QMainWindow(parent), dragging(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // Set a red outline for the dock window
    setStyleSheet("QMainWindow { border: 1px solid red; background-color: rgba(255, 255, 255, 0.8); }");

    QWidget *widget = new QWidget(this);
    layout = new QHBoxLayout(widget);
    // layout->setSpacing(); // Set spacing between blocks to 1 pixel

    // Add placeholder images
    for (int i = 0; i < 5; ++i) {
        QLabel *label = new QLabel(widget);
        QPixmap pixmap(50, 50); // Placeholder size
        pixmap.fill(Qt::gray); // Placeholder color
        label->setPixmap(pixmap);
        layout->addWidget(label);
    }

    widget->setLayout(layout);
    setCentralWidget(widget);

    // Position the dock at the bottom of the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int dockHeight = 100; // Adjust the height as needed
    setGeometry(0, screenHeight - dockHeight, screenWidth, dockHeight);
}

void Dock::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Dock::mouseMoveEvent(QMouseEvent *event) {
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - dragStartPosition);
        event->accept();
    }
}

void Dock::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        event->accept();
    }
}