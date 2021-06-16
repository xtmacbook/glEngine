//
//  audo.hpp
//  openGLTest
//
//  Created by xt on 17/8/26.
//  Copyright © 2017年 xt. All rights reserved.
//

#ifndef __OpenGL__Audo_hpp
#define __OpenGL__Audo_hpp

#include <stdio.h>


class Audo
{
public:
    Audo();

   virtual bool                initAudo();

   virtual bool                update();
    
   virtual bool                shutDown();

   virtual bool                play();

   virtual bool                stop();

};

extern bool                     initAudo();
extern void                     updateAudo();
extern void                     playMusic();
extern void                     stop();

extern Audo *                   g_Audo;
#endif /* audo_hpp */
