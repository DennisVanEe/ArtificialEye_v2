#include "RenderingUtilities.hpp"

void ee::loadIndexedRectangle(VertBuffer* vertList, IndexBuffer* indexList)
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
