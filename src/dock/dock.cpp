#include "dock.h"
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QFileDialog>
#include <QContextMenuEvent>

Dock::Dock(QWidget *parent) : QMainWindow(parent), dragging(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // Create main widget with background and border
    QWidget *widget = new QWidget(this);
    widget->setStyleSheet("QWidget { background-color: rgba(128, 128, 128, 128); border: 2px solid red; border-radius: 5px; }");
    
    layout = new QHBoxLayout(widget);
    layout->setSpacing(0);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setAlignment(Qt::AlignCenter);
    
    // Define icon paths and sizes
    const int ICON_SIZE = 48;
    QStringList iconPaths = {
        "/usr/share/icons/hicolor/scalable/apps/firefox.svg",
        "/usr/share/icons/breeze/apps/48/konsole.svg",
        "/usr/share/icons/breeze/apps/48/system-file-manager.svg",
        "/usr/share/icons/breeze/apps/48/kate.svg",
        "/usr/share/icons/breeze/apps/48/systemsettings.svg"
    };
    
    // Add icons
    int i = 0;  // Add counter variable
    for (const QString& path : iconPaths) {
        if (i > 0) {
            layout->addStretch(1);
        }
        QLabel *label = new QLabel(widget);
        QPixmap pixmap(path);
        if (pixmap.isNull()) {
            // Fallback if icon not found
            pixmap = QPixmap(ICON_SIZE, ICON_SIZE);
            pixmap.fill(Qt::gray);
        } else {
            pixmap = pixmap.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        i++; // Increment counter
    }

    widget->setLayout(layout);
    setCentralWidget(widget);

    // Position the dock at the bottom of the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int dockWidth = 400; // Fixed width
    int dockHeight = 80; // Reduced height
    setGeometry((screenWidth - dockWidth) / 2, // Center horizontally
                screenHeight - dockHeight - 50, // Leave some space from bottom
                dockWidth, 
                dockHeight);
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

void Dock::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    
    // Get the widget under cursor
    QWidget* widget = childAt(event->pos());
    QLabel* label = qobject_cast<QLabel*>(widget);
    
    if (label) {
        // Right-click on icon
        clickedLabel = label;
        QAction *removeIcon = menu.addAction("Remove Icon");
        connect(removeIcon, &QAction::triggered, this, &Dock::removeIcon);
    } else {
        // Right-click on dock
        QAction *addIcon = menu.addAction("Add Icon");
        connect(addIcon, &QAction::triggered, this, &Dock::addNewIcon);
    }
    
    menu.exec(event->globalPos());
}

void Dock::addNewIcon() {
    QString iconPath = QFileDialog::getOpenFileName(this,
        tr("Select Icon"), "/usr/share/icons/", tr("Images (*.png *.xpm *.jpg *.svg)"));
    
    if (!iconPath.isEmpty()) {
        if (layout->count() > 0) {
            layout->addStretch(1);
        }
        
        QLabel *label = new QLabel(centralWidget());
        QPixmap pixmap(iconPath);
        if (pixmap.isNull()) {
            pixmap = QPixmap(ICON_SIZE, ICON_SIZE);
            pixmap.fill(Qt::gray);
        } else {
            pixmap = pixmap.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        
        adjustDockSize();
    }
}

void Dock::removeIcon() {
    if (clickedLabel) {
        // Remove the stretch before or after the label
        int index = layout->indexOf(clickedLabel);
        if (index > 0) {
            QLayoutItem* stretch = layout->takeAt(index - 1);
            delete stretch;
        }
        
        // Remove and delete the label
        layout->removeWidget(clickedLabel);
        delete clickedLabel;
        
        adjustDockSize();
    }
}

void Dock::adjustDockSize() {
    int iconCount = layout->count() / 2 + 1; // Account for stretch items
    int newWidth = (ICON_SIZE + 20) * iconCount + 20; // 20px padding per icon + 20px total margin
    
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenHeight = screenGeometry.height();
    
    setGeometry((screenGeometry.width() - newWidth) / 2,
                screenHeight - height() - 50,
                newWidth,
                height());
}