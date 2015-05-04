#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include <QMainWindow>

class GLWidget;
class SceneWidget;
class ObjMesh;

namespace Ui {
class SceneEditor;
}

class SceneEditor : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SceneEditor(GLWidget *parent, ObjMesh *mesh);
    ~SceneEditor();
    void closeEvent(QCloseEvent *event);
    
public slots:
    void showHideGroupMaps();
    void showHideGroupBrush();
    void showHideGroupGlobals();
    
    void inputRadiusText(QString text);
    void setRadius(int value);
    
    void inputBrushGrayscaleText(QString text);
    void setBrushGrayscale(int value);
    
    void inputOpacityText(QString text);
    void setOpacity(int value);
    
    void setFalloffConstant();
    void setFalloffLinear();
    void setFalloffQuadratic();
    
    void setDensityTexture();
    void setDirectionTexture();
    
    void setBrushDirLeft();
    void setBrushDirRight();
    void setBrushDirUp();
    void setBrushDirDown();
    
    void inputGlobalDensityText(QString text);
    void setGlobalDensity(int value);
    void inputGlobalLengthText(QString text);
    void setGlobalLength(int value);
    
    void clearTexture();
    
    void applyEditor();
    void applyAndCloseEditor();
    void cancelEditor();
    
    
private:
    Ui::SceneEditor *m_ui;
    
    SceneWidget *m_sceneWidget;
};

#endif // SCENEEDITOR_H
