//
//  Texture.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#ifndef OPENGL_WRAPPER_LIGHTING
#define OPENGL_WRAPPER_LIGHTING

#include "OpenGL.h"
#include "Buffer.h"
#include "Shader.h"

namespace gl {
    class Light : protected UniformBuffer {
    public:
        struct Source {
            alignas(sizeof(Vector4))
                Point3 position {0};
            Int kind = 0;
            alignas(sizeof(Vector4))
                Vector3 angle {0};
            Float intensity = 0;
            Color color{ 1 };

            static Source Directional(Vector3 angle, Color color = Color{1})
            {
                return { Point3{0}, 1, angle, 0, color };
            }

            static Source Point(Point3 position, Float intensity = 0, Color color = Color{ 1 })
            {
                return { position, 2, Vector3{0}, intensity, color };
            }
        };

        friend Program& operator <<(Program& target, Light& lighting)
        {
            lighting.Activate(target["lighting"]);
            return target;
        }
        using UniformBuffer::Access;
    };

    template <int N = 1>
    class Lights : public Light {
    public:
        Lights() 
        { 
            Light::Source blank[N];
            Load(StaticDraw, blank, N);
        }
        auto Access() { return Light::Access<Source[N]>(); }
    private:
    };

}

#endif
