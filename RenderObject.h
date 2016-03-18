//
//  RenderObject.h
//  OpenGLShading
//
//  Created by Олег on 10.03.16.
//  Copyright © 2016 Oleg. All rights reserved.
//

#ifndef RenderObject_h
#define RenderObject_h

#include "ShaderProgram.h"

class RenderObject
{
public:
    
    RenderObject(const oglplus::VertexArray& vertexArray,
              const ShaderProgram& program,
              const oglplus::Buffer& indexes) :
        vertexArray(vertexArray), program(program), indexes(indexes)
    {
        nIndexCount = 0;
        primitiveType = oglplus::PrimitiveType::TriangleStrip;
    }
    
    void Init(oglplus::PrimitiveType primitiveType, int nIndexCount)
    {
        this->primitiveType = primitiveType;
        this->nIndexCount = nIndexCount;
    }
    
    void Render(oglplus::Context* pGLContext)
    {
        program.Use();
        vertexArray.Bind();
        indexes.Bind(oglplus::Buffer::Target::ElementArray);
        
        ApplyParam(paramMapMat4f, uniformMapMat4f);
        ApplyParam(paramMapVec3f, uniformMapVec3f);
        ApplyParam(paramMapFloat, uniformMapFloat);
        
        pGLContext->DrawElements(primitiveType, nIndexCount, oglplus::DataType::UnsignedInt);
        
        //pGLContext->DrawArrays(primitiveType, 0, nIndexCount);
    }
    
    void AddMatrix4fParam(const std::string& paramName, oglplus::Mat4f initValue)
    {
        AddParam(paramName, initValue, paramMapMat4f, uniformMapMat4f);
    }
    
    void SetMatrix4fParam(const std::string& paramName, oglplus::Mat4f value)
    {
        SetParam(paramName, value, paramMapMat4f);
    }
    
    void AddVector3fParam(const std::string& paramName, oglplus::Vec3f initValue)
    {
        AddParam(paramName, initValue, paramMapVec3f, uniformMapVec3f);
    }
    
    void SetVector3fParam(const std::string& paramName, oglplus::Vec3f value)
    {
        SetParam(paramName, value, paramMapVec3f);
    }
    
    void AddFloatParam(const std::string& paramName, float initValue)
    {
        AddParam(paramName, initValue, paramMapFloat, uniformMapFloat);
    }
    
    void SetFloatParam(const std::string& paramName, float value)
    {
        SetParam(paramName, value, paramMapFloat);
    }
    
protected:
    
    template <typename T> void AddParam(const std::string& paramName, T initValue, std::map<std::string, T>& paramMap, std::map<std::string, oglplus::Uniform<T>>& unitformMap)
    {
        paramMap[paramName] = initValue;
        unitformMap[paramName] = oglplus::Uniform<oglplus::Mat4f>(program.GetProgram(), paramName);
    }
    
    template <typename T> void SetParam(const std::string& paramName, T initValue, std::map<std::string, T>& paramMap)
    {
        paramMap[paramName] = initValue;
    }
    
    template <typename T> void ApplyParam(std::map<std::string, T>& paramMap, std::map<std::string, oglplus::Uniform<T>>& unitformMap)
    {
        std::for_each(paramMap.begin(), paramMap.end(),
                      [&](std::pair<const std::string, T>& element)
                        {
                            unitformMap[element.first] = element.second;
                        });
    }
    
    const oglplus::VertexArray& vertexArray;
    const ShaderProgram&        program;
    oglplus::PrimitiveType      primitiveType;
    const oglplus::Buffer&      indexes;
    int                         nIndexCount;
    
    // Own property
    std::map<std::string, oglplus::Mat4f> paramMapMat4f;
    std::map<std::string, oglplus::Uniform<oglplus::Mat4f>> uniformMapMat4f;
    
    std::map<std::string, oglplus::Vec3f> paramMapVec3f;
    std::map<std::string, oglplus::Uniform<oglplus::Vec3f>> uniformMapVec3f;
    
    std::map<std::string, float> paramMapFloat;
    std::map<std::string, oglplus::Uniform<float>> uniformMapFloat;
};


#endif /* RenderObject_h */
