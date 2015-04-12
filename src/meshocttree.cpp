#include "meshocttree.h"

#include "objmesh.h"
#include "QTime"
#include "QSet"

MeshOctTree::MeshOctTree(ObjMesh *mesh)
{

    QTime t;
    t.start();

    m_mesh = mesh;

    maxDepth = 4;
    minTrianglesForSplitting = 5;

    calculateBoundingBox();

    m_root = new OctTreeNode(boundingMin, boundingMax, maxDepth-1);

    for (int i = 0; i < m_mesh->triangles.size(); i++){
        insertTriangle(m_mesh->triangles.at(i));
    }

    cout << "done building octtree: "<< t.restart() << "ms" << endl;
}

MeshOctTree::~MeshOctTree()
{
    delete m_root;
}


void MeshOctTree::calculateBoundingBox(){
    if (m_mesh == NULL || m_mesh->triangles.size() <= 0){
        cerr << "mesh is borked" << endl;
        return;
    }

    boundingMin = m_mesh->triangles.at(0).v1;
    boundingMax = m_mesh->triangles.at(0).v1;

    for (int i = 0; i < m_mesh->triangles.size(); i++){
        Triangle tri = m_mesh->triangles.at(i);
        glm::vec3 pt = tri.v1;
        for (int j = 0; j < 3; j++){
            if (j == 1) pt = tri.v2;
            if (j == 2) pt = tri.v3;

            if (pt.x < boundingMin.x) boundingMin.x = pt.x;
            if (pt.x > boundingMax.x) boundingMax.x = pt.x;

            if (pt.y < boundingMin.y) boundingMin.y = pt.y;
            if (pt.y > boundingMax.y) boundingMax.y = pt.y;

            if (pt.z < boundingMin.z) boundingMin.z = pt.z;
            if (pt.z > boundingMax.z) boundingMax.z = pt.z;
        }
    }
}


void MeshOctTree::insertTriangle(Triangle tri)
{

    QList<OctTreeNode*> queue;
    queue.append(m_root);

    while (!queue.isEmpty()){
        OctTreeNode *current = queue.takeFirst();

        if (current->containsPoint(tri.v1) || current->containsPoint(tri.v2) || current->containsPoint(tri.v3)){

            current->triangles.append(tri);

            if (current->children == NULL){ // leaf node

                if (current->distFromMaxDepth > 0 && current->triangles.size() >= minTrianglesForSplitting){
                    current->split();
                }

            }


            if (current->children != NULL) {
                for (int i = 0; i < 8; i++){
                    queue.append(current->children[i]);
                }
            }

        }
    }
}

// from http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
bool MeshOctTree::rayIntersectsCube(glm::vec3 pt, glm::vec3 dir, glm::vec3 minCorner, glm::vec3 maxCorner){
    glm::vec3 recipDir;
    // r.dir is unit direction vector of ray
    recipDir.x = 1.0f / dir.x;
    recipDir.y = 1.0f / dir.y;
    recipDir.z = 1.0f / dir.z;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    float t1 = (minCorner.x - pt.x)*recipDir.x;
    float t2 = (maxCorner.x - pt.x)*recipDir.x;
    float t3 = (minCorner.y - pt.y)*recipDir.y;
    float t4 = (maxCorner.y - pt.y)*recipDir.y;
    float t5 = (minCorner.z - pt.z)*recipDir.z;
    float t6 = (maxCorner.z - pt.z)*recipDir.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
    if (tmax < 0) return false;

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) return false;

    return true;
}

uint qHash(Triangle tri){
    return ((int)(tri.v1.x*683 + tri.v1.y*811))^((int)(tri.v1.z*293))
           + ((int)(tri.v2.x*919 + tri.v2.z*773))^((int)(tri.v2.y*1061))
            + ((int)(tri.v3.z*1933 + tri.v3.y*2503))^((int)(tri.v3.x*1583));
}

void MeshOctTree::getRelevantTriangles(QList<Triangle> &list, glm::vec3 pt, glm::vec3 dir){

    QSet<Triangle> tris;

    QList<OctTreeNode*> queue;
    queue.append(m_root);

    while (!queue.isEmpty()){
        OctTreeNode *current = queue.takeFirst();

        if (rayIntersectsCube(pt, dir, current->min, current->max)){
            if (current->children == NULL){
                for (int i = 0; i < current->triangles.size(); i++){
                    tris.insert(current->triangles.at(i));
                }
            } else {
                for (int i = 0; i < 8; i++){
                    queue.append(current->children[i]);
                }
            }
        }
    }

    list = tris.values();
}

void MeshOctTree::print(){
    int currentDepth = m_root->distFromMaxDepth;

    QList<OctTreeNode*> queue;
    queue.append(m_root);

    while (!queue.isEmpty()){
        OctTreeNode *current = queue.takeFirst();

        if (currentDepth != current->distFromMaxDepth){
            currentDepth = current->distFromMaxDepth;
            cout << endl;
        }

        cout << "[" << current->triangles.size() << "] ";

        if (current->children != NULL) {
            for (int i = 0; i < 8; i++){
                queue.append(current->children[i]);
            }
        }

    }

    cout << endl;
}
