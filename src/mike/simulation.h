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


private:
    void sinDerp(HairObject *_object);


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

};

#endif // SIMULATION_H
