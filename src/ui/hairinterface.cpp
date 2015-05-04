#include "hairinterface.h"

#include "ui_mainwindow.h"
#include "ui_sceneeditor.h"
#include "sceneeditor.h"
#include "glwidget.h"
#include "hairobject.h"
#include "hair.h"
#include "simulation.h"
#include "objmesh.h"


HairInterface::HairInterface(Ui::MainWindow *ui)
{
    m_ui = ui;
    m_glWidget = NULL;
    m_hairObject = NULL;
    m_mesh = NULL;
    connectUserInputs();
    
    m_ui->testHolder->setAlignment(Qt::AlignTop);
    m_ui->scrollArea->setWidget(m_ui->controlsBox);
    m_ui->scrollArea->setFrameShape(QFrame::NoFrame);
    
    // to start with a group shown, use one of these:
    //    showHideGroupSim();
    //    showHideGroupTess();
    //    showHideGroupRender();
    
    // to start with a group hidden, use one of these:
    m_ui->groupTess->hide();
    m_ui->groupRender->hide();
    m_ui->groupSim->hide();
}

void HairInterface::connectUserInputs()
{
    connect(m_ui->showHideGroupSim, SIGNAL(pressed()), this, SLOT(showHideGroupSim()));
    connect(m_ui->showHideGroupTess, SIGNAL(pressed()), this, SLOT(showHideGroupTess()));
    connect(m_ui->showHideGroupRender, SIGNAL(pressed()), this, SLOT(showHideGroupRender()));
    
    connect(m_ui->sliderHairsPerPatch, SIGNAL(valueChanged(int)), this, SLOT(setHairsPerPatch(int)));
    connect(m_ui->inputHairsPerPatch, SIGNAL(textChanged(QString)), this, SLOT(inputHairsPerPatchText(QString)));
    connect(m_ui->sliderHairGroupSpread, SIGNAL(valueChanged(int)), this, SLOT(setHairGroupSpread(int)));
    connect(m_ui->inputHairGroupSpread, SIGNAL(textChanged(QString)), this, SLOT(inputHairGroupSpreadText(QString)));
    connect(m_ui->sliderSplineVertices, SIGNAL(valueChanged(int)), this, SLOT(setSplineVertices(int)));
    connect(m_ui->inputSplineVertices, SIGNAL(textChanged(QString)), this, SLOT(inputSplineVerticesText(QString)));
    connect(m_ui->sliderHairRadius, SIGNAL(valueChanged(int)), this, SLOT(setHairRadius(int)));
    connect(m_ui->inputHairRadius, SIGNAL(textChanged(QString)), this, SLOT(inputHairRadiusText(QString)));
    connect(m_ui->sliderNoiseAmp, SIGNAL(valueChanged(int)), this, SLOT(setNoiseAmp(int)));
    connect(m_ui->inputNoiseAmp, SIGNAL(textChanged(QString)), this, SLOT(inputNoiseAmpText(QString)));
    connect(m_ui->sliderNoiseFreq, SIGNAL(valueChanged(int)), this, SLOT(setNoiseFreq(int)));
    connect(m_ui->inputNoiseFreq, SIGNAL(textChanged(QString)), this, SLOT(inputNoiseFreqText(QString)));
    connect(m_ui->sliderHairColorR, SIGNAL(valueChanged(int)), this, SLOT(setHairColorR(int)));
    connect(m_ui->inputHairColorR, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorRText(QString)));
    connect(m_ui->sliderHairColorG, SIGNAL(valueChanged(int)), this, SLOT(setHairColorG(int)));
    connect(m_ui->inputHairColorG, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorGText(QString)));
    connect(m_ui->sliderHairColorB, SIGNAL(valueChanged(int)), this, SLOT(setHairColorB(int)));
    connect(m_ui->inputHairColorB, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorBText(QString)));
    connect(m_ui->sliderWindMagnitude, SIGNAL(valueChanged(int)), this, SLOT(setWindMagnitude(int)));
    connect(m_ui->inputWindMagnitude, SIGNAL(textChanged(QString)), this, SLOT(inputWindMagnitudeText(QString)));
    connect(m_ui->sliderShadowIntensity, SIGNAL(valueChanged(int)), this, SLOT(setShadowIntensity(int)));
    connect(m_ui->inputShadowIntensity, SIGNAL(textChanged(QString)), this, SLOT(inputShadowIntensityText(QString)));
    connect(m_ui->sliderDiffuseIntensity, SIGNAL(valueChanged(int)), this, SLOT(setDiffuseIntensity(int)));
    connect(m_ui->inputDiffuseIntensity, SIGNAL(textChanged(QString)), this, SLOT(inputDiffuseIntensityText(QString)));
    connect(m_ui->sliderSpecularIntensity, SIGNAL(valueChanged(int)), this, SLOT(setSpecularIntensity(int)));
    connect(m_ui->inputSpecularIntensity, SIGNAL(textChanged(QString)), this, SLOT(inputSpecularIntensityText(QString)));
    connect(m_ui->sliderStiffness, SIGNAL(valueChanged(int)), this, SLOT(setStiffness(int)));
    connect(m_ui->inputStiffness, SIGNAL(textChanged(QString)), this, SLOT(inputStiffnessText(QString)));
    connect(m_ui->sliderTransparency, SIGNAL(valueChanged(int)), this, SLOT(setTransparency(int)));
    connect(m_ui->inputTransparency, SIGNAL(textChanged(QString)), this, SLOT(inputTransparencyText(QString)));    
    connect(m_ui->sliderHairColorVariation, SIGNAL(valueChanged(int)), this, SLOT(setHairColorVariation(int)));
    connect(m_ui->inputHairColorVariation, SIGNAL(textChanged(QString)), this, SLOT(inputHairColorVariationText(QString)));    
    connect(m_ui->inputWindDirectionX, SIGNAL(textChanged(QString)), this, SLOT(inputWindDirectionXText(QString)));
    connect(m_ui->inputWindDirectionY, SIGNAL(textChanged(QString)), this, SLOT(inputWindDirectionYText(QString)));
    connect(m_ui->inputWindDirectionZ, SIGNAL(textChanged(QString)), this, SLOT(inputWindDirectionZText(QString)));
    connect(m_ui->actionReset_All_Values, SIGNAL(triggered()), this, SLOT(resetAllValues()));
    
    // toggles
    connect(m_ui->frictionSimCheckBox, SIGNAL(toggled(bool)), this, SLOT(setFrictionSim(bool)));
    connect(m_ui->shadowCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShadows(bool)));
    connect(m_ui->supersampleCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSupersampling(bool)));
    connect(m_ui->transparencyCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleTransparency(bool)));
    connect(m_ui->hairColorVariationCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleHairColorVariation(bool)));
    
    // buttons
    connect(m_ui->pauseButton, SIGNAL(pressed()), this, SLOT(togglePaused()));
    connect(m_ui->buttonResetSim, SIGNAL(pressed()), this, SLOT(resetSimulation()));
    connect(m_ui->sceneEditorButton, SIGNAL(pressed()), this, SLOT(startEditScene()));
}

