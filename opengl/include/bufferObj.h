//
//  bufferObj.h
//  openGLTest
//
//  Created by xt on 18/5/16.
//  Copyright © 2018年 xt. All rights reserved.
//


#ifndef __OpenGL__BufferObj__
#define __OpenGL__BufferObj__


class Shader;

class BufferObj
{

public:
    enum
    {
        UNIFORMBLOCK
    };

    explicit BufferObj(Shader * );

    ~BufferObj();

    //uniform block
    void setUniformName(const char** names, const char* uniformName, int uniformNum);

    /*
     Buffer Blocks
    */
private:
    Shader *				shader_;

    unsigned int*			indices_;
    int*					size_;
    int*					offset_;
    int*					type_;

    int						ubbIndex_;
    int						uboDataSize_;
    
};

#endif


