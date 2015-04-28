#include "hairinterface.h"

#include "ui_mainwindow.h"
#include "ui_sceneeditor.h"
#include "sceneeditor.h"
#include "glwidget.h"
#include "hairobject.h"
#include "hair.h"


HairInterface::HairInterface(Ui::MainWindow *ui)
{
    m_ui = ui;
    m_glWidget = NULL;
    m_hairObject = NULL;
    connectUserInputs();
    
    m_ui->testHolder->setAlignment(Qt::AlignTop);
    m_ui->scrollArea->setWidget(m_ui->controlsBox);
    m_ui->scrollArea->setFrameShape(QFrame::NoFrame);
    
//    showHideGroupSim();
//    showHideGroupTess();
//    showHideGroupRender();
    
    m_ui->groupTess->hide();
    m_ui->groupRender->hide();
    m_ui->groupSim->hide();
}

void HairInterface::connectUserInputs()
{
    connect(m_ui->showHideGroupSim, SIGNAL(pressed()), this, SLOT(showHideGroupSim()));
    connect(m_ui->showHideGroupTess, SIGNAL(pressed()), this, SLOT(showHideGroupTess()));
    connect(m_ui->showHideGroupRender, SIGNAL(pressed()), this, SLOT(showHideGroupRender()));
    
            
    // hairs per patch
    connect(m_ui->sliderHairsPerPatch, SIGNAL(valueChanged(int)), this, SLOT(setHairsPerPatch(int)));
    connect(m_ui->inputHairsPerPatch, SIGNAL(textChanged(QString)), this, SLOT(inputHairsPerPatchText(QString)));

    // spline vertices
    connect(m_ui->sliderSplineVertices, SIGNAL(valueChanged(int)), this, SLOT(setSplineVertices(int)));
    connect(m_ui->inputSplineVertices, SIGNAL(textChanged(QString)), this, SLOT(inputSplineVerticesText(QString)));
    
    // hair radius
    connect(m_ui->sliderHairRadius, SIGNAL(valueChanged(int)), this, SLOT(setHairRadius(int)));
    connect(m_ui->inputHairRadius, SIGNAL(textChanged(QString)), this, SLOT(inputHairRadiusText(QString)));
    
    // noise amp
    connect(m_ui->sliderNoiseAmp, SIGNAL(valueChanged(int)), this, SLOT(setNoiseAmp(int)));
    connect(m_ui->inputNoiseAmp, SIGNAL(textChanged(QString)), this, SLOT(inputNoiseAmpText(QString)));

    // rgb
    connect(m_ui->sliderHairColorR, SIGNAL(valueChanged(int)), this, SLOT(setHairColorR(int)));
    connect(m_ui->inputHairColorR, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorRText(QString)));
    connect(m_ui->sliderHairColorG, SIGNAL(valueChanged(int)), this, SLOT(setHairColorG(int)));
    connect(m_ui->inputHairColorG, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorGText(QString)));
    connect(m_ui->sliderHairColorB, SIGNAL(valueChanged(int)), this, SLOT(setHairColorB(int)));
    connect(m_ui->inputHairColorB, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorBText(QString)));

    // toggles
    connect(m_ui->frictionSimCheckBox, SIGNAL(toggled(bool)), this, SLOT(setFrictionSim(bool)));
    connect(m_ui->shadowCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShadows(bool)));
    connect(m_ui->supersampleCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSupersampling(bool)));

    // buttons
    connect(m_ui->pauseButton, SIGNAL(pressed()), this, SLOT(togglePaused()));
    connect(m_ui->buttonResetSim, SIGNAL(pressed()), this, SLOT(resetSimulation()));
    
    
    connect(m_ui->editScene, SIGNAL(pressed()), this, SLOT(startEditScene()));
}

void HairInterface::setGLWidget(GLWidget *glWidget)
{
    m_glWidget = glWidget;
}

