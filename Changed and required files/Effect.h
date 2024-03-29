//
//  Effect.h
//  Ders 7
//
//  Created by Gurel Erceis on 4/14/13.
//  Copyright (c) 2013 Yogurt3D. All rights reserved.
//

#ifndef Ders_7_Effect_h
#define Ders_7_Effect_h
#include "Util.h"
#include "SOIL.h"
#include "SceneNode.h"

class Effect{
public:
	static GLint LoadGLTexture(const char *filename){
			GLuint _texture;
			// filename = "/Users/gurelerceis/Documents/XCodeWorkspace/Ders2/SOIL/img_test.bmp"
    
			_texture = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture 
			(
			 filename,
			 SOIL_LOAD_AUTO,
			 SOIL_CREATE_NEW_ID,
			 SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT 
			 );
    
			// check for an error during the load process 
			if(_texture == 0)
			{
				printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
			}
	

			return _texture;
		}
    Effect(/*const char *fragmentShaderFile,*/ int* screen_width, int* screen_height):
        screen_width(screen_width),
        screen_height(screen_height)
    {
        initShader();
        initVertices();
        initFrameBuffer();
    }
    virtual ~Effect(){
        /* free_resources */
        glDeleteRenderbuffers(1, &rbo_depth);
        glDeleteTextures(1, &fbo_texture);
        glDeleteFramebuffers(1, &fbo);
        
        glDeleteBuffers(1, &vbo_fbo_vertices);
        
        glDeleteProgram(programID);
    } 
    void enableBuffer(){
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }
    void disableBuffer(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    virtual void draw(){
        glUseProgram(programID);
		glEnable(GL_TEXTURE_1D);
		//texture[0] = Effect::LoadGLTexture("images.jpg");
		//texture[1] = Effect::LoadGLTexture("ramp.bmp");
		GLint inSampler = glGetUniformLocation(programID, "inSampler");
        GLint ColorCorrMap = glGetUniformLocation(programID, "ColorCorrMap");
        glUseProgram(programID);
		glActiveTexture(GL_TEXTURE1);
		Effect::LoadGLTexture("images.jpg");
        glUniform1i(inSampler, 1);
		glActiveTexture(GL_TEXTURE2);
		Effect::LoadGLTexture("ramp.bmp");
		glUniform1i(ColorCorrMap, 2);
		glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_1D, fbo_texture);
        glUniform1i(UniformTexture, /*GL_TEXTURE*/0);
        glEnableVertexAttribArray(UniformVertex);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
        glVertexAttribPointer(
                              UniformVertex,  // attribute
                              2,                  // number of elements per vertex, here (x,y)
                              GL_FLOAT,           // the type of each element
                              GL_FALSE,           // take our values as-is
                              0,                  // no extra data between each position
                              0                   // offset of first element
                              );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(UniformVertex);
    }
    void resize(){
        // Rescale FBO and RBO as well
        glBindTexture(GL_TEXTURE_1D, fbo_texture);
        glTexImage2D(GL_TEXTURE_1D, 0, GL_RGBA, *screen_width, *screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_1D, 0);
        
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, *screen_width, *screen_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        fprintf(stdout, "Resize %d %d\n", *screen_width,*screen_height);
    }
protected:
    GLuint programID;
    GLuint fbo, fbo_texture, rbo_depth;
private:
    GLuint vbo_fbo_vertices;
    GLuint UniformVertex,UniformTexture;
    char* vertexShaderFile;
    char* fragmentShaderFile;
    int (*screen_width),(*screen_height);
    
    void initShader(){
        programID = createProgram("effect_vertex_shader.vert","effect_simple.frag");
        UniformVertex = glGetAttribLocation(programID, "v_coord");
        if (UniformVertex == -1) {
            fprintf(stderr, "Could not bind attribute %s\n", "v_coord");
        }
        UniformTexture = glGetUniformLocation(programID, "fbo_texture");
        if (UniformTexture == -1) {
            fprintf(stderr, "Could not bind attribute %s\n", "fbo_texture");
        }
    }
    
    void initVertices(){
        /* init_resources */
        GLfloat fbo_vertices[] = {
            -1, -1,
            1, -1,
            -1,  1,
            1,  1,
        };
        glGenBuffers(1, &vbo_fbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    }
    void initFrameBuffer(){
        /* Texture */
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_1D, fbo_texture);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_1D, 0, GL_RGBA, *screen_width, *screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_1D, 0);
        
        /* Depth buffer */
        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, *screen_width, *screen_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        /* Framebuffer to link everything together */
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_1D, fbo_texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
        GLenum status;
        if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
            return;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

};


#endif
