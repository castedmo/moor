#if defined (WIN32)
  #ifdef moor_EXPORTS
    #define MOOR_API __declspec(dllexport)
  #else
    #define MOOR_API __declspec(dllimport)
  #endif
#elif defined (__GNUC__)
  #define MOOR_API __attribute__ ((__visibility__("default")))
#endif
