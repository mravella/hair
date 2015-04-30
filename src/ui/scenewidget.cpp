#include "glwidget.h"

#include "scenewidget.h"
#include "QWidget"
#include "hairobject.h"
#include "texture.h"
#include "hairinterface.h"

#include "QMouseEvent"

SceneWidget::SceneWidget(GLWidget *parent, ObjMesh *mesh) :
    QGLWidget(parent),
    m_timer(this),
    m_mesh(mesh)
{
    mainWidget = parent;
    
    setMouseTracking(true);
    
    m_grayscale = 1;
    m_radius = 50;
    m_opacity = 1;
    m_mask = NULL;
    m_blendBuffer = NULL;
    m_blendBufferValid = false;
    m_mouseDown = false;
    
    
    m_brushFalloffType = CONSTANT;
    updateBrushSettings();
    
    m_timer.start(1000.0f / 60.);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateCanvas()));
}
SceneWidget::~SceneWidget(){
    delete[] m_mask;
    delete[] m_blendBuffer;
}

void SceneWidget::updateCanvas()
{
    update();
}
void SceneWidget::initializeGL()
{    
    
    test = new Texture();
    test->createColorTexture(mainWidget->m_hairObject->m_hairGrowthMap, GL_LINEAR, GL_LINEAR);
    
}
void SceneWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void SceneWidget::paintGL(){
    
    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    test->bind(GL_TEXTURE_2D);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0,1); glVertex3f(-1, -1, -1);
    glTexCoord2f(1,1); glVertex3f(1, -1, -1);
    glTexCoord2f(1,0); glVertex3f(1, 1, -1);
    glTexCoord2f(0,0); glVertex3f(-1, 1, -1);    
    glEnd();

    test->unbind(GL_TEXTURE_2D);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 1, 1, 0.25);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe

    // Scale and translate UV map
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(-1, -1, 0);
    glScalef(2, 2, 0);

    // Draw triangle UVs
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < m_mesh->triangles.size(); i++)
    {
        Triangle t = m_mesh->triangles[i];
        glVertex3f(t.uv1.x, t.uv1.y, -1);
        glVertex3f(t.uv2.x, t.uv2.y, -1);
        glVertex3f(t.uv3.x, t.uv3.y, -1);
    }
    glEnd();
    glPopMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_BLEND);
    glColor3f(1,1,1);
}


void SceneWidget::updateBrushSettings(){
    if (m_brushFalloffType == CONSTANT){
        makeConstantMask();
    } else if (m_brushFalloffType == LINEAR){
        makeLinearMask();
    } else if (m_brushFalloffType == QUADRATIC){
        makeQuadraticMask();
    }
    
//    m_previewBuffer = QImage(2*m_radius+1, 2*m_radius+1,)
}


void SceneWidget::mousePressEvent(QMouseEvent *event)
{
    m_mouseDown = true;
    QPoint pos = QPoint(round(event->x()*test->width()/width()), round(event->y()*test->height()/height()));        
    paintTexture(glm::vec2(pos.x(), pos.y()), test->m_image.bits(), glm::vec2(test->m_image.width(), test->m_image.height()));
    test->updateImage();
}

void SceneWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseDown){
        QPoint pos = QPoint(round(event->x()*test->width()/width()), round(event->y()*test->height()/height()));        
        paintTexture(glm::vec2(pos.x(), pos.y()), test->m_image.bits(), glm::vec2(test->m_image.width(), test->m_image.height()));
        test->updateImage();
    }
}

void SceneWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDown = false;
    m_blendBufferValid = false;
}


