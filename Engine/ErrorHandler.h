#ifndef _ERRORHANDLER_H
#define _ERRORHANDLER_H

#include <string>

namespace Engine
{
    /* Prints error message and exits the program */
    extern void applicationError(const std::string& errorMessage);

    /* If there are any errors return true and print the message about the error */
    extern bool errorCheckOpenGL();
}

#endif