#include "glwidget.h"

#include "sceneeditor.h"
#include "ui_sceneeditor.h"

#include "scenewidget.h"
#include "QGridLayout"

SceneEditor::SceneEditor(GLWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::SceneEditor)
{
    m_ui->setupUi(this);

    // add controls
    m_ui->testHolder->setAlignment(Qt::AlignTop);
    m_ui->scrollArea->setWidget(m_ui->controlsBox);
    m_ui->controlsBox->setStyleSheet("QGroupBox { background-color: none; }");
    m_ui->scrollArea->setFrameShape(QFrame::NoFrame);
    
    // add interaction area
    QGridLayout *gridLayout = new QGridLayout(m_ui->centralwidget);
    m_sceneWidget = new SceneWidget(parent);
    gridLayout->addWidget(m_sceneWidget, 0, 1);
    
    
    // connect inputs
    connect(m_ui->showHideGroupMaps, SIGNAL(pressed()), this, SLOT(showHideGroupMaps()));
    connect(m_ui->showHideGroupBrush, SIGNAL(pressed()), this, SLOT(showHideGroupBrush()));
//    connect(m_ui->showHideGroupRender, SIGNAL(pressed()), this, SLOT(showHideGroupRender()));
    
            
    // brush radius
    connect(m_ui->sliderRadius, SIGNAL(valueChanged(int)), this, SLOT(setRadius(int)));
    connect(m_ui->inputRadius, SIGNAL(textChanged(QString)), this, SLOT(inputRadiusText(QString)));

    // color
    connect(m_ui->sliderBrushGrayscale, SIGNAL(valueChanged(int)), this, SLOT(setBrushGrayscale(int)));
    connect(m_ui->inputBrushGrayscale, SIGNAL(textChanged(QString)), this, SLOT(inputBrushGrayscaleText(QString)));

    // opacity
    connect(m_ui->sliderOpacity, SIGNAL(valueChanged(int)), this, SLOT(setOpacity(int)));
    connect(m_ui->inputOpacity, SIGNAL(textChanged(QString)), this, SLOT(inputOpacityText(QString)));
    
    /*
    // toggles
    connect(m_ui->frictionSimCheckBox, SIGNAL(toggled(bool)), this, SLOT(setFrictionSim(bool)));
    connect(m_ui->shadowCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShadows(bool)));
    connect(m_ui->supersampleCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSupersampling(bool)));
*/
    
    // buttons
    connect(m_ui->applyButton, SIGNAL(pressed()), this, SLOT(applyEditor()));
//    connect(m_ui->cancelButton, SIGNAL(pressed()), this, SLOT(cancelEditor()));
    
    
    // init ui values
    m_ui->sliderRadius->setValue(m_sceneWidget->m_radius);
    m_ui->inputRadius->setText(QString::number(m_sceneWidget->m_radius));

    // Sync brush color slider
    m_ui->sliderBrushGrayscale->setValue(m_sceneWidget->m_grayscale*2550);
    m_ui->inputBrushGrayscale->setText(QString::number(m_sceneWidget->m_grayscale, 'g', 2));

    // Sync opacity slider
    m_ui->sliderOpacity->setValue(m_sceneWidget->m_opacity*2550);
    m_ui->inputOpacity->setText(QString::number(m_sceneWidget->m_opacity, 'g', 2));



    // open/close groups
//    showHideGroupMaps(); 
    m_ui->groupMaps->hide();
    showHideGroupBrush();
    
}

SceneEditor::~SceneEditor()
{
    delete m_ui;
}



void SceneEditor::showHideGroupMaps()
{
    if (m_ui->groupMaps->isVisible()){
        m_ui->groupMaps->hide();
        m_ui->showHideGroupMaps->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupMaps->show();
        m_ui->showHideGroupMaps->setIcon(QIcon(":/images/chevron_down"));
    }
}

void SceneEditor::showHideGroupBrush()
{
    if (m_ui->groupBrush->isVisible()){
        m_ui->groupBrush->hide();
        m_ui->showHideGroupBrush->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupBrush->show();
        m_ui->showHideGroupBrush->setIcon(QIcon(":/images/chevron_down"));
    }
}

void SceneEditor::inputRadiusText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    int value = text.toInt(&ok);
    if (!ok){
        value = m_sceneWidget->m_radius;
    } else if (value == m_sceneWidget->m_radius) return;
    setRadius(value);
}
void SceneEditor::setRadius(int value)
{
    m_sceneWidget->m_radius = value;
    m_ui->inputRadius->setText(QString::number(value));
    m_ui->sliderRadius->setValue(value);
    
    m_sceneWidget->updateBrushSettings();
}


void SceneEditor::inputBrushGrayscaleText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_sceneWidget->m_grayscale;
    } else if (value == m_sceneWidget->m_grayscale) return;
    setBrushGrayscale(2550*value);
    m_ui->sliderBrushGrayscale->setValue(2550*value);
}

void SceneEditor::setBrushGrayscale(int value)
{
    if (value < 0 || value > 2550) return;
    m_sceneWidget->m_grayscale = value/2550.;
    m_ui->inputBrushGrayscale->setText(QString::number(m_sceneWidget->m_grayscale, 'g', 3));
}
void SceneEditor::inputOpacityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_sceneWidget->m_opacity;
    } else if (value == m_sceneWidget->m_opacity) return;
    setOpacity(2550*value);
    m_ui->sliderOpacity->setValue(2550*value);
}
void SceneEditor::setOpacity(int value)
{
    if (value < 0 || value > 2550) return;
    m_sceneWidget->m_opacity = value/2550.;
    m_ui->inputOpacity->setText(QString::number(m_sceneWidget->m_opacity, 'g', 3));
}


void SceneEditor::applyEditor()
{
    m_sceneWidget->apply();
}
void SceneEditor::cancelEditor()
{
    
}

