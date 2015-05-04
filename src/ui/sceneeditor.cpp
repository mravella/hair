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
    connect(m_ui->showHideGroupGlobals, SIGNAL(pressed()), this, SLOT(showHideGroupGlobals()));
    connect(m_ui->sliderRadius, SIGNAL(valueChanged(int)), this, SLOT(setRadius(int)));
    connect(m_ui->inputRadius, SIGNAL(textChanged(QString)), this, SLOT(inputRadiusText(QString)));
    connect(m_ui->sliderBrushGrayscale, SIGNAL(valueChanged(int)), this, SLOT(setBrushGrayscale(int)));
    connect(m_ui->inputBrushGrayscale, SIGNAL(textChanged(QString)), this, SLOT(inputBrushGrayscaleText(QString)));
    connect(m_ui->sliderOpacity, SIGNAL(valueChanged(int)), this, SLOT(setOpacity(int)));
    connect(m_ui->inputOpacity, SIGNAL(textChanged(QString)), this, SLOT(inputOpacityText(QString)));
    connect(m_ui->falloffConstant, SIGNAL(clicked()), this, SLOT(setFalloffConstant()));
    connect(m_ui->falloffLinear, SIGNAL(clicked()), this, SLOT(setFalloffLinear()));
    connect(m_ui->falloffQuadratic, SIGNAL(clicked()), this, SLOT(setFalloffQuadratic()));
    connect(m_ui->clearButton, SIGNAL(pressed()), this, SLOT(clearTexture()));
    connect(m_ui->densityMapOption, SIGNAL(clicked()), this, SLOT(setDensityTexture()));
    connect(m_ui->directionMapOption, SIGNAL(clicked()), this, SLOT(setDirectionTexture()));    
    connect(m_ui->brushDirLeft, SIGNAL(clicked()), this, SLOT(setBrushDirLeft()));
    connect(m_ui->brushDirRight, SIGNAL(clicked()), this, SLOT(setBrushDirRight()));
    connect(m_ui->brushDirUp, SIGNAL(clicked()), this, SLOT(setBrushDirUp()));
    connect(m_ui->brushDirDown, SIGNAL(clicked()), this, SLOT(setBrushDirDown()));
    connect(m_ui->sliderGlobalDensity, SIGNAL(valueChanged(int)), this, SLOT(setGlobalDensity(int)));
    connect(m_ui->inputGlobalDensity, SIGNAL(textChanged(QString)), this, SLOT(inputGlobalDensityText(QString)));
    connect(m_ui->sliderGlobalLength, SIGNAL(valueChanged(int)), this, SLOT(setGlobalLength(int)));
    connect(m_ui->inputGlobalLength, SIGNAL(textChanged(QString)), this, SLOT(inputGlobalLengthText(QString)));
    
    // buttons
    connect(m_ui->applyButton, SIGNAL(pressed()), this, SLOT(applyEditor()));
    connect(m_ui->applyAndCloseButton, SIGNAL(pressed()), this, SLOT(applyAndCloseEditor()));
    connect(m_ui->cancelButton, SIGNAL(pressed()), this, SLOT(cancelEditor()));
    
    // INIT UI VALUES
    m_ui->sliderRadius->setValue(m_sceneWidget->m_radius);
    m_ui->inputRadius->setText(QString::number(m_sceneWidget->m_radius));
    m_ui->sliderBrushGrayscale->setValue(m_sceneWidget->m_grayscale*2550);
    m_ui->inputBrushGrayscale->setText(QString::number(m_sceneWidget->m_grayscale, 'g', 2));
    m_ui->sliderOpacity->setValue(m_sceneWidget->m_opacity*2550);
    m_ui->inputOpacity->setText(QString::number(m_sceneWidget->m_opacity, 'g', 2));
    m_ui->sliderGlobalDensity->setValue(m_sceneWidget->mainWidget->m_hairDensity);
    m_ui->inputGlobalDensity->setText(QString::number(m_sceneWidget->mainWidget->m_hairDensity, 'g', 3));
    m_ui->sliderGlobalLength->setValue(m_sceneWidget->mainWidget->m_maxHairLength*100);
    m_ui->inputGlobalLength->setText(QString::number(m_sceneWidget->mainWidget->m_maxHairLength, 'g', 3));
    
    
    // open/close groups
    m_ui->groupMaps->hide();
//    showHideGroupMaps();
    
    m_ui->groupGlobals->hide();
//    showHideGroupGlobals();
    showHideGroupBrush();
    
    setDensityTexture(); // to make sure correct controls are displayed
//    setDirectionTexture();
}

SceneEditor::~SceneEditor()
{
    delete m_ui;
}

void SceneEditor::closeEvent(QCloseEvent *event)
{
    m_sceneWidget->mainWidget->m_sceneEditor = NULL;
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
void SceneEditor::showHideGroupGlobals()
{
    if (m_ui->groupGlobals->isVisible()){
        m_ui->groupGlobals->hide();
        m_ui->showHideGroupGlobals->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupGlobals->show();
        m_ui->showHideGroupGlobals->setIcon(QIcon(":/images/chevron_down"));
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

void SceneEditor::clearTexture(){
    if (m_sceneWidget->m_currentTexture == m_sceneWidget->m_densityMapTexture){
        m_sceneWidget->clearTexture(0, 0, 0);
    } else if (m_sceneWidget->m_currentTexture == m_sceneWidget->m_directionMapTexture){
        m_sceneWidget->clearTexture(128, 128, 255);
    }
}

void SceneEditor::inputGlobalDensityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    int value = text.toInt(&ok);
    if (!ok){
        value = m_sceneWidget->mainWidget->m_hairDensity;
    } else if (value == m_sceneWidget->mainWidget->m_hairDensity) return;
    setGlobalDensity(value);
    m_ui->sliderGlobalDensity->setValue(value);
}
void SceneEditor::setGlobalDensity(int value)
{
    if (value < 0) return;
    m_sceneWidget->mainWidget->m_hairDensity = value;
    m_ui->inputGlobalDensity->setText(QString::number(m_sceneWidget->mainWidget->m_hairDensity, 'g', 3));
}
void SceneEditor::inputGlobalLengthText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_sceneWidget->mainWidget->m_maxHairLength;
    } else if (value == m_sceneWidget->mainWidget->m_maxHairLength) return;
    setGlobalLength(100*value);
    m_ui->sliderGlobalLength->setValue(100*value);
}
void SceneEditor::setGlobalLength(int value)
{
    if (value < 0) return;
    m_sceneWidget->mainWidget->m_maxHairLength = value/100.;
    m_ui->inputGlobalLength->setText(QString::number(m_sceneWidget->mainWidget->m_maxHairLength, 'g', 3));
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
    m_sceneWidget->mainWidget->m_sceneEditor = NULL;
}
void SceneEditor::cancelEditor()
{
    m_sceneWidget->mainWidget->unpause();
    this->close();
    m_sceneWidget->mainWidget->m_sceneEditor = NULL;
}
