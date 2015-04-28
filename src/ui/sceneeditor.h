#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include <QMainWindow>

class GLWidget;
class SceneWidget;

namespace Ui {
class SceneEditor;
}

class SceneEditor : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SceneEditor(GLWidget *parent = 0);
    ~SceneEditor();
    
public slots:
    void showHideGroupMaps();
    void showHideGroupBrush();
    
    void inputRadiusText(QString text);
    void setRadius(int value);
    
    void inputBrushGrayscaleText(QString text);
    void setBrushGrayscale(int value);
    
    void inputOpacityText(QString text);
    void setOpacity(int value);
    
    void applyEditor();
    void cancelEditor();
    
    
private:
    Ui::SceneEditor *m_ui;
    
    SceneWidget *m_sceneWidget;
};

#endif // SCENEEDITOR_H
