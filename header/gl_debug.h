/** ----------------------------------------------------------
 * VSDebugLib
 *
 * Lighthouse3D
 *
 * VSDebugLib - Very Simple Debug Library
 * https://github.com/OpenGLInsights/OpenGLInsightsCode/tree/master/Chapter%2033%20ARB_debug_output%20A%20Helping%20Hand%20for%20Desperate%20Developers/OpenGLInsightsDebug
 *
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#ifndef __VSDL__
#define __VSDL__

#ifdef _WIN32
    #define STDCALL __stdcall
#else
    #define STDCALL
#endif

#ifdef _LINUX
    #define UNW_LOCAL_ONLY
    #include <libunwind.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <cxxabi.h>

    extern const char *__progname;
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "glew.h"

/// our callback
void STDCALL DebugLog(GLenum source,
                   GLenum type,
                   GLuint id,
                   GLenum severity,
                   GLsizei length,
                   const GLchar* message,
                   void* userParam);

/** Inits the library, sets the Callback function
 *
 * \param outStream where to record the message log
 * NULL means cout
 *
*/
int Init_DebugOutput();

/** Adds an application event to the log
 *
 * \param id the event id
 * \param type event's type. Can be one of the following:
        GL_DEBUG_TYPE_ERROR_ARB
        GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
        GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
        GL_DEBUG_TYPE_PORTABILITY_ARB
        GL_DEBUG_TYPE_PERFORMANCE_ARB
        GL_DEBUG_TYPE_OTHER_ARB
 * \param severity event's severity. Can be one of the following:
        GL_DEBUG_SEVERITY_HIGH_ARB
        GL_DEBUG_SEVERITY_MEDIUM_ARB
        GL_DEBUG_SEVERITY_LOW_ARB
 * \param message  event's message
*/
void addApplicationMessage(GLuint id, GLenum type,
                GLenum severity, char* message);

/** Adds a third party event to the log
 *
 * \param id the event id
 * \param type event's type. Can be one of the following:
        GL_DEBUG_TYPE_ERROR_ARB
        GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
        GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
        GL_DEBUG_TYPE_PORTABILITY_ARB
        GL_DEBUG_TYPE_PERFORMANCE_ARB
        GL_DEBUG_TYPE_OTHER_ARB
 * \param severity event's severity. Can be one of the following:
        GL_DEBUG_SEVERITY_HIGH_ARB
        GL_DEBUG_SEVERITY_MEDIUM_ARB
        GL_DEBUG_SEVERITY_LOW_ARB
 * \param message  event's message
*/
void addThirdPartyMessage(GLuint id, GLenum type,
                GLenum severity, char* message);

/// Enable/Disable the callback
void enableCallback(bool enable);

/// Enalbe/Disable Low severity events
void enableLowSeverityMessages(bool enabled);

/// Enables/Disables app and third party events
void enableUserMessages(bool enabled);

/// Sets user param for the callback
void setUserParam(void *p);

/// clears the message log
void clearMessageLog();

/// Pointer to user data
void *spUserParam;

/// Helper functions to print human-readable message
char* getStringForSource(GLenum source);
char* getStringForType(GLenum type);
char* getStringForSeverity(GLenum type);

/// The function to print the call stack

#if (defined _WIN32 || defined _LINUX)
void printStack();
#endif

#ifdef _LINUX
static int getFileAndLine (unw_word_t addr, char *file, size_t flen, int *line);
#endif



#endif
