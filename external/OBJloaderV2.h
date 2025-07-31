#include <glm/glm.hpp>
#include <cstring>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

bool loadOBJ2(
	const char * path,
	std::vector<int> & vertexIndices,
	std::vector<glm::vec3> & temp_vertices,
	std::vector<glm::vec3> & out_normals,
	std::vector<glm::vec2> & out_uvs){

	std::vector<int> uvIndices, normalIndices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE * file;
	file = fopen(path, "r");
	if (!file){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}

   char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y; // Invert V coordinate for DDS-style textures
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            char line[128];
            fgets(line, sizeof(line), file);

            int vi[3], ui[3], ni[3];
            int matches = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d",
                &vi[0], &ui[0], &ni[0],
                &vi[1], &ui[1], &ni[1],
                &vi[2], &ui[2], &ni[2]);

            bool hasUV = true, hasNorm = true;

            if (matches != 9) {
                matches = sscanf(line, "%d//%d %d//%d %d//%d",
                    &vi[0], &ni[0], &vi[1], &ni[1], &vi[2], &ni[2]);
                if (matches == 6) {
                    hasUV = false;
                } else {
                    matches = sscanf(line, "%d/%d %d/%d %d/%d",
                        &vi[0], &ui[0], &vi[1], &ui[1], &vi[2], &ui[2]);
                    if (matches == 6) {
                        hasNorm = false;
                    } else {
                        matches = sscanf(line, "%d %d %d", &vi[0], &vi[1], &vi[2]);
                        if (matches != 3) {
                            std::cerr << "❌ Failed to parse face line at position " << ftell(file) << std::endl;
                            fclose(file);
                            return false;
                        }
                        hasUV = hasNorm = false;
                    }
                }
            }

            for (int i = 0; i < 3; ++i) {
                vertexIndices.push_back(abs(vi[i]) - 1);
                if (hasNorm) normalIndices.push_back(abs(ni[i]) - 1);
                if (hasUV) uvIndices.push_back(abs(ui[i]) - 1);
            }
        }
        else {
            // Ignore other lines
            char buffer[1000];
            fgets(buffer, sizeof(buffer), file);
        }
    }

    fclose(file);

    // Expand normals and uvs
    if (!normalIndices.empty()) out_normals.resize(temp_vertices.size());
    if (!uvIndices.empty()) out_uvs.resize(temp_vertices.size());

    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        int vi = vertexIndices[i];
        if (!normalIndices.empty()) out_normals[vi] = temp_normals[normalIndices[i]];
        if (!uvIndices.empty()) out_uvs[vi] = temp_uvs[uvIndices[i]];
    }

    return true;
}