void HairInterface::setGLWidget(GLWidget *glWidget)
{
    m_glWidget = glWidget;
}

void HairInterface::setMesh(ObjMesh *mesh)
{
    m_mesh = mesh;
}

void HairInterface::setHairObject(HairObject *hairObject)
{
    m_hairObject = hairObject;
    
    m_ui->sliderHairsPerPatch->setValue(m_hairObject->m_numGroupHairs);
    m_ui->inputHairsPerPatch->setText(QString::number(m_hairObject->m_numGroupHairs));
    m_ui->sliderHairGroupSpread->setValue(m_hairObject->m_hairGroupSpread*1000);
    m_ui->inputHairGroupSpread->setText(QString::number(m_hairObject->m_hairGroupSpread));    
    m_ui->sliderSplineVertices->setValue(m_hairObject->m_numSplineVertices);
    m_ui->inputSplineVertices->setText(QString::number(m_hairObject->m_numSplineVertices));
    m_ui->sliderHairRadius->setValue(m_hairObject->m_hairRadius*10000);
    m_ui->inputHairRadius->setText(QString::number(m_hairObject->m_hairRadius));
    m_ui->sliderNoiseAmp->setValue(m_hairObject->m_noiseAmplitude*100);
    m_ui->inputNoiseAmp->setText(QString::number(m_hairObject->m_noiseAmplitude));
    m_ui->sliderNoiseFreq->setValue(m_hairObject->m_noiseFrequency*100);
    m_ui->inputNoiseFreq->setText(QString::number(m_hairObject->m_noiseFrequency));
    m_ui->sliderHairColorR->setValue(m_hairObject->m_color.x*2550);
    m_ui->sliderHairColorG->setValue(m_hairObject->m_color.y*2550);
    m_ui->sliderHairColorB->setValue(m_hairObject->m_color.z*2550);
    m_ui->inputHairColorR->setText(QString::number(m_hairObject->m_color.x*255, 'g', 2));
    m_ui->inputHairColorG->setText(QString::number(m_hairObject->m_color.y, 'g', 2));
    m_ui->inputHairColorB->setText(QString::number(m_hairObject->m_color.z, 'g', 2));
    m_ui->sliderWindMagnitude->setValue(m_glWidget->m_testSimulation->m_windMagnitude*100);
    m_ui->inputWindMagnitude->setText(QString::number(m_glWidget->m_testSimulation->m_windMagnitude, 'g', 3));
    m_ui->sliderShadowIntensity->setValue(m_glWidget->m_hairObject->m_shadowIntensity*10);
    m_ui->inputShadowIntensity->setText(QString::number(m_glWidget->m_hairObject->m_shadowIntensity, 'g', 3));
    m_ui->sliderDiffuseIntensity->setValue(m_glWidget->m_hairObject->m_diffuseIntensity*100);
    m_ui->inputDiffuseIntensity->setText(QString::number(m_glWidget->m_hairObject->m_diffuseIntensity, 'g', 3));
    m_ui->sliderSpecularIntensity->setValue(m_glWidget->m_hairObject->m_specularIntensity*100);
    m_ui->inputSpecularIntensity->setText(QString::number(m_glWidget->m_hairObject->m_specularIntensity, 'g', 3));
    m_ui->sliderStiffness->setValue(m_glWidget->m_testSimulation->m_stiffness*1000);
    m_ui->inputStiffness->setText(QString::number(m_glWidget->m_testSimulation->m_stiffness, 'g', 4));
    m_ui->sliderTransparency->setValue(m_hairObject->m_transparency*1000);
    m_ui->inputTransparency->setText(QString::number(m_hairObject->m_transparency, 'g', 4));
    m_ui->sliderHairColorVariation->setValue(m_hairObject->m_hairColorVariation*1000);
    m_ui->inputHairColorVariation->setText(QString::number(m_hairObject->m_hairColorVariation, 'g', 4));
    m_ui->inputWindDirectionX->setText(QString::number(m_glWidget->m_testSimulation->m_windDir.x, 'g', 4));
    m_ui->inputWindDirectionY->setText(QString::number(m_glWidget->m_testSimulation->m_windDir.y, 'g', 4));
    m_ui->inputWindDirectionZ->setText(QString::number(m_glWidget->m_testSimulation->m_windDir.z, 'g', 4));
    
    // Sync toggles
    m_ui->frictionSimCheckBox->setChecked(m_glWidget->useFrictionSim);
    m_ui->shadowCheckBox->setChecked(m_glWidget->useShadows);
    m_ui->supersampleCheckBox->setChecked(m_glWidget->useSupersampling);
    m_ui->transparencyCheckBox->setChecked(m_glWidget->useTransparency);
    m_ui->hairColorVariationCheckBox->setChecked(m_hairObject->m_useHairColorVariation);
    
    updateStatsLabel();
}

