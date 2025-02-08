#ifndef DOCK_H
#define DOCK_H

#include <QMainWindow>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMenu>

class Dock : public QMainWindow {
    Q_OBJECT

public:
    Dock(QWidget *parent = nullptr);
    ~Dock();
    void saveDockConfig();  // Moved from private to public

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void addNewIcon();
    void removeIcon();
    void loadDockConfig();

private:
    QHBoxLayout *layout;
    bool dragging;
    QPoint dragStartPosition;
    QLabel* clickedLabel;
    void adjustDockSize();
    void setupInitialDock();
    QString configFilePath;
    const int ICON_SIZE = 48;
};

#endif