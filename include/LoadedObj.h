//
// Created by Arthur on 8/12/2023.
//

#ifndef FCG_TRAB_FINAL_LOADEDOBJ_H
#define FCG_TRAB_FINAL_LOADEDOBJ_H

// Headers de C++
#include <vector>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

class LoadedObj {
    private:

    public:
        tinyobj::attrib_t                 attrib;
        std::vector<tinyobj::shape_t>     shapes;
        std::vector<tinyobj::material_t>  materials;

        LoadedObj(const char* filename, const char* basepath = NULL, bool triangulate = true);
        LoadedObj();
};


#endif //FCG_TRAB_FINAL_LOADEDOBJ_H
