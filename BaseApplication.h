//
//  BaseApplication.h
//  OpenGLShading
//
//  Created by Олег on 08.03.16.
//  Copyright © 2016 Oleg. All rights reserved.
//

#ifndef BaseApplication_h
#define BaseApplication_h

#include <GL/glew.h>
#include <OpenGL/gl3.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <oglplus/all.hpp>

#include "Logger.h"

class BaseApplication
{
public:
    
    // Run message loop.
    virtual void Run()
    {
        glfwSetWindowUserPointer (pWindow, this);
        
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(pWindow))
        {
            /* Render here */
            Render();
            
            /* Swap front and back buffers */
            glfwSwapBuffers(pWindow);
            
            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    
protected:
    
    // OGLplus context.
    oglplus::Context* pGLContext;
    // GLFW window;
    GLFWwindow* pWindow;
    
    BaseApplication () : BaseApplication (640, 480, "OpenGL sample") {;}
    
    BaseApplication (int nWindowWidth, int nWindowHeight, const std::string& windowTitle) : pGLContext(nullptr), pWindow(nullptr)
    {
        /* Initialize the library */
        if (glfwInit())
        {
            
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            /* Create a windowed mode window and its OpenGL context */
            pWindow = glfwCreateWindow(nWindowWidth, nWindowHeight, windowTitle.data(), NULL, NULL);
            if (pWindow)
            {
                //Setup callbacks
                glfwSetFramebufferSizeCallback(pWindow, OnResizeStatic);
                glfwSetKeyCallback(pWindow, OnKeyPress);
                
                /* Make the window's context current */
                glfwMakeContextCurrent(pWindow);
                
                glewExperimental = GL_TRUE;
                GLenum err = glewInit();
                if (GLEW_OK == err)
                {
                    // Need call after glewInit, because we can have gl error and oglplus will catch it.
                    glGetError();
                    pGLContext = new oglplus::Context ();
                }
                else
                {
                    /* Problem: glewInit failed, something is seriously wrong. */
                    Logger().Get(logERROR) << "Error: " << glewGetErrorString(err);
                }
            }
            else
            {
                /* Problem: glewInit failed, something is seriously wrong. */
                Logger().Get(logERROR) << "Cannot create glfw Window";
            }
        }
        else
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            Logger().Get(logERROR) << "Cannot init glfw";
        }
    }
    
    ~BaseApplication ()
    {
        if (pWindow)
        {
            glfwDestroyWindow(pWindow);
            glfwTerminate();
        }
    }
    
    
    // Render something in this function.
    virtual void Render() = 0;

    // Widnow events.
    virtual void OnResize(int width, int height)
    {
        if (pGLContext)
        {
            pGLContext->Viewport(0, 0, width, height);
        }
    }
    
    virtual void OnKeyPress(int key) {;}
    
    void GetWidnowSize(int& width, int& height)
    {
        glfwGetWindowSize(pWindow, &width, &height);
    }
    
private:
    
    static void OnResizeStatic(GLFWwindow* window, int width, int height)
    {
        BaseApplication* ptr = (BaseApplication*)glfwGetWindowUserPointer(window);
        if (ptr)
        {
            ptr->OnResize(width, height);
        }
    }
    
    static void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        BaseApplication* ptr = (BaseApplication*)glfwGetWindowUserPointer(window);
        if (ptr && action == GLFW_PRESS)
        {
            ptr->OnKeyPress(key);
        }
    }
    
};


#endif /* BaseApplication_h */
