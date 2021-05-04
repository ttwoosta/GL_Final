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
#include <glm/gtx/string_cast.hpp>

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
    cam{ glm::perspectiveLH(1.0f, 4.0f / 3.0f, 1.0f, 200.0f), glm::lookAtLH(gfx::Point3{6, 2, -16}, gfx::Point3{9.5f,0.0f,0.0f}, gfx::Vector3{0, 1, 0}) }
{ 
    glVertexAttrib4f(renderer.attributes["color"], 1, 1, 1, 1);
    glVertexAttrib3f(renderer.attributes["normal"], 0, 0, 0);

    auto sun = std::make_shared<gfx::Mesh>();
    *sun <<= geometry::OBJ{ "sphere.obj" };


    // create another texture shared_ptr and initialize its contents from another TGA.
    // Texture orginally obtain from https://sketchfab.com/Protowork/collections/3dp 
    // Using online convert tool from JPG to TGA https://convertio.co/


    {   // The sun
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        //o->transform = glm::translate(gfx::Matrix4{}, gfx::Vector3{ 2.0f, 0.0f, 0.0f });
        o->transform = glm::rotate(
            glm::translate(
                glm::scale(gfx::Matrix4{}, gfx::Vector3{ 1.0f }), 
                gfx::Vector3{ 0.0f, 0.0f, 3.0f }), 2.0f / 3.0f, 
            gfx::Vector3{ 0,0,1 });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "suncyl1.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    {   // The mercury
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .30f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "8k_mercury.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }
    
    // The Venus
    {   
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .45f });
        o->tint = gfx::ColorAlpha{ 1.0f, 0.6f, 0.5f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory    
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "8k_venus_surface.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    {   // The earth
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .48f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "earth_8k.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    {   // The moon
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .2f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "8k_moon.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }
    {   // The mars
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .35f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "8k_mars.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    {   // The jupiter
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .8f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "8k_jupiter.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    {   // The saturn
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .7f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "8k_saturn.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    {   // The uranus
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .55f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "uranusmap.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    {   // The neptune
        auto o = new gl::Object{ sun, renderer };
        objects.push_back(o);
        o->transform = glm::scale(gfx::Matrix4{}, gfx::Vector3{ .52f });
        //o->tint = gfx::ColorAlpha{ 1.0f, 1.0f, 0.0f, 1.0f };

        // set the object's surface from one of the texture pointers
        // Load texture into memory       
        gl::Texture* sand = new gfx::Texture;
        *sand <<= image::TGA{ "2k_neptune.tga" };

        o->surface = std::shared_ptr<gfx::Texture>{ sand };
    }

    renderer << lights;
    renderer.Uniform<gfx::Color>("ambient") = gfx::Color{ 1.2f };
    gfx::Light::Source scene_lighting[] = {
        gfx::Light::Source::Directional(gfx::Vector3{-.4f, -.75f, .35f}, gfx::Color{.3f}),
        gfx::Light::Source::Point(gfx::Point3{0, 1, -1}, 7.5f),
    };
    {
        using namespace std;
        auto lighting = lights.Access();
        copy(begin(scene_lighting), end(scene_lighting), begin(*lighting));
    }

    // shift the camera to the left to center the sun
    cam << gfx::Vector3{ 10.0f,0,0 };
}

static bool isHoldingShiftKey = false;
static float centerCamera = 9.5f;

int Project::operator()(const std::vector<std::string>& args)
{
    using namespace std;
    using std::chrono::duration_cast;
    SDL_Event event;
    sdl::Ticks previous = SDL.GetTicks();

    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode keySym = event.key.keysym.sym;

                if (keySym == SDLK_DOWN)
                    cam << gfx::Vector3{ 0,0.2f,0 };
                else if (keySym == SDLK_UP)
                    cam << gfx::Vector3{ 0,-0.2f,0 };

                if (keySym == SDLK_LEFT)
                    cam << gfx::Vector3{ 0.2f,0,0 };
                else if (keySym == SDLK_RIGHT)
                    cam << gfx::Vector3{ -0.2f,0,0 };

                else if (keySym == SDLK_LSHIFT)
                    isHoldingShiftKey = true;

                else if (keySym == SDLK_r) {
                    cam.Access()->facing = glm::lookAtLH(gfx::Point3{ 6, 2, -16 }, gfx::Point3{ 9.5f,0.0f,0.0f }, gfx::Vector3{ 0, 1, 0 });
                }

                else if (keySym == SDLK_SPACE) {
                    auto c = cam.Access();
                    cout << "Facing: " << glm::to_string(c->facing) << endl;
                }
                else if (keySym == SDLK_p) {
                    auto c = cam.Access();
                    cout << "Projection: " << glm::to_string(c->projection) << endl;
                }
            }
            else if (event.type == SDL_KEYUP) {
                SDL_Keycode keySym = event.key.keysym.sym;

                if (keySym == SDLK_LSHIFT)
                    isHoldingShiftKey = false;
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                SDL_Keycode keySym = event.key.keysym.sym;

                if (event.wheel.y > 0) // scroll up
                {
                    if (isHoldingShiftKey) {
                        centerCamera -= 0.1f;
                        cam.Access()->facing = glm::lookAtLH(gfx::Point3{ 6, 2, -16 }, gfx::Point3{ centerCamera,0.0f,0.0f }, gfx::Vector3{ 0, 1, 0 });
                    }
                    else
                        cam << gfx::Vector3{ 0,0,-0.2f };
                }
                else if (event.wheel.y < 0) // scroll down
                {
                    if (isHoldingShiftKey) {
                        centerCamera += 0.1f;
                        cam.Access()->facing = glm::lookAtLH(gfx::Point3{ 6, 2, -16 }, gfx::Point3{ centerCamera,0.0f,0.0f }, gfx::Vector3{ 0, 1, 0 });
                    }
                    else
                        cam << gfx::Vector3{ 0,0, 0.2f };
                }
            }
        }

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
    
    //sun
    objects[0]->transform = glm::rotate(objects[0]->transform, frame.count() * -0.5f, gfx::Vector3{ 0,1,0 }); 
    //mercury
    objects[1]->transform = glm::translate(objects[1]->transform, gfx::Vector3{ -5.0f, 0.0f,20.0f }); //orbit
    objects[1]->transform = glm::translate(glm::rotate(objects[1]->transform, frame.count(), gfx::Vector3{ 0,1,0 }), gfx::Vector3{0.05f,0,0.05f});
    objects[1]->transform = glm::translate(objects[1]->transform, gfx::Vector3{ 5.0f, 0.0f,-20.0f });
    //venus
    objects[2]->transform = glm::translate(objects[2]->transform, gfx::Vector3{ -5.0f, 0.0f, 24.0f });
    objects[2]->transform = glm::translate(glm::rotate(objects[2]->transform, frame.count() * 0.8f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0,0.05f });
    objects[2]->transform = glm::translate(objects[2]->transform, gfx::Vector3{ 5.0f, 0.0f, -24.0f });
    //earth
    objects[3]->transform = glm::translate(objects[3]->transform, gfx::Vector3{ -5.0f, 0.0f,30.5f });
    objects[3]->transform = glm::translate(glm::rotate(objects[3]->transform, frame.count() * 0.75f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0.0f,0.05f });
    objects[3]->transform = glm::translate(objects[3]->transform, gfx::Vector3{ 5.0f, 0.0f,-30.5f });
    //moon
    objects[4]->transform = glm::translate(objects[4]->transform, gfx::Vector3{ -5.0f, 4.0f,55.5f });
    objects[4]->transform = glm::translate(glm::rotate(objects[4]->transform, frame.count() * 0.75f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0.0f,0.05f });
    objects[4]->transform = glm::translate(objects[4]->transform, gfx::Vector3{ 5.0f, -4.0f,-55.5f });
    //mars
    objects[5]->transform = glm::translate(objects[5]->transform, gfx::Vector3{ -5.0f, 0.0f,51.5f });
    objects[5]->transform = glm::translate(glm::rotate(objects[5]->transform, frame.count() * 0.7f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0.0f,0.05f });
    objects[5]->transform = glm::translate(objects[5]->transform, gfx::Vector3{ 5.0f, 0.0f,-51.5f });
    //juipter
    objects[6]->transform = glm::translate(objects[6]->transform, gfx::Vector3{ -5.0f, 0.0f,48.5f });
    objects[6]->transform = glm::translate(glm::rotate(objects[6]->transform, frame.count() * 0.5f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0.0f,0.05f });
    objects[6]->transform = glm::translate(objects[6]->transform, gfx::Vector3{ 5.0f, 0.0f,-48.5f });
    //saturn
    objects[7]->transform = glm::translate(objects[7]->transform, gfx::Vector3{ -5.0f, 0.0f,73.5f });
    objects[7]->transform = glm::translate(glm::rotate(objects[7]->transform, frame.count() * 0.45f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0.0f,0.05f });
    objects[7]->transform = glm::translate(objects[7]->transform, gfx::Vector3{ 5.0f, 0.0f,-73.5f });
    //uranus
    objects[8]->transform = glm::translate(objects[8]->transform, gfx::Vector3{ -5.0f, 0.0f,120.5f });
    objects[8]->transform = glm::translate(glm::rotate(objects[8]->transform, frame.count() * 0.4f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0.0f,0.05f });
    objects[8]->transform = glm::translate(objects[8]->transform, gfx::Vector3{ 5.0f, 0.0f,-120.5f });
    //neptune
    objects[9]->transform = glm::translate(objects[9]->transform, gfx::Vector3{ -5.0f, 0.0f,145.5f });
    objects[9]->transform = glm::translate(glm::rotate(objects[9]->transform, frame.count() * 0.3f, gfx::Vector3{ 0,1,0 }), gfx::Vector3{ 0.05f,0.0f,0.05f });
    objects[9]->transform = glm::translate(objects[9]->transform, gfx::Vector3{ 5.0f, 0.0f,-145.5f });
    
}

void Project::render() const
{
    output.Clear();
    
    for (auto& object : objects) {cam << *object; }

    output.Refresh();
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "Author: Tu Tong (0262620)\n";
    cout << "Use arrow keys to move cam up/down/lef/right\n";
    cout << "Press Mouse scroll up/down to adjust the zoom\n";
    cout << "Press Mouse scroll up/down with left Shift to adjust the center of camera\n";
    cout << "Press R key to reset position\n";
    cout << "Press Space key to print the camera position\n";
    
    try {
        return Project{1280, 960}(vector<string>{argv, argv + argc});
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
