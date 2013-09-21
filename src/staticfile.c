#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

char *make_path(char *dir, char *uri, char *indexFile)
{
    int dirLength = strlen(dir);
    int uriLength = strlen(uri);
    int fileLength = (indexFile == NULL) ? 0 : strlen(indexFile);
    char *fullPath = malloc(
                         dirLength +
                         uriLength +
                         fileLength +
                         1);
    strcpy(fullPath, dir);
    strcat(fullPath, uri);
    if(fullPath[dirLength + uriLength - 1] == '/') {
        fullPath[dirLength + uriLength - 1] = '\0';
    }
    if(indexFile != NULL) {
        strcat(fullPath, indexFile);
    }
    return fullPath;
}

void del_path(char *path) {
    free(path);
}


char *getHTTPDate(time_t t)
{
    char *dateStr = malloc(256);
    if( ! dateStr) return NULL;
    struct tm ctm = *gmtime(&t);
    strftime(dateStr, 256, "%a, %d %b %Y %H:%M:%S %Z\r\n", &ctm);
    return dateStr;
}

char *getContentLength(int size)
{
    char *ctlen = malloc(64);
    if(! ctlen) return NULL;
    snprintf(ctlen, 64, "%d\r\n", size);
    return ctlen;
}

void getMIMEType(char *path, char **mimetype)
{
    char *ext = strrchr(path, '.');
    if(ext == NULL) {
        *mimetype = "application/octet-stream\r\n";
    }
    if(strcmp(ext, ".html") == 0) {
        *mimetype = "text/html\r\n";
    }
    else if(strcmp(ext, ".htm") == 0) {
        *mimetype = "text/html\r\n";
    }
    else if(strcmp(ext, ".css") == 0) {
        *mimetype = "text/css\r\n";
    }
    else if(strcmp(ext, ".png") == 0) {
        *mimetype = "image/png\r\n";
    }
    else if(strcmp(ext, ".jpeg") == 0) {
        *mimetype = "image/jpeg\r\n";
    }
    else if(strcmp(ext, ".gif") == 0) {
        *mimetype = "image/gif\r\n";
    }
    else {
        *mimetype = "application/octet-stream\r\n";
    }

}
