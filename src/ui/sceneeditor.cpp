#include "glwidget.h"

#include "sceneeditor.h"
#include "ui_sceneeditor.h"

#include "scenewidget.h"
#include "QGridLayout"

SceneEditor::SceneEditor(GLWidget *parent, ObjMesh *mesh) :
    QMainWindow(parent),
    m_ui(new Ui::SceneEditor)
{
    m_ui->setupUi(this);

    // add controls
    m_ui->testHolder->setAlignment(Qt::AlignTop);
    m_ui->scrollArea->setWidget(m_ui->controlsBox);
    m_ui->scrollArea->setFrameShape(QFrame::NoFrame);
    
    // add interaction area
    QGridLayout *gridLayout = new QGridLayout(m_ui->centralwidget);
    m_sceneWidget = new SceneWidget(parent, mesh);
    gridLayout->addWidget(m_sceneWidget, 0, 1);
    
    // connect inputs
    connect(m_ui->showHideGroupMaps, SIGNAL(pressed()), this, SLOT(showHideGroupMaps()));
    connect(m_ui->showHideGroupBrush, SIGNAL(pressed()), this, SLOT(showHideGroupBrush()));    
            
    // brush radius
    connect(m_ui->sliderRadius, SIGNAL(valueChanged(int)), this, SLOT(setRadius(int)));
    connect(m_ui->inputRadius, SIGNAL(textChanged(QString)), this, SLOT(inputRadiusText(QString)));

    // color
    connect(m_ui->sliderBrushGrayscale, SIGNAL(valueChanged(int)), this, SLOT(setBrushGrayscale(int)));
    connect(m_ui->inputBrushGrayscale, SIGNAL(textChanged(QString)), this, SLOT(inputBrushGrayscaleText(QString)));

    // opacity
    connect(m_ui->sliderOpacity, SIGNAL(valueChanged(int)), this, SLOT(setOpacity(int)));
    connect(m_ui->inputOpacity, SIGNAL(textChanged(QString)), this, SLOT(inputOpacityText(QString)));
    
    
    connect(m_ui->falloffConstant, SIGNAL(clicked()), this, SLOT(setFalloffConstant()));
    connect(m_ui->falloffLinear, SIGNAL(clicked()), this, SLOT(setFalloffLinear()));
    connect(m_ui->falloffQuadratic, SIGNAL(clicked()), this, SLOT(setFalloffQuadratic()));
    
    connect(m_ui->clearWhiteButton, SIGNAL(pressed()), this, SLOT(clearWhite()));
    connect(m_ui->clearBlackButton, SIGNAL(pressed()), this, SLOT(clearBlack()));
    
    connect(m_ui->densityMapOption, SIGNAL(clicked()), this, SLOT(setDensityTexture()));
    connect(m_ui->directionMapOption, SIGNAL(clicked()), this, SLOT(setDirectionTexture()));    
    
    connect(m_ui->brushDirLeft, SIGNAL(clicked()), this, SLOT(setBrushDirLeft()));
    connect(m_ui->brushDirRight, SIGNAL(clicked()), this, SLOT(setBrushDirRight()));
    connect(m_ui->brushDirUp, SIGNAL(clicked()), this, SLOT(setBrushDirUp()));
    connect(m_ui->brushDirDown, SIGNAL(clicked()), this, SLOT(setBrushDirDown()));
    
    // buttons
    connect(m_ui->applyButton, SIGNAL(pressed()), this, SLOT(applyEditor()));
    connect(m_ui->applyAndCloseButton, SIGNAL(pressed()), this, SLOT(applyAndCloseEditor()));
    connect(m_ui->cancelButton, SIGNAL(pressed()), this, SLOT(cancelEditor()));
    
    
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
//    m_ui->groupMaps->hide();
    showHideGroupMaps();
    showHideGroupBrush();
    
    setDensityTexture();
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

void SceneEditor::setFalloffConstant(){
    m_sceneWidget->m_brushFalloffType = CONSTANT;
    m_sceneWidget->updateBrushSettings();
}

void SceneEditor::setFalloffLinear(){
    m_sceneWidget->m_brushFalloffType = LINEAR;
    m_sceneWidget->updateBrushSettings();
}

void SceneEditor::setFalloffQuadratic(){
    m_sceneWidget->m_brushFalloffType = QUADRATIC;
    m_sceneWidget->updateBrushSettings();
}

void SceneEditor::setBrushDirLeft(){
    m_sceneWidget->setBrushDir("left");
    m_sceneWidget->updateBrushSettings();
}
void SceneEditor::setBrushDirRight(){
    m_sceneWidget->setBrushDir("right");
    m_sceneWidget->updateBrushSettings();
}
void SceneEditor::setBrushDirUp(){
    m_sceneWidget->setBrushDir("up");
    m_sceneWidget->updateBrushSettings();
}
void SceneEditor::setBrushDirDown(){
    m_sceneWidget->setBrushDir("down");
    m_sceneWidget->updateBrushSettings();
}


void SceneEditor::clearWhite(){
    m_sceneWidget->clearTexture(255, 255, 255);
}
void SceneEditor::clearBlack(){
    m_sceneWidget->clearTexture(0, 0, 0);
}

void SceneEditor::setDensityTexture(){
    m_ui->labelBrushGrayscale->show();
    m_ui->inputBrushGrayscale->show();
    m_ui->sliderBrushGrayscale->show();
    m_ui->groupBrushDirection->hide();

    m_sceneWidget->m_currentTexture = m_sceneWidget->m_densityMapTexture;
}
void SceneEditor::setDirectionTexture(){
    m_ui->labelBrushGrayscale->hide();
    m_ui->inputBrushGrayscale->hide();
    m_ui->sliderBrushGrayscale->hide();
    m_ui->groupBrushDirection->show();
    m_ui->groupBrushDirection->move(m_ui->labelBrushGrayscale->pos());
    
    m_sceneWidget->m_currentTexture = m_sceneWidget->m_directionMapTexture;
}

void SceneEditor::applyEditor()
{
    m_sceneWidget->apply();
    m_sceneWidget->mainWidget->pause();
    m_sceneWidget->mainWidget->forceUpdate();
}
void SceneEditor::applyAndCloseEditor()
{
    m_sceneWidget->apply();
    this->close();
}
void SceneEditor::cancelEditor()
{
    this->close();
}