void SceneWidget::paintTexture(glm::vec2 center, uchar *data, glm::vec2 imgSize)
{
    int canvasW = imgSize.x;
    int canvasH = imgSize.y;
    
    int maskSize = 2*m_radius+1;
    
    int colStart = center.x-m_radius;
    int colEnd = center.x+m_radius+1;
    int rowStart = center.y-m_radius;
    int rowEnd = center.y+m_radius+1;
    
    int maskColStart = 0;
    int maskColEnd = maskSize;
    int maskRowStart = 0;
    int maskRowEnd = maskSize;
    
    if (colStart < 0){
        maskColStart -= colStart;
        colStart = 0;
    }
    if (colEnd > canvasW){
        maskColEnd -= (colEnd-canvasW);
        colEnd = canvasW;
    }
    if (rowStart < 0){
        maskRowStart -= rowStart;
        rowStart = 0;
    }
    if (rowEnd > canvasH){
        maskRowEnd -= (rowEnd-canvasH);
        rowEnd = canvasH;
    }
    
    BGRA *pix = (BGRA*)data;
    
    if (!m_blendBufferValid){
        m_blendBufferValid = true;
        
        delete[] m_blendBuffer;
        m_blendBuffer = new float[canvasW*canvasH];
        int i; for (i = 0; i < canvasW*canvasH; i++) m_blendBuffer[i] = 0;
    }
    
    int row, col, maskRow, maskCol;
    for (row = rowStart, maskRow = maskRowStart; row < rowEnd; row++, maskRow++){
        for (col = colStart, maskCol = maskColStart; col < colEnd; col++, maskCol++){
            float maskValue = m_mask[maskRow*maskSize+maskCol];
            
            float blendValue = m_opacity*maskValue;
            
            float blendLeftToMax = (m_opacity-m_blendBuffer[row*canvasW+col])/(1.0-m_blendBuffer[row*canvasW+col]);
            if (blendValue > blendLeftToMax) blendValue = blendLeftToMax;
            if (blendValue < 0) blendValue = 0;
            m_blendBuffer[row*canvasW+col] += (1.0-m_blendBuffer[row*canvasW+col])*blendValue; // incorporate previous blends
            
            pix[row*canvasW+col].r = (pix[row*canvasW+col].r)*(1-blendValue) + (255*m_grayscale)*(blendValue);
            pix[row*canvasW+col].g = (pix[row*canvasW+col].g)*(1-blendValue) + (255*m_grayscale)*(blendValue);
            pix[row*canvasW+col].b = (pix[row*canvasW+col].b)*(1-blendValue) + (255*m_grayscale)*(blendValue);
        }
    }
}

void SceneWidget::apply(){
    test->m_image.save("output.png");
    
    mainWidget->resetTexture = test;
    
    mainWidget->unpause();
}

void SceneWidget::makeConstantMask()
{
    delete[] m_mask;
    int len = (2*m_radius+1);
    m_mask = new float[len*len];

    int row, col;
    for (row = 0; row < len; row++){
      for (col = 0; col < len; col++){
        float distance = sqrt((row-m_radius)*(row-m_radius)+(col-m_radius)*(col-m_radius));
        m_mask[row*len+col] = (distance <= m_radius ? 1 : 0);
      }
    }
}

void SceneWidget::makeLinearMask()
{
    delete[] m_mask;
    int len = (2*m_radius+1);
    m_mask = new float[len*len];
    
    int row, col;
    for (row = 0; row < len; row++){
        for (col = 0; col < len; col++){
            float distance = sqrt((row-m_radius)*(row-m_radius)+(col-m_radius)*(col-m_radius));
            m_mask[row*len+col] = 1-distance/m_radius;
            if (m_mask[row*len+col] < 0) m_mask[row*len+col] = 0;
        }
    }
}

void SceneWidget::makeQuadraticMask()
{
    delete[] m_mask;
    int len = (2*m_radius+1);
    m_mask = new float[len*len];

    int row, col;
    for (row = 0; row < len; row++){
      for (col = 0; col < len; col++){
        float distance = sqrt((row-m_radius)*(row-m_radius)+(col-m_radius)*(col-m_radius));
        m_mask[row*len+col] = (1-distance/m_radius);
        if (m_mask[row*len+col] < 0) m_mask[row*len+col] = 0; // because apparently using max(a, b) is a bitch
        else m_mask[row*len+col] *= (1-distance/m_radius);
      }
    }
}


void SceneWidget::clearTexture(int r, int g, int b){
    test->m_image.fill(QColor(r, g, b));
    test->updateImage();
}

