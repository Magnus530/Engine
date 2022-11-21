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
    }
    VisualObject::~VisualObject()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }
    void VisualObject::Init(std::shared_ptr<VertexArray>& vertexarray)
    {
        /* Create Vertex array */
        vertexarray = VertexArray::Create();

        /* Vertex buffer */
        m_VB = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
        m_VB->SetLayout
        ({
            { ShaderDataType::Float3, "PositionIn" },
            { ShaderDataType::Float4, "colorIn" },
            { ShaderDataType::Float2, "uvIn" }
            });
        vertexarray->AddVertexBuffer(m_VB);

        /* Index buffer */
        m_IB = IndexBuffer::Create(m_Indices.data(), m_Indices.size());
        vertexarray->SetIndexBuffer(m_IB);
    }
    void VisualObject::Init()
    {
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));// array buffer offset
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &m_EAB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EAB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(GLuint), m_Indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void VisualObject::Draw()
    {
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    void VisualObject::Draw(glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
    {
        glUseProgram(m_Shader->GetProgram());
        m_Shader->SetUniformMatrix("mMatrix", m_Matrix);
        m_Shader->SetUniformMatrix("pMatrix", projectionMatrix);
        m_Shader->SetUniformMatrix("vMatrix", viewMatrix);

        Draw();
    }
}