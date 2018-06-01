#pragma once

#include <vector>
#include "../Types/Bound.hpp"
#include "../Mesh/Mesh.hpp"

namespace ee
{
    class KDTree
    {
    public:
        KDTree(const Mesh* mesh);

    private:
        struct Node
        {
            Bound3 bound;
            int parent; // might want to know this for something I am planning
            int left;
            int right;
            std::vector<int> faces;

            Node() : left(-1), right(-1) {}
        };

        int constructTree(const std::vector<int>& faces, int parent, int maxFacesPerNode);
        bool intersect(int node, Ray ray, glm::vec3* point, int* triangle) const;

        const Mesh* m_mesh;
        std::vector<Node> m_nodes; // first one is root
    };
}