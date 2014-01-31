/** ----------------------------------------------------------
 * \class VSDebugLib
 *
 * Lighthouse3D
 *
 * VSDebugLib - Very Simple Debug Library
 *
 * \version 0.1.0
 * Initial Release
 *
 * This class provides a simple interface to
 * ARB_debug_output extension. It includes a
 * callback function that prints out
 * the call stack
 *
 * This class was built for the book
 *    "OpenGL Insights"
 *
 * This lib requires:
 * External Dependencies
 *	GLEW (http://glew.sourceforge.net/)
 *
 * Full documentation at
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#include "gl_debug.h"
#include <string.h>

#ifdef _LINUX
#include <execinfo.h>
#endif

int Init_DebugOutput() {

    // check if the extension is there
    char* s;
    int i = 0, max;
    glGetIntegerv (GL_NUM_EXTENSIONS, &max);

    do {
        s = NULL;
        s = (char*) glGetStringi (GL_EXTENSIONS, i++);
    }
    while (i < max && strcmp (s, "GL_ARB_debug_output") != 0);

    // if we have the extension then ...
    if (strcmp (s, "GL_ARB_debug_output") == 0) {
        // enable sync mode and set the callback
        glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB (DebugLog, spUserParam);
        // On récupère déjà manuellement les messages du compilateur
        // Donc on désactive la gestion auto
        glDebugMessageControlARB (GL_DEBUG_SOURCE_SHADER_COMPILER_ARB, GL_DONT_CARE,
                              GL_DONT_CARE, 0, NULL, false);
        return true;
    }
    else // extension has not been loaded
    {
        printf("Note: GL_ARB_debug_output unsupported by your videocard\n");
        return false;
    }
}

// Sets the pointer to user data
void setUserParam (void* p) {

    spUserParam = p;
    glDebugMessageCallbackARB (DebugLog, spUserParam);
}

// clears the message log
void clearMessageLog() {

    int totalMessages;
    glGetIntegerv (GL_DEBUG_LOGGED_MESSAGES_ARB, &totalMessages);
    glGetDebugMessageLogARB (totalMessages, 0, NULL, NULL, NULL, NULL, NULL, NULL);
}

// Adds an application event
void addApplicationMessage (GLuint id, GLenum type, GLenum severity, char* message) {

    glDebugMessageInsertARB (GL_DEBUG_SOURCE_APPLICATION_ARB, type, id,
                             severity, -1, message );
}

// Adds a third party event
void addThirdPartyMessage (GLuint id, GLenum type, GLenum severity, char* message) {

    glDebugMessageInsertARB (GL_DEBUG_SOURCE_THIRD_PARTY_ARB, type, id,
                             severity, -1, message );
}

// Enables/disables the callback
void enableCallback (bool enable) {

    if (enable)
        glDebugMessageCallbackARB (DebugLog, NULL);
    else
        glDebugMessageCallbackARB (NULL, NULL);
}

// Enables/disables low severity events
void enableLowSeverityMessages (bool enabled) {

    glDebugMessageControlARB (GL_DONT_CARE, GL_DONT_CARE,
                              GL_DEBUG_SEVERITY_LOW_ARB, 0, NULL, enabled);

}

// enables/disables user events (app and third party)
void enableUserMessages (bool enabled) {

    glDebugMessageControlARB (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DONT_CARE,
                              GL_DONT_CARE, 0, NULL, enabled);
    glDebugMessageControlARB (GL_DEBUG_SOURCE_THIRD_PARTY_ARB, GL_DONT_CARE,
                              GL_DONT_CARE, 0, NULL, enabled);
}

// --------------------------------------------------


/*CALLBACK */
extern void STDCALL DebugLog (GLenum source,
          GLenum type,
          GLuint id,
          GLenum severity,
          GLsizei length,
          const GLchar* message,
          void* userParam) {

    if (length && userParam) {}

    printf(" -- \n Type: %s ; Source: %s ; ID: %d ; Severity: %s \n %s\n",
           getStringForType (type),
           getStringForSource (source),
           id,
           getStringForSeverity (severity),
           message);

    	//exit(1);
}

// aux function to translate source to string
char* getStringForSource (GLenum source) {

    switch (source) {
    case GL_DEBUG_SOURCE_API_ARB:
        return ("API");

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
        return ("Window System");

    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
        return ("Shader Compiler");

    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
        return ("Third Party");

    case GL_DEBUG_SOURCE_APPLICATION_ARB:
        return ("Application");

    case GL_DEBUG_SOURCE_OTHER_ARB:
        return ("Other");

    default:
        return ("");
    }
}

// aux function to translate severity to string
char* getStringForSeverity (GLenum severity) {

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
        return ("High");

    case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        return ("Medium");

    case GL_DEBUG_SEVERITY_LOW_ARB:
        return ("Low");

    default:
        return ("");
    }
}

// aux function to translate type to string
char* getStringForType (GLenum type) {

    switch (type) {
    case GL_DEBUG_TYPE_ERROR_ARB:
        return ("Error");

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        return ("Deprecated Behaviour");

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
        return ("Undefined Behaviour");

    case GL_DEBUG_TYPE_PORTABILITY_ARB:
        return ("Portability Issue");

    case GL_DEBUG_TYPE_PERFORMANCE_ARB:
        return ("Performance Issue");

    case GL_DEBUG_TYPE_OTHER_ARB:
        return ("Other");

    default:
        return ("");
    }
}
