#ifndef HAIROBJECT_H
#define HAIROBJECT_H

#include "QList"
#include "hairCommon.h"
#include "shaderprogram.h"
#include "objmesh.h"

class Hair;
class Simulation;
class Texture;

class HairObject
{
public:
    HairObject(int _numGuideHairs, Simulation *_simulation);
    HairObject(HairObject *_oldObject, Simulation *_simulation);
    ~HairObject();
    
    // Generates hair on the mesh according to the hair growth map.
    HairObject(ObjMesh *_mesh,
            float _hairsPerUnitArea,
            QImage &hairGrowthMap,
            Simulation *_simulation,
            HairObject *_oldObject = NULL);
    
    
    void update(float _time);
    void paint(ShaderProgram *program);
    void setAttributes(HairObject *_oldObject);
    void setAttributes(
            glm::vec3 _color = glm::vec3(14./255, .356f, .231f),
            int _numGroupHairs = 64,
            float _hairGroupSpread = 0.08,
            float _hairRadius = 0.001f,
            float _noiseAmplitude = 0.15f,
            float _noiseFrequency = 0.2f,
            int _numSplineVertices = 20);
    
public:
    
    QList<Hair*> m_guideHairs;

    Simulation *m_simulation;

    QImage m_hairGrowthMap;
    Texture *m_blurredHairGrowthMapTexture;
    
    int m_numGuideHairs;
    int m_numHairVertices;
    
    int m_numHairsPerPatch;
    int m_numGroupHairs;
    float m_hairGroupSpread;
    float m_hairRadius;
    float m_noiseAmplitude;
    float m_noiseFrequency;
    int m_numSplineVertices;

    glm::vec3 m_color;
    
    float m_shadowIntensity;
    float m_diffuseIntensity;
    float m_specularIntensity;
    float m_transparency;
};

#endif // HAIROBJECT_H
