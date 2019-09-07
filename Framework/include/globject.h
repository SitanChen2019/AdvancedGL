#ifndef ADVANCEDGL_GL_OBJECT_H
#define  ADVANCEDGL_GL_OBJECT_H


#include "glad/glad.h"
#include <memory>


class GLObjectMgr;

class SPGLBuffer
{
private:
    struct GLBufferObj
    {
    public:
        GLBufferObj(const GLBufferObj&) = delete;
        GLBufferObj(){
            glGenBuffers( 1, &m_bufferid );
        };

        ~GLBufferObj(){
            glDeleteBuffers( 1, &m_bufferid);
            m_bufferid = 0;
        };

        GLuint  m_bufferid = 0;
    };

    std::shared_ptr<GLBufferObj> m_ptr;

public:
    GLuint Id() {
        return m_ptr ? m_ptr->m_bufferid : 0;
    }

    friend  GLObjectMgr;
};



class GLObjectMgr
{
public:
    SPGLBuffer createBufferObject()
    {
        SPGLBuffer ret;
        ret.m_ptr = std::make_shared<SPGLBuffer::GLBufferObj>();
        return ret;
    }

};



#endif