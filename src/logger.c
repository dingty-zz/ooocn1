#include <stdio.h>
#include <stdarg.h>
#include <logger.h>

void log_print(int level, char* filename, int line, char *fmt,...)
{
    va_list         list;
    char            *p, *r;
    int             e;

    FILE *fp = stdout;

    switch (level) {
    case LOG_DEBUG:
        fprintf(fp,"-DEBUG: ");
        break;
    case LOG_INFO:
        fprintf(fp,"-INFO: ");
        break;
    case LOG_WARN:
        fprintf(fp,"-WARN: ");
        break;
    case LOG_ERROR:
        fprintf(fp,"-ERROR: ");
        break;
    default:
        break;
    }

    fprintf(fp,"[%s][line: %d] ",filename,line);
    va_start( list, fmt );

    for ( p = fmt ; *p ; ++p )
    {
        if ( *p != '%' )//If simple string
        {
            fputc( *p,fp );
        }
        else
        {
            switch ( *++p )
            {
                /* string */
            case 's':
            {
                r = va_arg( list, char * );

                fprintf(fp,"%s", r);
                continue;
            }

            /* integer */
            case 'd':
            {
                e = va_arg( list, int );

                fprintf(fp,"%d", e);
                continue;
            }

            default:
                fputc( *p, fp );
            }
        }
    }
    va_end( list );
    fputc( '\n', fp );
}