void HairInterface::updateFPSLabel(int totalNumFrames)
{
    int updateFrequency = 5;
    if (totalNumFrames % updateFrequency == 1) {
        if (totalNumFrames > 1) {
            double fps = updateFrequency * 1000.0 / m_clock.elapsed();
            m_ui->fpsLabel->setText(QString::number(fps, 'f', 1) + " FPS");
        }
        m_clock.restart();
    }
}

void HairInterface::updateFPSLabelPaused(float fps)
{
    m_ui->fpsLabel->setText(QString::number(fps, 'f', 1) + " FPS");
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
    if (m_glWidget->m_sceneEditor == NULL){
        m_glWidget->resetSimulation();
        m_glWidget->forceUpdate();
        updateStatsLabel();
    }
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
    m_glWidget->forceUpdate();
}

void HairInterface::inputHairGroupSpreadText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_hairGroupSpread;
    } else if (value == m_hairObject->m_hairGroupSpread) return;
    if (value == 0) return;
    setHairGroupSpread(1000*value);
    m_ui->sliderHairGroupSpread->setValue(1000*value);
}
void HairInterface::setHairGroupSpread(int value)
{
    m_hairObject->m_hairGroupSpread = value/1000.;
    m_ui->inputHairGroupSpread->setText(QString::number(m_hairObject->m_hairGroupSpread, 'g', 4));
    m_glWidget->forceUpdate();
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
    m_glWidget->forceUpdate();
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
    m_glWidget->forceUpdate();
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
    m_glWidget->forceUpdate();
}

