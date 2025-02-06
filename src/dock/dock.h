#ifndef DOCK_H
#define DOCK_H

#include <QMainWindow>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

class Dock : public QMainWindow {
    Q_OBJECT

public:
    Dock(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QHBoxLayout *layout;
    bool dragging;
    QPoint dragStartPosition;
};

#endif 