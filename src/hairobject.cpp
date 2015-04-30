#include "hairobject.h"

#include "hair.h"
#include "errorchecker.h"
#include "simulation.h"
#include "texture.h"
#include "blurrer.h"

#include <glm/gtx/color_space.hpp>

HairObject::HairObject(int _numGuideHairs, Simulation *_simulation)
{
    
    m_numGuideHairs = _numGuideHairs;
    
    for (int i = 0; i < m_numGuideHairs; i++)
    {
        m_guideHairs.append(new Hair(20, 1, glm::vec3(i + 0.25, 1, 0), glm::vec3(1, 0, 0)));
    }
    
    setAttributes();

    m_simulation = _simulation;
}

HairObject::HairObject(HairObject *_oldObject, Simulation *_simulation)
{
    
    m_numGuideHairs = _oldObject->m_numGuideHairs;
    
    for (int i = 0; i < m_numGuideHairs; i++)
    {
        m_guideHairs.append(new Hair(20, 1, glm::vec3(i + 0.25, 1, 0), glm::vec3(1, 0, 0)));
    }
    
    setAttributes(_oldObject);

    m_simulation = _simulation;
}

HairObject::~HairObject()
{
    for (int i = 0; i < m_guideHairs.size(); ++i)
        delete m_guideHairs.at(i);
    safeDelete(m_blurredHairGrowthMapTexture);
}

HairObject::HairObject(
        ObjMesh *_mesh,
        float _hairsPerUnitArea,
        QImage &image,
        Simulation *_simulation,
        HairObject *_oldObject)
{    
    if (image.width() == 0)
    {
//        std::cout << _hairGrowthMap << " does not appear to be a valid image." << std::endl;
        exit(1);
    }

    m_hairGrowthMap = image;

    // Initialize blurred hair growth map texture.
    QImage blurredImage;
    Blurrer::blur(image, blurredImage);
    m_blurredHairGrowthMapTexture = new Texture();
    m_blurredHairGrowthMapTexture->createColorTexture(blurredImage, GL_LINEAR, GL_LINEAR);

    
    int _failures = 0;
    int _emptyPoints = 0;
    for (unsigned int i = 0; i < _mesh->triangles.size(); i++)
    {
        Triangle t = _mesh->triangles[i];

        // Number of guide hairs to generate on this triangle.
        int numHairs = (int) (_hairsPerUnitArea * t.area() + rand() / (float)RAND_MAX);
        for (int hair = 0; hair < numHairs; hair++)
        {
            // Generate random point on triangle.
            glm::vec3 pos; glm::vec2 uv; glm::vec3 normal;
            t.randPoint(pos, uv, normal);
            uv = glm::vec2(MIN(uv.x, 0.999), MIN(uv.y, 0.999)); // Make UV in range [0,1) instead of [0,1]

            QPoint p = QPoint(uv.x * image.width(), (1 - uv.y) * image.height());
            if (!image.valid(p)){
                _failures++;
                continue; // Don't put hair on neck......
            }

            // If hair growth map is black, skip this hair.
            QColor hairGrowth = QColor(image.pixel(p));
            if (hairGrowth.value() == 0){
                _emptyPoints++;
                continue;
            }

            m_guideHairs.append(new Hair(20, 0.45, pos, normal));
        }
    }
    
    setAttributes(_oldObject);

    m_simulation = _simulation;    
}

void HairObject::setAttributes(HairObject *_oldObject){
    if (_oldObject == NULL){
        setAttributes();
    } else {
        setAttributes(
                    _oldObject->m_color,
                    _oldObject->m_numGroupHairs,
                    _oldObject->m_hairGroupSpread,
                    _oldObject->m_hairRadius,
                    _oldObject->m_noiseAmplitude,
                    _oldObject->m_numSplineVertices);
    }
}

void HairObject::setAttributes(glm::vec3 _color, int _numGroupHairs, float _hairGroupSpread, float _hairRadius, float _noiseAmplitude, float _noiseFrequency, int _numSplineVertices){
    m_color = _color;
    m_numGroupHairs = _numGroupHairs;
    m_hairGroupSpread = _hairGroupSpread;
    m_hairRadius = _hairRadius;
    m_noiseAmplitude = _noiseAmplitude;
    m_noiseFrequency = _noiseFrequency;
    m_numSplineVertices = _numSplineVertices;
    m_shadowIntensity = 15;
    m_diffuseIntensity = 1;
    m_specularIntensity = .5;
}

void HairObject::update(float _time){

    if (m_simulation != NULL)
    {
        m_simulation->simulate(this);
    }

    for (int i = 0; i < m_guideHairs.size(); i++)
    {
        m_guideHairs.at(i)->update(_time);
    }
}

void HairObject::paint(ShaderProgram *program){
    
    program->uniforms.color = glm::rgbColor(glm::vec3(m_color.x*255, m_color.y, m_color.z));
    program->uniforms.numGroupHairs = m_numGroupHairs;
    program->uniforms.hairGroupSpread = m_hairGroupSpread;
    program->uniforms.hairRadius = m_hairRadius;
    program->uniforms.noiseAmplitude = m_noiseAmplitude;
    program->uniforms.noiseFrequency = m_noiseFrequency;
    program->uniforms.numSplineVertices = m_numSplineVertices;
    program->setPerObjectUniforms();

    for (int i = 0; i < m_guideHairs.size(); i++)
    {
        m_guideHairs.at(i)->paint(program);
    }
}
