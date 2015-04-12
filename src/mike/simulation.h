#ifndef SIMULATION_H
#define SIMULATION_H

#include "hairCommon.h"
#include "integrator.h"
#include "objmesh.h"

class HairObject;

class Simulation
{
public:
    Simulation(ObjMesh *mesh);

    virtual ~Simulation();

    void update(float _time);
    void simulate(HairObject *_object);

    glm::mat4 m_xform;


private:
    void moveObjects(void);

    void calculateExternalForces(HairObject *_object);
    void calculateConstraintForces(HairObject *_object);

    void integrate(HairObject *_object);
    void integrate2(HairObject *_object);
    void integrate3(HairObject *_object);
    void integrate4(HairObject *_object);

    void particleSimulation(HairObject *obj);


public:
    QList<glm::vec3> m_externalForces;



private:
    float m_time;
    ObjMesh *m_mesh;
    glm::vec4 m_prev;

};

#endif // SIMULATION_H
