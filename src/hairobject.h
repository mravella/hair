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

    // Generates hair on the mesh according to the hair growth map.
    HairObject(ObjMesh *_mesh, const char *_hairGrowthMap, Simulation *_simulation);

    // Generates hair all over the given mesh.
    HairObject(ObjMesh *_mesh, Simulation *_simulation)
        : HairObject(_mesh, ":/images/white.jpg", _simulation) { }

    void update(float _time);
    void paint(ShaderProgram &program);

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
