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
     // front
     0, 1, 2,
     2, 3, 0,
     // top
     1, 5, 6,
     6, 2, 1,
     // back
     7, 6, 5,
     5, 4, 7,
     // bottom
     4, 0, 3,
     3, 7, 4,
     // left
     4, 5, 1,
     1, 0, 4,
     // right
     3, 2, 6,
     6, 7, 3,
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