void HairInterface::setHairObject(HairObject *hairObject)
{
    m_hairObject = hairObject;

    // Sync hairs per patch slider
    m_ui->sliderHairsPerPatch->setValue(m_hairObject->m_numGroupHairs);
    m_ui->inputHairsPerPatch->setText(QString::number(m_hairObject->m_numGroupHairs));

    // Sync spline vertices slider
    m_ui->sliderSplineVertices->setValue(m_hairObject->m_numSplineVertices);
    m_ui->inputSplineVertices->setText(QString::number(m_hairObject->m_numSplineVertices));

    // Sync hair radius slider
    m_ui->sliderHairRadius->setValue(m_hairObject->m_hairRadius*10000);
    m_ui->inputHairRadius->setText(QString::number(m_hairObject->m_hairRadius));
    
    // Sync noise amplitude slider
    m_ui->sliderNoiseAmp->setValue(m_hairObject->m_noiseAmplitude*100);
    m_ui->inputNoiseAmp->setText(QString::number(m_hairObject->m_noiseAmplitude));
    
    // Sync rgb sliders
    m_ui->sliderHairColorR->setValue(m_hairObject->m_color.x*2550);
    m_ui->sliderHairColorG->setValue(m_hairObject->m_color.y*2550);
    m_ui->sliderHairColorB->setValue(m_hairObject->m_color.z*2550);
    m_ui->inputHairColorR->setText(QString::number(m_hairObject->m_color.x, 'g', 2));
    m_ui->inputHairColorG->setText(QString::number(m_hairObject->m_color.y, 'g', 2));
    m_ui->inputHairColorB->setText(QString::number(m_hairObject->m_color.z, 'g', 2));

    // Sync toggles
    m_ui->frictionSimCheckBox->setChecked(m_glWidget->useFrictionSim);
    m_ui->shadowCheckBox->setChecked(m_glWidget->useShadows);
    m_ui->supersampleCheckBox->setChecked(m_glWidget->useSupersampling);

    updateStatsLabel();
}

void HairInterface::updateFPSLabel(int totalNumFrames)
{
    int updateFrequency = 10;
    if (totalNumFrames % updateFrequency == 1) {
        if (totalNumFrames > 1) {
            int fps = updateFrequency * 1000.0 / m_clock.elapsed();
            m_ui->fpsLabel->setText(QString::number(fps, 'f', 1) + " FPS");
        }
        m_clock.restart();
    }
}

void HairInterface::updateStatsLabel()
{
    // Update stats label.
    int numGuideHairs = m_hairObject->m_guideHairs.size();
    int numGroupHairs = m_hairObject->m_numGroupHairs;
    int numGuideVertices = m_hairObject->m_guideHairs[0]->m_vertices.size();
    int numSplineVertices = m_hairObject->m_numSplineVertices;
    m_ui->statsLabel->setText(
                QString::number(numGuideHairs) + " guide hairs\n" +
                QString::number(numGuideHairs * numGroupHairs) + " rendered hairs\n" +
                QString::number(numGuideHairs * numGuideVertices) + " simulated vertices\n" +
                QString::number(numGuideHairs * numGroupHairs * (numSplineVertices-1) * 2) + " rendered triangles");
}

void HairInterface::resetSimulation()
{
    m_glWidget->resetSimulation();
    updateStatsLabel();
}


void HairInterface::showHideGroupSim()
{
    if (m_ui->groupSim->isVisible()){
        m_ui->groupSim->hide();
        m_ui->showHideGroupSim->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupSim->show();
        m_ui->showHideGroupSim->setIcon(QIcon(":/images/chevron_down"));
    }
}

void HairInterface::showHideGroupTess()
{
    if (m_ui->groupTess->isVisible()){
        m_ui->groupTess->hide();
        m_ui->showHideGroupTess->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupTess->show();
        m_ui->showHideGroupTess->setIcon(QIcon(":/images/chevron_down"));
    }
}

void HairInterface::showHideGroupRender()
{
    if (m_ui->groupRender->isVisible()){
        m_ui->groupRender->hide();
        m_ui->showHideGroupRender->setIcon(QIcon(":/images/chevron_up"));
    } else {
        m_ui->groupRender->show();
        m_ui->showHideGroupRender->setIcon(QIcon(":/images/chevron_down"));
    }
}


