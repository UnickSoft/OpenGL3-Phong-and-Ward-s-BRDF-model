//
//  ShaderLoader.h
//  OpenGLShading
//
//  Created by Олег on 08.03.16.
//  Copyright © 2016 Oleg. All rights reserved.
//

#ifndef Shader_h
#define Shader_h

#include <string.h>
#include "Resource.h"
#include "Logger.h"

class ShaderProgram : public Resource
{
public:
    
    ShaderProgram (const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName)
    {
        std::string vertexShaderFullPath = getFullFileName(vertexShaderFileName);
        std::string fragmentShaderFullPath = getFullFileName(fragmentShaderFileName);
        
        m_vertexShader.Source(getFileContent(vertexShaderFullPath));
        m_vertexShader.Compile();
        m_fragmentShader.Source(getFileContent(fragmentShaderFullPath));
        m_fragmentShader.Compile();

        m_program.AttachShader(m_vertexShader);
        m_program.AttachShader(m_fragmentShader);
        
        m_program.Link();
    }
    
    void Use() const
    {
        m_program.Use();
    }
    
    const oglplus::Program& GetProgram() const
    {
        return m_program;
    }
    
private:
    
    
    std::string getFileContent(const std::string& fullFilename)
    {
        std::ifstream ifs(fullFilename);
        std::string res;
        
        if (ifs.is_open())
        {
            res = std::string( (std::istreambuf_iterator<char>(ifs) ),
                            (std::istreambuf_iterator<char>()) );
        }
        else
        {
            Logger().Get(logERROR) << "File not found: " << fullFilename;
        }
        
        return res;
    }
    
    oglplus::VertexShader   m_vertexShader;
    oglplus::FragmentShader m_fragmentShader;
    oglplus::Program        m_program;
};


#endif /* Shader_h */
