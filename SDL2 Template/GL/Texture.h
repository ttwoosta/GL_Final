//
//  Texture.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include "OpenGL.h"

namespace gl {
    
    class Texture: public Name<Texture> {
    public:
        using Unit = Int;

        Texture() = default;
        
        template<typename T>
        Texture(const T& source) : Texture{} { operator <<=(source); }
        
        template <class T>
        Texture& operator <<=(const T& source);
        
        Unit Activate(Unit index = 0) const;
        
        static Unit Deactivate(Unit index = 0);

		static void init();
    };
}
