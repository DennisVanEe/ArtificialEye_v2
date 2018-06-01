#include "KDTree.hpp"

#include "RTUtility.hpp"

ee::KDTree::KDTree(const Mesh* mesh) :
    m_mesh(mesh)
{

}

int ee::KDTree::constructTree(const std::vector<int>& faces, int parent, int maxFacesPerNode)
{
    m_nodes.push_back(Node());
    const int nodePosition = m_nodes.size() - 1;
    Node& currNode = m_nodes.back();

    currNode.parent = parent;

    // generate a complete bounding box:
    for (const int i : faces)
    {
        currNode.bound = combine(currNode.bound, boundTriangle(m_mesh->getTriangle(faces[0])));
    }

    // generate complete midpoint:
    const float factor = 1.f / faces.size();
    glm::vec3 globalMidpoint = glm::vec3(0.f);
    for (const int i : faces)
    {
        const Triangle currTriangle = m_mesh->getTriangle(i);
        globalMidpoint += currTriangle.midpoint() * factor;
    }

    std::vector<int> left;
    std::vector<int> right;
    const int longestAxis = currNode.bound.longestAxis();
    const float globalRef = globalMidpoint[longestAxis];
    for (const int i : faces)
    {
        const Triangle currTriangle = m_mesh->getTriangle(i);
        if (globalRef >= currTriangle.midpoint()[longestAxis])
        {
            right.push_back(i);
        }
        else
        {
            left.push_back(i);
        }
    }

    if (left.empty() && !right.empty())
    {
        left = right;
    }
    else if (right.empty() && !left.empty())
    {
        right = left;
    }

    if (left.size() >= maxFacesPerNode)
    {
        currNode.left = constructTree(left, nodePosition, maxFacesPerNode);
    }
    if (right.size() >= maxFacesPerNode)
    {
        currNode.right = constructTree(right, nodePosition, maxFacesPerNode);
    }

    currNode.faces = std::move(faces);

    return nodePosition;
}

bool ee::KDTree::intersect(int node, Ray ray, glm::vec3* point, int* triangle) const
{
    Node currNode = m_nodes[node];
    if (!currNode.bound.intersect(ray))
    {
        return false;
    }

    if ((currNode.left >= 0 && !m_nodes[currNode.left].faces.empty()) || 
        (currNode.right >= 0 && !m_nodes[currNode.right].faces.empty()))
    {
        const bool leftRes = intersect(currNode.left, ray, point, triangle);
        const bool rightRes = intersect(currNode.right, ray, point, triangle);
        return leftRes || rightRes;
    }

    for (const int f : currNode.faces)
    {
        const MeshFace& face = m_mesh->getFace(f);
        const glm::vec3 p0 = transPoint3(m_mesh->getModelTrans(), m_mesh->getVertex(face[0]));
        const glm::vec3 p1 = transPoint3(m_mesh->getModelTrans(), m_mesh->getVertex(face[1]));
        const glm::vec3 p2 = transPoint3(m_mesh->getModelTrans(), m_mesh->getVertex(face[2]));

        if (intersectTriangle(ray, p0, p1, p2, point))
        {
            *triangle = f;
            return true;
        }
    }

    return false;
}