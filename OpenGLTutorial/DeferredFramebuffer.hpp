#pragma once 

#include <glad/glad.h>

#include <type_traits>

struct FramebufferBase {
    unsigned int _framebufferId;
    unsigned int _width, _height;
    FramebufferBase(unsigned int width, unsigned int height) : _width(width), _height(height) {
        glGenFramebuffers(1, &_framebufferId);
    }

    ~FramebufferBase() {
        glDeleteFramebuffers(1, &_framebufferId);
    }

    template<class T, class = std::enable_if_t<std::is_base_of_v<FramebufferBase, T>>>
    struct ScopedBinding {
        ScopedBinding(const T& framebuffer) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer._framebufferId);
            glViewport(0, 0, framebuffer._width, framebuffer._height);
        }

        ~ScopedBinding() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    };
};

// Hardcoded everything!
class DeferredFramebuffer : public FramebufferBase {

    unsigned int _positionTextureOutput;
    unsigned int _albedoTextureOutput;
    unsigned int _normalsTextureOutput;
    unsigned int _zBufferTextureOutput;
public:
    DeferredFramebuffer(unsigned int width, unsigned int height) : FramebufferBase(width, height) {
        ScopedBinding binding(*this);
        glGenTextures(1, &_positionTextureOutput);
        glBindTexture(GL_TEXTURE_2D, _positionTextureOutput);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionTextureOutput, 0);

        glGenTextures(1, &_albedoTextureOutput);
        glBindTexture(GL_TEXTURE_2D, _albedoTextureOutput);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _albedoTextureOutput, 0);

        glGenTextures(1, &_normalsTextureOutput);
        glBindTexture(GL_TEXTURE_2D, _normalsTextureOutput);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8_SNORM, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _normalsTextureOutput, 0);

        glGenTextures(1, &_zBufferTextureOutput);
        glBindTexture(GL_TEXTURE_2D, _zBufferTextureOutput);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _zBufferTextureOutput, 0);

        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~DeferredFramebuffer() {
        unsigned int texturesAsArray[] = { _positionTextureOutput, _albedoTextureOutput, _normalsTextureOutput, _zBufferTextureOutput };
        glDeleteTextures(sizeof(texturesAsArray), texturesAsArray);
    }

    unsigned int getPositionTexture() const { return _positionTextureOutput; }
    unsigned int getAlbedoTexture() const { return _albedoTextureOutput; }
    unsigned int getNormalsTexture() const { return _normalsTextureOutput; }
    unsigned int getZBufferTexture() const { return _zBufferTextureOutput; }
};