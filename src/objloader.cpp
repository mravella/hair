#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <QFile>
#include <QTextStream>

#include <glm/glm.hpp>

#include "objloader.hpp"

bool OBJLoader::loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
    std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

    // Read obj file.
    QString filepathStr = QString(path);
    QFile file(filepathStr);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);

        while (!stream.atEnd()) {
            QString line = stream.readLine();
            QStringList parts = line.split(" ", QString::SkipEmptyParts);
            if (line.startsWith("v ")) {
                glm::vec3 vertex = glm::vec3(
                            parts.at(1).toFloat(),
                            parts.at(2).toFloat(),
                            parts.at(3).toFloat());
                temp_vertices.push_back(vertex);
            }

            else if (line.startsWith("vt ")) {
                glm::vec2 uv = glm::vec2(parts.at(1).toFloat(), parts.at(2).toFloat());
                temp_uvs.push_back(uv);
            }

            else if (line.startsWith("vn ")) {
                glm::vec3 normal = glm::vec3(
                            parts.at(1).toFloat(),
                            parts.at(2).toFloat(),
                            parts.at(3).toFloat());
                temp_normals.push_back(normal);
            }

            else if (line.startsWith("f ")) {
                for (int i = 1; i <= 3; i++) {
                    QStringList v = parts.at(i).split("/");
                    vertexIndices.push_back(v.at(0).toInt());
                    uvIndices.push_back(v.at(1).toInt());
                    normalIndices.push_back(v.at(2).toInt());
                }

                // If face is a quad, make another triangle
                if (parts.size() > 4) {
                    std::vector<int> indices = {3, 4, 1};
                    for (unsigned int i = 0; i < indices.size(); i++) {
                        QStringList v = parts.at(indices[i]).split("/");
                        vertexIndices.push_back(v.at(0).toInt());
                        uvIndices.push_back(v.at(1).toInt());
                        normalIndices.push_back(v.at(2).toInt());
                    }
                }
            }
        }

        // For each vertex of each triangle
        for (unsigned int i = 0; i < vertexIndices.size(); i++) {

            // Get the indices of its attributes
            unsigned int vertexIndex = vertexIndices[i];
            unsigned int uvIndex = uvIndices[i];
            unsigned int normalIndex = normalIndices[i];

            // Get the attributes thanks to the index
            glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
            glm::vec2 uv = temp_uvs[ uvIndex-1 ];
            glm::vec3 normal = temp_normals[ normalIndex-1 ];

            // Put the attributes in buffers
            out_vertices.push_back(vertex);
            out_uvs     .push_back(uv);
            out_normals .push_back(normal);
        }
    }

    return true;
}
