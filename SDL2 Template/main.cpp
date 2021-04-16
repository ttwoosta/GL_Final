//
//  main.cpp
//  gfxLite
//
//  Created by Nevin Flanagan on 12/21/20.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>

using namespace std::string_literals;

std::string file_contents(std::string filename)
{
    using namespace std;
    ifstream in{ filename, ios::in | ios::binary };
    if (in) {
        ostringstream contents;
        contents << in.rdbuf();
        return contents.str();
    }
    throw (errno);
}

#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Display.h"
#include "GL/Shader.h"
#include "GL/Vertex.h"
#include "GL/Camera.h"
#include "GL/Mesh.h"
#include "GL/Texture.h"
#include "GL/Lighting.h"

#include "format/OBJ.h"
#include "format/TGA.h"

namespace gfx = gl;

class Project {
public:
    Project(int w, int h);
    int operator()(const std::vector<std::string>& args);
protected:
    using seconds = std::chrono::duration<float>;
    void update(seconds frame, seconds total);
    void render() const;
private:
    sdl::Library SDL;
    Display output;
    bool running;
    mutable gfx::Program renderer; // add another Program field
    mutable gfx::Camera cam;
    gfx::Lights<4> lights;
    std::vector<gfx::Object*> objects;
};

template <typename E, typename C = E>
using ArrayAttribute = gfx::Vertex::ArrayAttribute<E, C>;

Project::Project(int w, int h)
:   SDL {sdl::Library::Video}, 
    output {SDL, w, h}, 
    running {true},
    renderer { 
        gfx::Shader{ gfx::Shader::Vertex, file_contents("plain_vertex.glsl") }, 
        gfx::Shader{ gfx::Shader::Fragment, file_contents("plain_fragment.glsl") } 
    },
    cam{ glm::perspectiveLH(1.0f, 4.0f / 3.0f, 1.0f, 200.0f), glm::lookAtLH(gfx::Point3{0, 2, -4}, gfx::Point3{0}, gfx::Vector3{0, 1, 0}) }
{ 
    glVertexAttrib4f(renderer.attributes["color"], 1, 1, 1, 1);
    glVertexAttrib3f(renderer.attributes["normal"], 0, 0, 0);

    auto sun = std::make_shared<gfx::Mesh>();
    *sun <<= geometry::OBJ{ "sphere.obj" };


    // create another texture shared_ptr and initialize its contents from another TGA.

    {   // The sun
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        //o->transform = glm::translate(gfx::Matrix4{}, gfx::Vector3{ 2.0f, 0.0f, 0.0f });
        o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };
        
        auto e = new gl::Object{ sun, renderer };
        objects.push_back(e);
        e->transform = glm::translate(glm::scale(gfx::Matrix4{}, gfx::Vector3{ .2f }), gfx::Vector3{ 10.0f, 0.0f, 0.0f });
        e->tint = gfx::ColorAlpha{ .35f,.35f,.35f,1.0f };
        // set the object's surface from one of the texture pointers
        // Load texture into memory       

        //Venus
        auto v = new gl::Object{ sun,renderer };
        objects.push_back(v);
        v->transform = glm::translate(glm::scale(gfx::Matrix4{}, gfx::Vector3{ .3f }), gfx::Vector3{ 10.5f, 0.0f, 0.0f });
        v->tint = gfx::ColorAlpha{ .65f,.35f,.35f,1.0f };


        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "lichen.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    renderer << lights;
    renderer.Uniform<gfx::Color>("ambient") = gfx::Color{ .95f };
    gfx::Light::Source scene_lighting[] = {
        gfx::Light::Source::Directional(gfx::Vector3{-.4f, -.75f, .35f}, gfx::Color{.3f}),
        gfx::Light::Source::Point(gfx::Point3{0, 1, -1}, 7.5f),
    };
    {
        using namespace std;
        auto lighting = lights.Access();
        copy(begin(scene_lighting), end(scene_lighting), begin(*lighting));
    }
}

int Project::operator()(const std::vector<std::string>& args)
{
    using std::chrono::duration_cast;

    cam.Access()->projection = glm::orthoLH(-2.0f, 14.0f, -7.0f, 7.0f, 1.0f, 200.0f);

    sdl::Ticks previous = SDL.GetTicks();
    while (running) {
        sdl::Ticks now = SDL.GetTicks();
        update(duration_cast<seconds>(now - previous), duration_cast<seconds>(now));
        previous = now;
        render();
    }
    return 0;
}

void Project::update(seconds frame, seconds total)
{
    std::unordered_map<sdl::EventType, std::function<void(const sdl::Event&)>> responses;
    responses.emplace(sdl::EventType::Quit, [this](const sdl::Event&) { running = false; });
    SDL.ProcessEvents(responses);



    /* TODO: Disable translation and glow light
    
    objects.front()->transform = glm::translate(gfx::Matrix4{}, { 2 + sin(total.count()), 0.0f, 0.0f });

    // set the angle of the directional light
    float t = sinf(total.count());
    float x = sinf(t);
    float y = cosf(t);
    auto lighting = lights.Access();

    // change the directional light so that it swings 
    // from the upper left to the upper right and back. 
    (*lighting)->angle = gfx::Vector3{ x, y, 0.5f };
    (*lighting)->kind = 1;
    (*lighting)->color = gfx::Color{ 0.35f };

    // set the ambient lighting
    // update the ambient light over time to go back and forth 
    // between dim white light and brighter orange light 
    // (it will probably still be fairly dim, a little goes a long way).
    // derive the third argument from a trig function on total.count(), 
    // but accelerate it by multiplying total.count() by a coefficient before taking the sine or cosine. 
    auto mixed = glm::mix(gfx::Color{ 0.15f }, gfx::Color{ 0.35f, 0.0f, 0.0f }, sinf(total.count() * 2.0f));
    renderer.Uniform<gfx::Color>("ambient") = mixed;
    */
}

void Project::render() const
{
    output.Clear();
    
    for (auto& object : objects) { cam << *object; }

    output.Refresh();
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "Author: Tu Tong (0262620)\n";

    try {
        return Project{1280, 960}(vector<string>{argv, argv + argc});
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
