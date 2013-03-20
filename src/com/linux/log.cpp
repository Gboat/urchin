#include "com/Com.h"
#include <stdarg.h>
#include <stdio.h>
#include "com/linux/internal.h"

void firtex::com::internal::log_printf( firtex::com::internal::log_level ll, char const* format, ... )
{
    switch ( ll ) {
    case ERROR:
        fprintf( stderr, "ERROR:  " );
        break;
    case WARN:
        fprintf( stderr, "WARNING:  " );
        break;
    case INFO:
        fprintf( stderr, "INFO:  " );
        break;
    case NONE:
    	break;
    default:
        fprintf( stderr, "MESSAGE:  " );
        break;
    }
    
    va_list ap;
    va_start(ap, format);
    vfprintf( stderr, format, ap );
    va_end(ap);
}