void HairInterface::inputNoiseFreqText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_noiseFrequency;
    } else if (value == m_hairObject->m_noiseFrequency) return;
    setNoiseFreq(100*value);
    m_ui->sliderNoiseFreq->setValue(100*value);
}
void HairInterface::setNoiseFreq(int value)
{
    if (value < 0) return;
    m_hairObject->m_noiseFrequency = value/100.;
    m_ui->inputNoiseFreq->setText(QString::number(m_hairObject->m_noiseFrequency, 'g', 3));
    m_glWidget->forceUpdate();
}

void HairInterface::inputHairColorRText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_color.x;
    } else if (value == m_hairObject->m_color.x) return;
    setHairColorR(10*value);
    m_ui->sliderHairColorR->setValue(10*value);
}
void HairInterface::setHairColorR(int value)
{
    if (value < 0) return;
    m_hairObject->m_color.x = value/2550.;
    m_ui->inputHairColorR->setText(QString::number(value/10, 'g', 3));
    m_glWidget->forceUpdate();
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
    m_glWidget->forceUpdate();
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
    m_glWidget->forceUpdate();
}

void HairInterface::inputWindMagnitudeText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windMagnitude;
    } else if (value == m_glWidget->m_testSimulation->m_windMagnitude) return;
    setWindMagnitude(100*value);
    m_ui->sliderWindMagnitude->setValue(100*value);
}
void HairInterface::setWindMagnitude(int value)
{
    if (value < 0) return;
    m_glWidget->m_testSimulation->m_windMagnitude = value/100.;
    m_ui->inputWindMagnitude->setText(QString::number(m_glWidget->m_testSimulation->m_windMagnitude, 'g', 3));
}

void HairInterface::inputWindDirectionXText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windDir.x;
    } else if (value == m_glWidget->m_testSimulation->m_windDir.x) return;
    m_glWidget->m_testSimulation->m_windDir.x = value;
}
void HairInterface::inputWindDirectionYText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windDir.y;
    } else if (value == m_glWidget->m_testSimulation->m_windDir.y) return;
    m_glWidget->m_testSimulation->m_windDir.y = value;
}
void HairInterface::inputWindDirectionZText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_windDir.z;
    } else if (value == m_glWidget->m_testSimulation->m_windDir.z) return;
    m_glWidget->m_testSimulation->m_windDir.z = value;
}

void HairInterface::inputShadowIntensityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_hairObject->m_shadowIntensity;
    } else if (value == m_glWidget->m_hairObject->m_shadowIntensity) return;
    setShadowIntensity(10*value);
    m_ui->sliderShadowIntensity->setValue(10*value);
}
void HairInterface::setShadowIntensity(int value)
{
    if (value < 0) return;    
    m_glWidget->m_hairObject->m_shadowIntensity = value/10.;
    m_ui->inputShadowIntensity->setText(QString::number(m_glWidget->m_hairObject->m_shadowIntensity, 'g', 3));
    m_glWidget->forceUpdate();
}

void HairInterface::inputDiffuseIntensityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_hairObject->m_diffuseIntensity;
    } else if (value == m_glWidget->m_hairObject->m_diffuseIntensity) return;
    setDiffuseIntensity(100*value);
    m_ui->sliderDiffuseIntensity->setValue(100*value);
}
void HairInterface::setDiffuseIntensity(int value)
{
    if (value < 0) return;    
    m_glWidget->m_hairObject->m_diffuseIntensity = value/100.;
    m_ui->inputDiffuseIntensity->setText(QString::number(m_glWidget->m_hairObject->m_diffuseIntensity, 'g', 3));
    m_glWidget->forceUpdate();
}

