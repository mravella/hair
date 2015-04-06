#ifndef HAIROBJECT_H
#define HAIROBJECT_H

#include "QList"
#include "hairCommon.h"
#include "shaderprogram.h"
#include "objmesh.h"

class Hair;
class Simulation;

class HairObject
{
public:
    HairObject(int _numGuideHairs, Simulation *_simulation);
    HairObject(HairObject *_oldObject, Simulation *_simulation);
    
    // Generates hair on the mesh according to the hair growth map.
    HairObject(ObjMesh *_mesh,
            float _hairsPerUnitArea,
            const char *_hairGrowthMap,
            Simulation *_simulation,
            HairObject *_oldObject = NULL);

    // Generates hair all over the given mesh.
    HairObject(ObjMesh *_mesh, float _hairsPerUnitArea, Simulation *_simulation)
        : HairObject(_mesh, _hairsPerUnitArea, ":/images/white.jpg", _simulation) { }

    void update(float _time);
    void paint(ShaderProgram &program);
    void setAttributes(HairObject *_oldObject);
    void setAttributes(
            glm::vec3 _color = glm::vec3(.6f, .4f, .3f),
            int _numGroupHairs = 15,
            float _hairGroupWidth = 0.2,
            float _hairRadius = 0.005f,
            float _noiseAmplitude = 0.03f,
            int _numSplineVertices = 20);
    
public:
    
    QList<Hair*> m_guideHairs;

    Simulation *m_simulation;
    
    int m_numGuideHairs;
    int m_numHairVertices;
    
    int m_numHairsPerPatch;
    glm::vec3 m_color;
    int m_numGroupHairs;
    float m_hairGroupWidth;
    float m_hairRadius;
    float m_noiseAmplitude;
    int m_numSplineVertices;

};

#endif // HAIROBJECT_H
