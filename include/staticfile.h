#ifndef _STATICFILE_H
#define _STATICFILE_H


char *make_path(char *dir, char *path, char *indexFile);
void del_path(char *path);
char *getHTTPDate(time_t tm);
void getMIMEType(char *path, char **mimetype);
char *getContentLength(int size);

#endif // for #ifndef _STATICFILE_H