void HairInterface::inputSpecularIntensityText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_hairObject->m_specularIntensity;
    } else if (value == m_glWidget->m_hairObject->m_specularIntensity) return;
    setSpecularIntensity(100*value);
    m_ui->sliderSpecularIntensity->setValue(100*value);
}
void HairInterface::setSpecularIntensity(int value)
{
    if (value < 0) return;    
    m_glWidget->m_hairObject->m_specularIntensity = value/100.;
    m_ui->inputSpecularIntensity->setText(QString::number(m_glWidget->m_hairObject->m_specularIntensity, 'g', 3));
    m_glWidget->forceUpdate();
}

void HairInterface::inputStiffnessText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_glWidget->m_testSimulation->m_stiffness;
    } else if (value == m_glWidget->m_testSimulation->m_stiffness) return;
    setStiffness(1000*value);
    m_ui->sliderStiffness->setValue(1000*value);
}
void HairInterface::setStiffness(int value)
{
    if (value < 0) return;    
    m_glWidget->m_testSimulation->m_stiffness = value/1000.;
    m_ui->inputStiffness->setText(QString::number(m_glWidget->m_testSimulation->m_stiffness, 'g', 3));
}

void HairInterface::inputTransparencyText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_transparency;
    } else if (value == m_hairObject->m_transparency) return;
    setTransparency(1000*value);
    m_ui->sliderTransparency->setValue(1000*value);
}
void HairInterface::setTransparency(int value)
{
    if (value < 0) return;    
    m_hairObject->m_transparency = value/1000.;
    m_ui->inputTransparency->setText(QString::number(m_hairObject->m_transparency, 'g', 3));
    m_glWidget->forceUpdate();
}
void HairInterface::inputHairColorVariationText(QString text)
{
    if (text.length() == 0) return;
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok){
        value = m_hairObject->m_hairColorVariation;
    } else if (value == m_hairObject->m_hairColorVariation) return;
    setHairColorVariation(1000*value);
    m_ui->sliderHairColorVariation->setValue(1000*value);
}
void HairInterface::setHairColorVariation(int value)
{
    if (value < 0) return;    
    m_hairObject->m_hairColorVariation= value/1000.;
    m_ui->inputHairColorVariation->setText(QString::number(m_hairObject->m_hairColorVariation, 'g', 3));
    m_glWidget->forceUpdate();
}



void HairInterface::setShadows(bool checked)
{
    m_glWidget->useShadows = checked;
    m_glWidget->forceUpdate();
}
void HairInterface::setSupersampling(bool checked)
{
    m_glWidget->useSupersampling = checked;
    m_glWidget->forceUpdate();
}
void HairInterface::setFrictionSim(bool checked)
{
    m_glWidget->useFrictionSim = checked;
}
void HairInterface::toggleTransparency(bool checked)
{
    m_glWidget->useTransparency = checked;
    m_glWidget->forceUpdate();
}
void HairInterface::toggleHairColorVariation(bool checked)
{
    m_hairObject->m_useHairColorVariation = checked;
    m_glWidget->forceUpdate();
}


void HairInterface::togglePaused()
{
    if (m_glWidget->isPaused())
    {
        m_glWidget->unpause();
        m_ui->pauseButton->setText("Pause");
    }
    
    else
    {
        m_glWidget->pause();
        m_ui->pauseButton->setText("Continue");
    }
}

void HairInterface::startEditScene(){
    if (m_glWidget->m_sceneEditor == NULL){
        if (!m_glWidget->isPaused()){
            togglePaused();
        }
        
        m_glWidget->m_sceneEditor = new SceneEditor(m_glWidget, m_mesh);
        
        m_glWidget->m_sceneEditor->show();
        m_glWidget->m_sceneEditor->raise();
        m_glWidget->m_sceneEditor->activateWindow();
    }
}

void HairInterface::resetAllValues(){
    m_glWidget->resetSimulation(true);
    updateStatsLabel();
}
