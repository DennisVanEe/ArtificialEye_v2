#pragma once

#include <vector>
#include <numeric>
#include "../Types/Bound.hpp"
#include "../Mesh/Mesh.hpp"

namespace ee
{
    const int KD_FACES_PER_NODE = 24;
    const int KD_AMT_RESERVE = 512;

    class KDTree
    {
    public:
        KDTree(const Mesh* mesh);

        void reconstruct()
        {
            m_nodes.clear();
            initialConstruct(KD_FACES_PER_NODE);
        }

        bool intersect(int node, Ray ray, int ignore, glm::vec3* point, int* triangle) const;

    private:
        struct Node
        {
            Bound3 bound;
            int parent; // might want to know this for something I am planning
            int left;
            int right;
            std::vector<int> faces;

            // It's a leaf if left and right are negative
            Node() : parent(-1), left(-1), right(-1) {}
        };

        void initialConstruct(int maxFacesPerNode);
        int constructTree(const std::vector<int>& faces, int parent, int maxFacesPerNode);

        const Mesh* m_mesh;
        std::vector<Node> m_nodes; // first one is root
    };
}