//
//  Resource.h
//  OpenGLShading
//
//  Created by Олег on 08.03.16.
//  Copyright © 2016 Oleg. All rights reserved.
//

#ifndef Resource_h
#define Resource_h

#include <string.h>

class Resource
{
public:
    
    // @return full filename.
    std::string getFullFileName(const std::string& filename)
    {
        return getResoucesDir() + filename;
    }
    
    static std::string getResoucesDir()
    {
        return getResourceDir();
    }
    
    static void setRootDir (const std::string& sourceMainFile)
    {
        getResourceDir() = sourceMainFile.substr(0, sourceMainFile.rfind("/")) + std::string("/Resources/");;
    }
    
protected:
    
    static std::string& getResourceDir()
    {
        static std::string resourcesDir;
        return resourcesDir;
    }
    
    
};

#endif /* Resource_h */
