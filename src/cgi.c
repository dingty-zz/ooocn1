#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <clientsocket.h>
#include <logger.h>
#include <netservice.h>

#include "http_internal.h"

#define CGI_ENV_VARNUM 22

char* ARGV[] = { "flaskr.py", NULL };

static int preprocess_cgi(ClientSocket *clisock);

int is_cgi( HttpRequest *request, HttpResponse *response) {
    return request->cgi == 1 && response->httpcode == 200
           && request->state == REQ_DONE ;
}

/*
If returns 0. means successfully processed.
if returns <0, means it needs to be further processed in process_response function.
 */
int process_cgi(ClientSocket *clisock) {
    int *stdin_pipe = clisock->stdin_pipe;
    int *stdout_pipe = clisock->stdout_pipe;
    struct timeval tv;
    fd_set rfds;
    int nready;

    if(! clisock->cgi_preprocessed) {
        if( preprocess_cgi(clisock) < 0) {
            clisock->response.httpcode = 500;
            return -1;
        }
    }

    FD_ZERO(&rfds);
    FD_SET(stdout_pipe[0], &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    nready = select(stdout_pipe[0] + 1, &rfds, NULL, NULL, &tv);
    if(nready <= 0) {
        return 0;
    }

    /* you want to be looping with select() telling you when to read */
    clisock->writeIndex = read(stdout_pipe[0], clisock->writebuf, CLISOCK_BUFSIZE);
    if( clisock->writeIndex > 0)
    {
        return 0;
    }

    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);

    start_pipelining(& clisock->request, & clisock->response);
    clisock->response.state = -1;
    logger(LOG_INFO, "Finished CGI");
    return 0;
}

static char *newENVPair(char *key, char *value) {
    int keylen = strlen(key);
    if(!value) value = "";
    int vallen = strlen(value);
    // "key=value'\0'"
    char *envPair = malloc(keylen + 1 + vallen + 1);

    if(! envPair) return NULL;
    strcpy(envPair, key);
    strcat(envPair, "=");
    strcat(envPair, value);
    return envPair;
}

static int prepare_envp(ClientSocket *clisock) {
    int i = 0;

    clisock->envp = malloc(CGI_ENV_VARNUM * sizeof( char *));
    if( ( clisock->envp[i++] =
                newENVPair("CONTENT_LENGTH", getValueByKey(& clisock->request.headers, "Content-Length")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("CONTENT_TYPE", getValueByKey(& clisock->request.headers, "Content-Type")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("GATEWAY_INTERFACE", "CGI/1.1"))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("REMOTE_ADDR", inet_ntoa(clisock->ip)))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("SCRIPT_NAME", "/cgi"))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("SERVER_PORT", (clisock->isHTTPS)? get_ssl_port(): get_server_port()))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("SERVER_PROTOCOL", "HTTP/1.1"))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("SERVER_SOFTWARE", "Liso/1.0"))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_ACCEPT", getValueByKey(& clisock->request.headers, "Accept")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_REFERER", getValueByKey(& clisock->request.headers, "Referer")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_ACCEPT_ENCODING", getValueByKey(& clisock->request.headers, "Accept-Encoding")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_ACCEPT_LANGUAGE", getValueByKey(& clisock->request.headers, "Accept-Language")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_ACCEPT_CHARSET", getValueByKey(& clisock->request.headers, "Accept-Charset")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_HOST", getValueByKey(& clisock->request.headers, "Host")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_COOKIE", getValueByKey(& clisock->request.headers, "Cookie")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_USER_AGENT", getValueByKey(& clisock->request.headers, "User-Agent")))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("HTTP_CONNECTION", getValueByKey(& clisock->request.headers, "Connection")))
            == NULL) {
        return -1;
    }

    char *uri = clisock->request.uri;
    char *path_info = strlen(uri) >= 5? &uri[5]: NULL; // "/cgi/"
    char *query_string = (path_info)? strchr(path_info, '?'): NULL;
    if(query_string) {
        *query_string = '\0';
        query_string ++;
    }

    if( ( clisock->envp[i++] =
                newENVPair("REQUEST_URI", clisock->request.uri))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("REQUEST_METHOD", clisock->request.method))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("PATH_INFO", path_info))
            == NULL) {
        return -1;
    }
    if( ( clisock->envp[i++] =
                newENVPair("QUERY_STRING", query_string))
            == NULL) {
        return -1;
    }

    clisock->envp[i++] = NULL;

    if(i != CGI_ENV_VARNUM) return -1;

    return 0;
}

static int preprocess_cgi(ClientSocket *clisock) {
    clisock->cgi_preprocessed = 1;

    /*************** BEGIN VARIABLE DECLARATIONS **************/
    int *stdin_pipe = clisock->stdin_pipe;
    int *stdout_pipe = clisock->stdout_pipe;
    /*************** END VARIABLE DECLARATIONS **************/

    /*************** BEGIN PIPE **************/
    /* 0 can be read from, 1 can be written to */
    if (pipe(stdin_pipe) < 0)
    {
        return -1;
    }

    if (pipe(stdout_pipe) < 0)
    {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        return -1;
    }
    /*************** END PIPE **************/

    /*************** BEGIN FORK **************/
    clisock->child_pid = fork();
    /* not good */
    if (clisock->child_pid < 0)
    {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        return -1;
    }

    /* child, setup environment, execve */
    if (clisock->child_pid == 0)
    {
        if(prepare_envp(clisock) < 0)
            exit(0);
        /*************** BEGIN EXECVE ****************/
        dup2(stdin_pipe[0], fileno(stdin));
        dup2(stdout_pipe[1], fileno(stdout));
        /* you should probably do something with stderr */

        /* pretty much no matter what, if it returns bad things happened... */
        execve( get_cgiProgram(), ARGV, clisock->envp);

        logger(LOG_ERROR, "Error happened in executing cgi script");
        exit(0);
        /*************** END EXECVE ****************/
    }

    if (clisock->child_pid > 0)
    {
        if (write(stdin_pipe[1],
                  clisock->request.content, clisock->request.ctLength) < 0)
        {
            close(stdin_pipe[0]);
            close(stdin_pipe[1]);
            close(stdout_pipe[0]);
            close(stdout_pipe[1]);
            return -1;
        }

        return 0 ;
    }
    /*************** END FORK **************/
    return -1; // stupid compiler
}
