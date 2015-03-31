#ifndef HAIROBJECT_H
#define HAIROBJECT_H

#include "QList"
#include "hairCommon.h"
#include "shaderprogram.h"

class Hair;
class Simulation;

class HairObject
{
public:
    HairObject(int _numGuideHairs, Simulation *_simulation);

    void update(float _time);
    void paint(ShaderProgram &program);

public:
    QList<Hair*> m_guideHairs;

    Simulation *m_simulation;

};

#endif // HAIROBJECT_H
