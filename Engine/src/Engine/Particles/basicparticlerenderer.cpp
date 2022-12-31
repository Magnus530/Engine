#include "epch.h"

#include "basicparticlerenderer.h"
#include "basicparticles.h"

#include <string>

#include <assert.h>
//#include "gl_includes.h"
#include <gl/GLU.h>


namespace particles
{
    std::shared_ptr<IParticleRenderer> RendererFactory::create(const char *name)
    {
        std::string renderer{ name };

        if (renderer == "gl")
            return std::make_shared<GLParticleRenderer>();

        return nullptr;
    }

    void GLParticleRenderer::generate(ParticleSystem *sys, bool)
        {
        //mMatrix.setToIdentity();
        mMatrix = glm::mat4(1.0);
        //initializeOpenGLFunctions();

            assert(sys != nullptr);

            m_system = sys;

            const size_t count = m_system->numAllParticles();
//            const size_t count = sys->numAllParticles();

            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);

            glGenBuffers(1, &m_bufPos);
            glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
//            glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(float)* 4 * count,
                         nullptr,
                         GL_DYNAMIC_DRAW
                         );
            glEnableVertexAttribArray(0);

//            if (ogl_ext_ARB_vertex_attrib_binding)
//            {
//                glBindVertexBuffer(0, m_bufPos, 0, sizeof(float)* 4);
//                glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
//                glVertexAttribBinding(0, 0);
//            }
//            else
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));


            glGenBuffers(1, &m_bufCol);
            glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
//            glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(1);

//            if (ogl_ext_ARB_vertex_attrib_binding)
//            {
//                glBindVertexBuffer(1, m_bufCol, 0, sizeof(float)* 4);
//                glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
//                glVertexAttribBinding(1, 1);
//            }
//            else
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));

            glBindVertexArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
/**/
        }

        void GLParticleRenderer::destroy()
        {
            if (m_bufPos != 0)
            {
                glDeleteBuffers(1, &m_bufPos);
                m_bufPos = 0;
            }

            if (m_bufCol != 0)
            {
                glDeleteBuffers(1, &m_bufCol);
                m_bufCol = 0;
            }
        }

        void GLParticleRenderer::update()
        {
            assert(m_system != nullptr);
            assert(m_bufPos > 0 && m_bufCol > 0);

            const size_t count = m_system->numAliveParticles();
            if (count > 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
                float *ptr = (float *)(m_system->finalData()->m_pos.get());
                glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, ptr);

                glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
                ptr = (float*)(m_system->finalData()->m_col.get());
                glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, ptr);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        }

        void GLParticleRenderer::render()
        {
            glPointSize(10);
/**/
            glBindVertexArray(m_vao);

            const size_t count = m_system->numAliveParticles();
            if (count > 0)
                glDrawArrays(GL_POINTS, 0, count);

            glBindVertexArray(0);
/**/

            //  test
/**
            glBindVertexArray( m_vao );
            glDrawArrays(GL_POINTS, 0, 3);
            glBindVertexArray(0);
/**/
        }
}
