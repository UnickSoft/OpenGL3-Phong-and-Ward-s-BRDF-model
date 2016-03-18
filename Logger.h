//
//  Logger.h
//  httpServer
//
//  Created by Олег on 21.02.16.
//  Copyright © 2016 Oleg. All rights reserved.
//



#ifndef Logger_h
#define Logger_h

#include <fstream>
#include <thread>
#include <sstream>

enum TLogLevel {logERROR, logINFO, logDEBUG};


class Logger
{
public:
    
    Logger() {;}
    
    
    virtual ~Logger()
    {
        os << std::endl;
     
        if (messageLevel >= Logger::ReportingLevel())
        {
            fprintf(stderr,"%s", os.str().c_str());
            fflush(stderr);
        }
    }
    
    
    
    std::ostringstream& Get(TLogLevel level = logINFO)
    {
        os << "- " << std::this_thread::get_id();
        os << " " << ToString(level) << ": ";
        messageLevel = level;
        return os;
    }
    
    
public:
    
    
    
    std::string ToString(TLogLevel level)
    {
        
        switch(level)
        {
            case logERROR: return "ERROR";
            case logINFO:  return "INFO";
            case logDEBUG: return "DEBUG";
            default: return "UNKNOWN";
        }
        
        return "";
    }
    
protected:
    
    static TLogLevel& ReportingLevel()
    {
        static TLogLevel reportingLevel = logERROR;
        return reportingLevel;
    }
    
    std::ostringstream os;
    
private:
    
    Logger(const Logger&) = delete;
    Logger& operator =(const Logger&) = delete;
    
private:
    
    TLogLevel messageLevel;
};





#endif /* Logger_h */

