#include "hairobject.h"

#include "hair.h"
#include "errorchecker.h"
#include "simulation.h"
#include "texture.h"
#include "blurrer.h"

#include <glm/gtx/color_space.hpp>

HairObject::~HairObject()
{
    for (int i = 0; i < m_guideHairs.size(); ++i)
        delete m_guideHairs.at(i);
    safeDelete(m_blurredHairGrowthMapTexture);
}

HairObject::HairObject(
        ObjMesh *mesh,
        float hairsPerUnitArea,
        float maxHairLength,
        QImage &hairGrowthMap,
        QImage &hairGroomingMap,
        Simulation *simulation,
        HairObject *oldObject)
{    
    if (hairGrowthMap.width() == 0)
    {
        std::cout << "Hair growth map does not appear to be a valid image." << std::endl;
        exit(1);
    }

    m_hairGrowthMap = hairGrowthMap;
    m_hairGroomingMap = hairGroomingMap;

    // Initialize blurred hair growth map texture.
    QImage blurredImage;
    Blurrer::blur(hairGrowthMap, blurredImage);
    m_blurredHairGrowthMapTexture = new Texture();
    m_blurredHairGrowthMapTexture->createColorTexture(blurredImage, GL_LINEAR, GL_LINEAR);

    
    int _failures = 0;
    int _emptyPoints = 0;
    for (unsigned int i = 0; i < mesh->triangles.size(); i++)
    {
        Triangle t = mesh->triangles[i];

        // Number of guide hairs to generate on this triangle.
        int numHairs = (int) (hairsPerUnitArea * t.area() + rand() / (float)RAND_MAX);
        for (int hair = 0; hair < numHairs; hair++)
        {
            // Generate random point on triangle.
            glm::vec3 pos; glm::vec2 uv; glm::vec3 normal;
            t.randPoint(pos, uv, normal);
            uv = glm::vec2(MIN(uv.x, 0.999), MIN(uv.y, 0.999)); // Make UV in range [0,1) instead of [0,1]

            QPoint p = QPoint(uv.x * hairGrowthMap.width(), (1 - uv.y) * hairGrowthMap.height());
            if (!hairGrowthMap.valid(p)){
                _failures++;
                continue; // Don't put hair on neck......
            }

            // If hair growth map is black, skip this hair.
            QColor hairGrowth = QColor(hairGrowthMap.pixel(p));
            if (hairGrowth.valueF() < 0.05){
                _emptyPoints++;
                continue;
            }
            
            glm::vec3 u = glm::normalize(glm::cross(normal, glm::vec3(0, 1, 0)));
            glm::vec3 v = glm::normalize(glm::cross(u, normal));
            QColor groomingColor = QColor(m_hairGroomingMap.pixel(p));
            float a = 10.0 * (groomingColor.red() - 128.0) / 255.0;
            float b = 10.0 * (groomingColor.green() - 128.0) / 255.0;
            glm::vec3 x = glm::vec3(a, b, 1.0);
            glm::mat3 m = glm::mat3(u, v, normal);
            glm::vec3 dir = glm::normalize(m * x);
            
            m_guideHairs.append(new Hair(20, maxHairLength * hairGrowth.valueF(), pos, dir, normal));
        }
    }
    
    setAttributes(oldObject);

    m_simulation = simulation;    
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
                    _oldObject->m_noiseFrequency,
                    _oldObject->m_numSplineVertices,
                    _oldObject->m_shadowIntensity,
                    _oldObject->m_diffuseIntensity,
                    _oldObject->m_specularIntensity,
                    _oldObject->m_transparency,
                    _oldObject->m_useHairColorVariation,
                    _oldObject->m_hairColorVariation);
    }
}

void HairObject::setAttributes(glm::vec3 _color, int _numGroupHairs, float _hairGroupSpread, float _hairRadius, float _noiseAmplitude, float _noiseFrequency, int _numSplineVertices, float _shadowIntensity, float _diffuseIntensity, float _specularIntensity, float _transparency, float _useHairColorVariation, float _hairColorVariation){
    m_color = _color;
    m_numGroupHairs = _numGroupHairs;
    m_hairGroupSpread = _hairGroupSpread;
    m_hairRadius = _hairRadius;
    m_noiseAmplitude = _noiseAmplitude;
    m_noiseFrequency = _noiseFrequency;
    m_numSplineVertices = _numSplineVertices;
    m_shadowIntensity = _shadowIntensity;
    m_diffuseIntensity = _diffuseIntensity;
    m_specularIntensity = _specularIntensity;
    m_transparency = _transparency;
    m_useHairColorVariation = _useHairColorVariation;
    m_hairColorVariation = _hairColorVariation;
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
