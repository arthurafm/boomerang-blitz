//
// Created by Arthur on 8/27/2023.
//

#ifndef FCG_TRAB_FINAL_MODEL_H
#define FCG_TRAB_FINAL_MODEL_H

#include "glm/vec4.hpp"
#include "LoadedObj.h"
#include "Scene.h"
#include <string>

class Model {
    private:
    glm::vec3 scale;
        std::string name;
        LoadedObj obj;
        int objectId;

        void ComputeNormals();
        void BuildTrianglesAndAddToVirtualScene(std::map<std::string, Scene> &virtualScene);

    protected:
        glm::vec3 position;

    public:
        Model(int id, glm::vec3 position, glm::vec3 scale, const char* name, const char* path, std::map<std::string, Scene> &virtualScene);

        glm::vec3 getPosition();
        glm::vec3 getScale();
        std::string getName();
        int getId() const;
        LoadedObj& getObj();
};


#endif //FCG_TRAB_FINAL_MODEL_H
