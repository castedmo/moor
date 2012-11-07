#if defined (WIN32) && (!defined MOOR_STATIC)
  #ifdef moor_EXPORTS
    #define MOOR_API __declspec(dllexport)
  #else
    #define MOOR_API __declspec(dllimport)
  #endif
#elif defined (__GNUC__)
  #define MOOR_API __attribute__ ((__visibility__("default")))
#else
  #defined MOOR_API
#endif