void HairInterface::inputHairsPerPatchText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    int value = text.toInt(&ok);
    if (!ok){
        value = m_hairObject->m_numGroupHairs;
    } else if (value == m_hairObject->m_numGroupHairs) return;
    setHairsPerPatch(value);
}
void HairInterface::setHairsPerPatch(int numHairs)
{
    m_hairObject->m_numGroupHairs = numHairs;
    m_ui->inputHairsPerPatch->setText(QString::number(numHairs));
    m_ui->sliderHairsPerPatch->setValue(numHairs);
    updateStatsLabel();
}

void HairInterface::inputSplineVerticesText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    int value = text.toInt(&ok);
    if (!ok){
        value = m_hairObject->m_numSplineVertices;
    } else if (value == m_hairObject->m_numSplineVertices) return;
    setSplineVertices(value);
}
void HairInterface::setSplineVertices(int numVertices)
{
//    if (value <= 0) return;
    m_hairObject->m_numSplineVertices = numVertices;
    m_ui->inputSplineVertices->setText(QString::number(numVertices));
    m_ui->sliderSplineVertices->setValue(numVertices);
    updateStatsLabel();
}

void HairInterface::inputHairRadiusText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_hairRadius;
    } else if (value == m_hairObject->m_hairRadius) return;
    if (value == 0) return;
    setHairRadius(10000*value);
    m_ui->sliderHairRadius->setValue(10000*value);
}
void HairInterface::setHairRadius(int value)
{
    m_hairObject->m_hairRadius = value/10000.;
    m_ui->inputHairRadius->setText(QString::number(m_hairObject->m_hairRadius, 'g', 4));
}

void HairInterface::inputNoiseAmpText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_noiseAmplitude;
    } else if (value == m_hairObject->m_noiseAmplitude) return;
    setNoiseAmp(100*value);
    m_ui->sliderNoiseAmp->setValue(100*value);
}
void HairInterface::setNoiseAmp(int value)
{
    if (value < 0) return;
    m_hairObject->m_noiseAmplitude = value/100.;
    m_ui->inputNoiseAmp->setText(QString::number(m_hairObject->m_noiseAmplitude, 'g', 3));
}


void HairInterface::inputHairColorRText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_color.x;
    } else if (value == m_hairObject->m_color.x) return;
    setHairColorR(2550*value);
    m_ui->sliderHairColorR->setValue(2550*value);
}
void HairInterface::setHairColorR(int value)
{
    if (value < 0) return;
    m_hairObject->m_color.x = value/2550.;
    m_ui->inputHairColorR->setText(QString::number(m_hairObject->m_color.x, 'g', 3));
}

void HairInterface::inputHairColorGText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_color.y;
    } else if (value == m_hairObject->m_color.y) return;
    setHairColorG(2550*value);
    m_ui->sliderHairColorG->setValue(2550*value);
}
void HairInterface::setHairColorG(int value)
{
    if (value < 0) return;    
    m_hairObject->m_color.y = value/2550.;
    m_ui->inputHairColorG->setText(QString::number(m_hairObject->m_color.y, 'g', 3));
}

void HairInterface::inputHairColorBText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_color.z;
    } else if (value == m_hairObject->m_color.z) return;
    setHairColorB(2550*value);
    m_ui->sliderHairColorB->setValue(2550*value);
}
void HairInterface::setHairColorB(int value)
{
    if (value < 0) return;
    m_hairObject->m_color.z = value/2550.;
    m_ui->inputHairColorB->setText(QString::number(m_hairObject->m_color.z, 'g', 3));
}

void HairInterface::setShadows(bool checked)
{
    m_glWidget->useShadows = checked;
}

void HairInterface::setSupersampling(bool checked)
{
    m_glWidget->useSupersampling = checked;
}

void HairInterface::setFrictionSim(bool checked)
{
    m_glWidget->useFrictionSim = checked;
}

void HairInterface::togglePaused()
{
    m_glWidget->paused = !m_glWidget->paused;
    m_ui->pauseButton->setText(m_glWidget->paused ? "Continue" : "Pause");
}

void HairInterface::startEditScene(){
    if (!m_glWidget->paused){
        togglePaused();
    }
    
    SceneEditor *newSceneEditor = new SceneEditor(m_glWidget);
    
    newSceneEditor->show();
    newSceneEditor->raise();
    newSceneEditor->activateWindow();
}
