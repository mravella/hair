#ifndef SIMULATION_H
#define SIMULATION_H

#include "hairCommon.h"
#include "integrator.h"
#include "objmesh.h"
#include <QMap>
#include <tuple>

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
    void moveObjects(HairObject *_object);

    void calculateExternalForces(HairObject *_object);
    void calculateFluidGrid(HairObject *_object);
    void calculateFrictionAndRepulsion(HairObject *_object);

    void integrate(HairObject *_object);
    void integrate2(HairObject *_object);
    void integrate3(HairObject *_object);
    void integrate4(HairObject *_object);

    void particleSimulation(HairObject *obj);
    void addToTable(QMap<std::tuple<double, double, double>, double> &grid, std::tuple<double, double, double> key, double value);
    void addToTable(QMap<std::tuple<double, double, double>, glm::vec3> &grid, std::tuple<double, double, double> key, glm::vec3 value);


public:
    QList<glm::vec3> m_externalForces;
    QMap<std::tuple<double, double, double>, double> m_densityGrid;
    QMap<std::tuple<double, double, double>, glm::vec3> m_velocityGrid;



private:
    float m_time;
    ObjMesh *m_mesh;

};

#endif // SIMULATION_H
