#include "engine.hpp"

#include <GL/glew.h>
#include <vector>

#include "render/framebuffer.hpp"
#include "render/renderSurface.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"

Engine::Engine(uint width, uint height, uint depth, std::string frag_path, GLuint internal_format, uint stages)
    : _width(width), _height(height), _stages(stages)
{
    _surface = new RenderSurface();
    TextureOptions option{};
    option.target = GL_TEXTURE_2D_ARRAY;
    option.internal_format = internal_format;

    _tex0 = new Texture(_width, _height, depth, option);
    _tex1 = new Texture(_width, _height, depth, option);
    _fbo = new Framebuffer();
    program = new FragmentProgram("basic.vs", frag_path);
}

Engine::~Engine()
{
    delete _surface;
    delete _tex0;
    delete _tex1;
    delete _fbo;
    delete program;
}

void Engine::start()
{
    glViewport(0, 0, _width, _height);
    program->use();
    _fbo->use();
}
void Engine::step()
{
    for (uint stage = 0; stage < _stages; stage++)
    {
        _tex0_last_drawn = !_tex0_last_drawn;

        std::vector<GLenum> buffers;
        for (size_t i = 0; i < _tex0->depth; i++)
        {
            buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        glDrawBuffers(_tex0->depth, &buffers[0]);
        _fbo->set_texture(0, _tex0_last_drawn ? _tex1 : _tex0);
        program->set_texture(0, "tex", _tex0_last_drawn ? _tex0 : _tex1);

        program->set_uniform("v63", _counter);
        program->set_uniform("stage", (uint32_t)stage);

        _surface->draw();
        glFinish();

        _counter++;
    }
}

Texture *Engine::current_texture()
{
    if (!_tex0_last_drawn)
        return _tex0;
    else
        return _tex1;
}