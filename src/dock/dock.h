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

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void addNewIcon();
    void removeIcon();

private:
    QHBoxLayout *layout;
    bool dragging;
    QPoint dragStartPosition;
    QLabel* clickedLabel;
    void adjustDockSize();
    const int ICON_SIZE = 48;
};

#endif