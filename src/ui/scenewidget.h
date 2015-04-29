#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include "hairCommon.h"

#include <QTimer>
#include <QGLWidget>

class Texture;
class GLWidget;

enum BrushFalloff { CONSTANT, LINEAR, QUADRATIC };

class SceneWidget : public QGLWidget
{
    Q_OBJECT
    friend class SceneEditor;
    
public:
    explicit SceneWidget(GLWidget *parent);
    ~SceneWidget();
    
    GLWidget *mainWidget;
    
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    
    void updateBrushSettings();
    
    void paintTexture(glm::vec2 center, uchar *pix, glm::vec2 imgSize);    
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    
    void apply();
    
    void makeConstantMask();
    void makeLinearMask();
    void makeQuadraticMask();
    
    void clearTexture(int r, int g, int b);
    
signals:
    
public slots:
    void updateCanvas();
    
protected:
    QTimer m_timer;
    
    Texture *test;
    
    double m_grayscale;
    double m_opacity;
    int m_radius;
    
    float *m_mask;
    float *m_blendBuffer;
    QImage m_previewBuffer;
    bool m_blendBufferValid;
    
    BrushFalloff m_brushFalloffType;
    
    
    bool m_mouseDown;
    
    
    
    
};

#endif // SCENEWIDGET_H
