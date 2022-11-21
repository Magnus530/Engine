#include "epch.h"
#include "VisualObject.h"

#include <glad/glad.h>

namespace Engine {

    VisualObject::VisualObject()
    {
    }
    VisualObject::VisualObject(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
        : m_Vertices{ vertices }, m_Indices{ indices }
    {
        //Init(m_VA);

        m_Matrix = glm::mat4(1.f);
        m_Position = glm::mat4(1.f);
        m_Rotation = glm::mat4(1.f);
        m_Scale = glm::mat4(1.f);
    }
    VisualObject::~VisualObject()
    {
    }
    void VisualObject::Init(std::shared_ptr<VertexArray>& vertexarray)
    {
        /* Create Vertex array */
        vertexarray.reset(VertexArray::Create());
        vertexarray->Bind();
    
        /* Vertex buffer */
        m_VB.reset(VertexBuffer::Create(m_Vertices.data(), (uint32_t)m_Vertices.size() * sizeof(Vertex)));
        m_VB->SetLayout
        ({
            { ShaderDataType::Float3, "PositionIn" },
            { ShaderDataType::Float3, "colorIn" },
            { ShaderDataType::Float2, "uvIn" }
            });
        vertexarray->AddVertexBuffer(m_VB);
    
        /* Index buffer */
        m_IB.reset(IndexBuffer::Create(m_Indices.data(), m_Indices.size()));
        vertexarray->SetIndexBuffer(m_IB);
    }
}