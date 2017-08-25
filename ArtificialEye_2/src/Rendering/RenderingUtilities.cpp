#include "RenderingUtilities.hpp"

void ee::loadIndexedRectangle(VertBuffer* const vertList, IndexBuffer* const indexList)
{
    vertList->clear();
    indexList->clear();

    vertList->push_back(Vertex(Vector3(F(0.5), F(0.5), F(0.0))));
    vertList->push_back(Vertex(Vector3(F(0.5), F(-0.5), F(0.0))));
    vertList->push_back(Vertex(Vector3(F(-0.5), F(-0.5), F(0.0))));
    vertList->push_back(Vertex(Vector3(F(-0.5), F(0.5), F(0.0))));

    indexList->push_back(0), indexList->push_back(1), indexList->push_back(3);
    indexList->push_back(1), indexList->push_back(2), indexList->push_back(3);
}

 const ee::Float g_cubeVertices[] = 
 {
    // front
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    // back
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0,
};

 /* init_resources */
 const GLuint g_cubeElements[] = 
 {
     // -- winding is counter-clockwise (facing camera)
     0,1,2,                // pos z
     0,2,3,

     1,5,6,                // pos x
     1,6,2,

     2,6,7,                // pos y
     2,7,3,

     // -- winding is clockwise (facing away from camera)
     3,4,0,                // neg x
     3,7,4,

     4,5,1,                // neg y
     4,1,0,

     5,7,6,                // neg z
     5,4,7,
 };

void ee::loadIndexedCube(VertBuffer* const vertList, IndexBuffer* const indexList)
{
    vertList->clear();
    indexList->clear();

    for (std::size_t i = 0; i < F(sizeof(g_cubeVertices)) / sizeof(Float);)
    {
        vertList->push_back(Vertex(Vector3(g_cubeVertices[i++], g_cubeVertices[i++], g_cubeVertices[i++])));
    }

    indexList->insert(indexList->end(), std::begin(g_cubeElements), std::end(g_cubeElements));
}