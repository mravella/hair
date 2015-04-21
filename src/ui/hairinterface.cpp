#include "hairinterface.h"

#include "ui_mainwindow.h"
#include "glwidget.h"
#include "hairobject.h"
#include "hair.h"

HairInterface::HairInterface(Ui::MainWindow *ui)
{
    m_ui = ui;
    m_glWidget = NULL;
    m_hairObject = NULL;
    connectUserInputs();
}

void HairInterface::connectUserInputs()
{
    // hairs per patch
    connect(m_ui->sliderHairsPerPatch, SIGNAL(valueChanged(int)), this, SLOT(setHairsPerPatch(int)));

    // spline vertices
    connect(m_ui->sliderSplineVertices, SIGNAL(valueChanged(int)), this, SLOT(setSplineVertices(int)));
    
    // hair radius
    connect(m_ui->sliderHairRadius, SIGNAL(valueChanged(int)), this, SLOT(setHairRadius(int)));

    // rgb
    connect(m_ui->sliderHairColorR, SIGNAL(valueChanged(int)), this, SLOT(setHairColorR(int)));
    connect(m_ui->sliderHairColorG, SIGNAL(valueChanged(int)), this, SLOT(setHairColorG(int)));
    connect(m_ui->sliderHairColorB, SIGNAL(valueChanged(int)), this, SLOT(setHairColorB(int)));

    // toggles
    connect(m_ui->frictionSimCheckBox, SIGNAL(toggled(bool)), this, SLOT(setFrictionSim(bool)));
    connect(m_ui->shadowCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShadows(bool)));
    connect(m_ui->supersampleCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSupersampling(bool)));

    // buttons
    connect(m_ui->pauseButton, SIGNAL(pressed()), this, SLOT(togglePaused()));
    connect(m_ui->buttonResetSim, SIGNAL(pressed()), this, SLOT(resetSimulation()));
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
                QString::number(numGuideHairs * numGroupHairs * numSplineVertices * 4) + " rendered vertices");
}

void HairInterface::resetSimulation()
{
    m_glWidget->resetSimulation();
    updateStatsLabel();
}

void HairInterface::setHairsPerPatch(int numHairs)
{
    m_hairObject->m_numGroupHairs = numHairs;
    m_ui->inputHairsPerPatch->setText(QString::number(numHairs));
    updateStatsLabel();
}

void HairInterface::setSplineVertices(int numVertices)
{
    m_hairObject->m_numSplineVertices = numVertices;
    m_ui->inputSplineVertices->setText(QString::number(numVertices));
    updateStatsLabel();
}

void HairInterface::setHairRadius(int value)
{
    m_hairObject->m_hairRadius = value/10000.;
    m_ui->inputHairRadius->setText(QString::number(m_hairObject->m_hairRadius, 'g', 3));
}


void HairInterface::setHairColorR(int value)
{
    m_hairObject->m_color.x = value/2550.;
    m_ui->inputHairColorR->setText(QString::number(m_hairObject->m_color.x, 'g', 2));
}

void HairInterface::setHairColorG(int value)
{
    m_hairObject->m_color.y = value/2550.;
    m_ui->inputHairColorG->setText(QString::number(m_hairObject->m_color.y, 'g', 2));
}

void HairInterface::setHairColorB(int value)
{
    m_hairObject->m_color.z = value/2550.;
    m_ui->inputHairColorB->setText(QString::number(m_hairObject->m_color.z, 'g', 2));
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
