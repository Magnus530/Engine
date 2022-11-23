#include "epch.h"
#include "VisualObject.h"

#include <glad/glad.h>

namespace Engine {

    VisualObject::VisualObject(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    {
        /* Create Vertex array */
        m_VA.reset(VertexArray::Create());
        m_VA->Bind();

        /* Vertex buffer */
        m_VB.reset(VertexBuffer::Create(vertices.data(), (uint32_t)vertices.size() * sizeof(Vertex)));
        m_VB->SetLayout
        ({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoord" }
            });
        m_VA->AddVertexBuffer(m_VB);

        /* Index buffer */
        m_IB.reset(IndexBuffer::Create(indices.data(), indices.size()));
        m_VA->SetIndexBuffer(m_IB);
    }
    VisualObject::~VisualObject()
    {
    }

}