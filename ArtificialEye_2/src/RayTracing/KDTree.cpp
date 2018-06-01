#include "KDTree.hpp"

#include "RTUtility.hpp"

#include <numeric>

ee::KDTree::KDTree(const Mesh* mesh) :
    m_mesh(mesh)
{
    //int numNodes = static_cast<int>(static_cast<float>(m_mesh->getNumFaces()) / KD_FACES_PER_NODE + 0.5f);
    //m_nodes.reserve(numNodes);
    initialConstruct(KD_FACES_PER_NODE);
}

void ee::KDTree::initialConstruct(int maxFacesPerNode)
{
    m_nodes.push_back(Node());
    Node currNode;

    for (int i = 0; i < m_mesh->getNumFaces(); i++)
    {
        currNode.bound = combine(currNode.bound, boundTriangle(m_mesh->getTransTriangle(i)));
    }

    // If for some reason it's a really small mesh:
    if (m_mesh->getNumFaces() <= maxFacesPerNode)
    {
        currNode.faces.resize(maxFacesPerNode);
        std::iota(currNode.faces.begin(), currNode.faces.end(), 0);
        m_nodes[0] = currNode;
        return;
    }

    // Then go through and split it up:
    const float factor = 1.f / m_mesh->getNumFaces();
    glm::vec3 globalMidpoint = glm::vec3(0.f);
    for (int i = 0; i < m_mesh->getNumFaces(); i++)
    {
        const Triangle currTriangle = m_mesh->getTriangle(i);
        globalMidpoint += transPoint3(m_mesh->getModelTrans(), currTriangle.midpoint()) * factor;
    }

    std::vector<int> left;
    std::vector<int> right;
    const int   longestAxis = currNode.bound.longestAxis();
    const float globalRef = globalMidpoint[longestAxis];
    for (int i = 0; i < m_mesh->getNumFaces(); i++)
    {
        const Triangle currTriangle = m_mesh->getTriangle(i);
        const glm::vec3 mid = transPoint3(m_mesh->getModelTrans(), currTriangle.midpoint());
        if (globalRef >= mid[longestAxis])
        {
            right.push_back(i);
        }
        else
        {
            left.push_back(i);
        }
    }

    if (!left.empty())
    {
        currNode.left = constructTree(left, 0, maxFacesPerNode);
    }
    if (!right.empty())
    {
        currNode.right = constructTree(right, 0, maxFacesPerNode);
    }

    m_nodes[0] = currNode;
}

int ee::KDTree::constructTree(const std::vector<int>& faces, int parent, int maxFacesPerNode)
{
    m_nodes.push_back(Node());
    const int nodePosition = m_nodes.size() - 1;
    Node currNode;

    currNode.parent = parent;

    // generate a complete bounding box:
    for (const int i : faces)
    {
        currNode.bound = combine(currNode.bound, boundTriangle(m_mesh->getTransTriangle(i)));
    }

    // this means that we are done (at a leaf)
    if (faces.size() <= maxFacesPerNode)
    {
        currNode.faces = std::move(faces);
        m_nodes[nodePosition] = currNode;
        return nodePosition;
    }

    // otherwise keep going to generate rest of tree:

    // generate complete midpoint:
    const float factor = 1.f / faces.size();
    glm::vec3 globalMidpoint = glm::vec3(0.f);
    for (const int i : faces)
    {
        const Triangle currTriangle = m_mesh->getTriangle(i);
        globalMidpoint += transPoint3(m_mesh->getModelTrans(), currTriangle.midpoint()) * factor;
    }

    std::vector<int> left;
    std::vector<int> right;
    const int longestAxis = currNode.bound.longestAxis();
    const float globalRef = globalMidpoint[longestAxis];
    for (const int i : faces)
    {
        const Triangle currTriangle = m_mesh->getTriangle(i);
        const glm::vec3 mid = transPoint3(m_mesh->getModelTrans(), currTriangle.midpoint());
        if (globalRef >= mid[longestAxis])
        {
            right.push_back(i);
        }
        else
        {
            left.push_back(i);
        }
    }

    if (!left.empty())
    {
        currNode.left = constructTree(left, nodePosition, maxFacesPerNode);
    }
    if (!right.empty())
    {
        currNode.right = constructTree(right, nodePosition, maxFacesPerNode);
    }

    m_nodes[nodePosition] = currNode;
    return nodePosition;
}

bool ee::KDTree::intersect(int node, Ray ray, int ignore, glm::vec3* point, int* triangle) const
{
    Node currNode = m_nodes[node];
    if (!currNode.bound.intersect(ray))
    {
        return false;
    }

    if (currNode.left == -1 && currNode.right == -1)
    {
        // if we are at a leaf we check all the triangles in that leaf:
        for (const int f : currNode.faces)
        {
            if (f == ignore)
            {
                continue;
            }

            const MeshFace& face = m_mesh->getFace(f);
            const glm::vec3 p0 = m_mesh->getTransVertex(face[0]);
            const glm::vec3 p1 = m_mesh->getTransVertex(face[1]);
            const glm::vec3 p2 = m_mesh->getTransVertex(face[2]);

            if (intersectTriangle(ray, p0, p1, p2, point))
            {
                *triangle = f;
                return true;
            }
        }

        return false;
    }

    // if we aren't at a leaf yet:
    const bool leftRes = currNode.left != -1 ? intersect(currNode.left, ray, ignore, point, triangle) : false;
    const bool rightRes = currNode.right != -1 ? intersect(currNode.right, ray, ignore, point, triangle) : false;
    return leftRes || rightRes;
}