#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "glwidget.h"

namespace Ui {
    class MainWindow;
}
class HairInterface;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    HairInterface *m_hairInterface;
    GLWidget *m_glWidget;

};

#endif // MAINWINDOW_H
