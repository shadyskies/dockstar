#include "dock.h"
#include <QScreen>
#include <QApplication>
#include <QGuiApplication>
#include <QPixmap>
#include <QFileDialog>
#include <QContextMenuEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

Dock::Dock(QWidget *parent) : QMainWindow(parent), dragging(false) {
    configFilePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) 
                    + "/dockstar/config.json";
    
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    QWidget *widget = new QWidget(this);
    widget->setStyleSheet("QWidget { background-color: rgba(128, 128, 128, 128); border: 2px solid red; border-radius: 5px; }");
    
    layout = new QHBoxLayout(widget);
    layout->setSpacing(0);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setAlignment(Qt::AlignCenter);
    
    setCentralWidget(widget);
    
    QFile configFile(configFilePath);
    if (!configFile.exists()) {
        setupInitialDock();
    } else {
        loadDockConfig();
    }
    
    // Connect quit signal to save config
    connect(qApp, &QApplication::aboutToQuit, this, &Dock::saveDockConfig);
}

Dock::~Dock() {
    saveDockConfig();
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
        label->setProperty("iconPath", iconPath);
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
    int dockHeight = ICON_SIZE + 20; // Fixed height based on icon size plus padding
    
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenHeight = screenGeometry.height();
    
    setGeometry((screenGeometry.width() - newWidth) / 2,
                screenHeight - dockHeight - 50,
                newWidth,
                dockHeight);
}

void Dock::saveDockConfig() {
    QJsonArray iconPaths;
    
    for(int i = 0; i < layout->count(); i++) {
        QLayoutItem* item = layout->itemAt(i);
        if(QLabel* label = qobject_cast<QLabel*>(item->widget())) {
            QString path = label->property("iconPath").toString();
            if(!path.isEmpty()) {
                iconPaths.append(path);
            }
        }
    }
    
    QJsonObject config;
    config["icons"] = iconPaths;
    config["position"] = QJsonObject{
        {"x", x()},
        {"y", y()}
    };
    
    QDir().mkpath(QFileInfo(configFilePath).path());
    QFile file(configFilePath);
    if(file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(config).toJson());
    }
}

void Dock::loadDockConfig() {
    QFile file(configFilePath);
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    // Clear existing icons
    while (layout->count() > 0) {
        QLayoutItem* item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
    
    // Load saved config
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject config = doc.object();
    
    // Load position
    QJsonObject pos = config["position"].toObject();
    move(pos["x"].toInt(), pos["y"].toInt());
    
    // Load icons
    QJsonArray icons = config["icons"].toArray();
    for(const QJsonValue &icon : icons) {
        QString path = icon.toString();
        QLabel *label = new QLabel(centralWidget());
        QPixmap pixmap(path);
        if(!pixmap.isNull()) {
            pixmap = pixmap.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            label->setPixmap(pixmap);
            label->setProperty("iconPath", path);
            label->setAlignment(Qt::AlignCenter);
            if(layout->count() > 0) {
                layout->addStretch(1);
            }
            layout->addWidget(label);
        }
    }
    
    adjustDockSize();
}

void Dock::setupInitialDock() {
    // Default icon paths
    QStringList iconPaths = {
        "/usr/share/icons/hicolor/scalable/apps/firefox.svg",
        "/usr/share/icons/breeze/apps/48/konsole.svg",
        "/usr/share/icons/breeze/apps/48/system-file-manager.svg",
        "/usr/share/icons/breeze/apps/48/kate.svg",
        "/usr/share/icons/breeze/apps/48/systemsettings.svg"
    };
    
    for (const QString& path : iconPaths) {
        QLabel *label = new QLabel(centralWidget());
        QPixmap pixmap(path);
        if (!pixmap.isNull()) {
            pixmap = pixmap.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            label->setPixmap(pixmap);
            label->setProperty("iconPath", path);
            label->setAlignment(Qt::AlignCenter);
            if (layout->count() > 0) {
                layout->addStretch(1);
            }
            layout->addWidget(label);
        }
    }
    
    adjustDockSize();
}