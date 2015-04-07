#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hairinterface.h"
#include <QSettings>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    QGLFormat qglFormat;
    qglFormat.setVersion(4,2);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    m_ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(m_ui->centralWidget);

    m_hairInterface = new HairInterface(m_ui);
    m_glWidget = new GLWidget(qglFormat, m_hairInterface, this);
    
    gridLayout->addWidget(m_glWidget, 0, 1);
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_hairInterface;
    delete m_glWidget;
}
