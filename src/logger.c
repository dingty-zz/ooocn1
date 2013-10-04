#include <stdio.h>
#include <stdarg.h>
#include <logger.h>

FILE *log_fp;

int log_init(char *logfile){
    log_fp = fopen(logfile, "w");
    fprintf(log_fp, "Hello\n");
    if(! log_fp){
      /*log_fp = stdout; // for debugging*/
      return -1;
    }
    return 0;
}

/*
@brief
  This log function simulates normal logging system.
  LIMITATION:
    For varaibles passed behind, currently it supports only "%d" and "%s"
 */
void log_print(int level, char* filename, int line, char *fmt,...)
{
    va_list         list;
    char            *p, *r;
    int             e;

    switch (level) {
    case LOG_DEBUG:
        fprintf(log_fp,"-DEBUG: ");
        break;
    case LOG_INFO:
        fprintf(log_fp,"-INFO: ");
        break;
    case LOG_WARN:
        fprintf(log_fp,"-WARN: ");
        break;
    case LOG_ERROR:
        fprintf(log_fp,"-ERROR: ");
        break;
    default:
        break;
    }

    fprintf(log_fp,"[%s][line: %d] ",filename,line);
    va_start( list, fmt );

    for ( p = fmt ; *p ; ++p )
    {
        if ( *p != '%' )//If simple string
        {
            fputc( *p,log_fp );
        }
        else
        {
            switch ( *++p )
            {
                /* string */
            case 's':
            {
                r = va_arg( list, char * );

                fprintf(log_fp,"%s", r);
                continue;
            }

            /* integer */
            case 'd':
            {
                e = va_arg( list, int );

                fprintf(log_fp,"%d", e);
                continue;
            }

            default:
                fputc( *p, log_fp );
            }
        }
    }
    va_end( list );
    fputc( '\n', log_fp );
}
