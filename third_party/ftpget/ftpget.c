/* 

   ftpget.c (M. Andreoli)
   Heavy based on snarf code by Zachary Beane 1996
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <unistd.h>

#define MAXLEN 4096

#define LOG(x,y) if(verbose) fprintf(stderr,"%s %s",x,y)

struct url {
    char u_service[24];
    char u_host[128];
    unsigned short u_port;
    char u_path[255];
    char u_file[128];
    char user[16];
    char password[16];
    char cmd[256];
};

int resume=0, verbose=0;
int upload_stdin=0;

static int getresponse (int, char *) ;
off_t getsize(const char *);

enum {RETR, STOR, LIST};
int op;

int rc;

int usage()
{
    printf("ftpget: connection-less FTP client (1999,by M.Andreoli) \n \
            Usage:\n \  
            \tftpget -h host {FTP hostname}\n \
            \t[-u user -p password ] {default: anonymous login}\n \
            \t[-c command ] {optional FTP command, after auth phase}\n \
            \t[-d dir/] \n \
            \t[ -f file] {without -f, send LIST command}\n \
            \t[-i] {upload from stdin, instead of download to stdout}\n \
            \t[-v] {verbose log}\n \
            \n \
            ");
    exit(0);
}


#define BIG_BUFFER_SIZE 4096

/* Reads a \n terminated line (includes the \n) */
int readline (int fd, char *line) 
{
    int n, rc = 0;
    char c ;

    for (n = 1 ; n < BIG_BUFFER_SIZE ; n++)
    {
        if ((rc == read (fd, &c, 1)) != 1)
        {
            *line++ = c ;
            if (c == '\n')
                break ;
        }
        else if (rc == 0)
        {
            if (n == 1)
                return 0 ; /* EOF */
            else
                break ; /* unexpected EOF */
        }
        else 
            return -1 ;
    }

    *line = 0 ;
    return n ;
}

int dump_data(int fd_in, int fd_out){
    int i;
    char recv_buf[BIG_BUFFER_SIZE];

    while(i = read(fd_in, recv_buf, sizeof(recv_buf)))
        write(fd_out, recv_buf, i);

    return(1);
}

int upload( int fd_out){
    int n;
    char recv_buf[BIG_BUFFER_SIZE];

    while ( 1 ) {
        n=read(0, recv_buf, sizeof(recv_buf) );
        if (n == 0) break;          /* EOF.  */
        if (n < 0) return -2;
        write(fd_out, recv_buf, n);
    }
    return(0);
}


/* -- fine misc --*/

int ftp_get (struct url u, char *outfile)
{
    struct hostent *host ;
    struct sockaddr_in sin ;
    int s, control, data, tmp ;
    char send_buf[MAXLEN] ; 
    char line[MAXLEN] ;
    unsigned char *a, *p ;
    int fd, open_flags = O_CREAT | O_WRONLY;
    int response;
    off_t file_offset;

    if(resume)
        open_flags |= O_APPEND;
    else
        open_flags |= O_TRUNC;

    /* get the host's IP addr */
    if((host = gethostbyname(u.u_host)) == NULL) 
    {
        fprintf(stderr, "%s: host not found.\n", u.u_host);
        return(1);
    }

    /* get the socket */
    if((control = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        fprintf(stderr, "Unable to create socket.\n");
        return(1);
    }

    /* connect the socket, filling in the important stuff */
    sin.sin_family = AF_INET;
    sin.sin_port = htons(u.u_port);
    bcopy(host->h_addr, &sin.sin_addr, host->h_length);

    if(connect(control, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        fprintf(stderr, "Connection to host %s refused.\n", u.u_host);
        return(1);
    }

    LOG("connected to",u.u_host);

    if (rc=getresponse (control, line) != 220) /* ready for new user */
        return rc ;

    sprintf (send_buf, "USER %s\r\n", u.user) ;
    LOG("send",send_buf);
    send (control, send_buf, strlen (send_buf), 0) ;

    response = getresponse(control, line);
    if (response != 331 && response != 230) /* login ok, enter passwd 
                                               - or, you're logged in without 
                                               further info necessary
                                               - zpb */
        return response ;
    sprintf (send_buf, "PASS %s\r\n", u.password) ;
    /* some ftp servers may require rest of address too - not done 
       - actually most are pretty flexible, and just warn you if you don't
       put in the whole thing; i've yet to see one that actually requires
       the whole thing
       - zpb */
    LOG("send",send_buf);
    send (control, send_buf, strlen (send_buf), 0) ;
#ifdef DEBUG
    fprintf(stderr, "checking guest login ability\n");
#endif
    response = getresponse(control, line);
    if (response != 230 && response != 202) /* guest login ok 
                                               - or no passwd needed
                                               - zpb */
        return response ;

    if (strlen(u.u_path)!=0)
    {
        sprintf (send_buf, "CWD %s\r\n", u.u_path) ;
        LOG("send",send_buf);
        send (control, send_buf, strlen (send_buf), 0) ;

        if (rc=getresponse (control, line) != 250) /* cwd command successful */
            return rc ;
    }


    sprintf (send_buf, "TYPE I\r\n") ; /* always use binary */
    LOG("send",send_buf);
    send (control, send_buf, strlen (send_buf), 0) ;
    if (rc=getresponse (control, line) != 200) /* type set to I */
        return rc ;

    /* user's command */

    if ( strlen(u.cmd)!=0 )
    {
        sprintf (send_buf, "%s\r\n", u.cmd) ;
        LOG("send",send_buf);
        send (control, send_buf, strlen (send_buf), 0) ;

        if (getresponse (control, line) != 250)
            /*return 0*/ ;
    }



    /* now for the dirty part - create data connection and suck data from
       server */

    if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        fprintf(stderr, "Unable to create socket, shitola.\n");
        return(1);
    }
    tmp = sizeof (sin) ; 
    /* We use PORT, rather than SO_REUSEADDR. However, you can easily modify
       the code if you want that */
    getsockname (control, (struct sockaddr *)&sin, &tmp) ; 
    sin.sin_port = 0 ; /* let system choose a port */
    if (bind (s, (struct sockaddr *)&sin, sizeof (sin)) < 0)
    {
        fprintf (stderr, "Can't bind.\n") ; 
        return 1 ;
    }
    getsockname (s, (struct sockaddr *)&sin, &tmp) ;
    listen (s, 1) ;
    a = (unsigned char *)&sin.sin_addr ;
    p = (unsigned char *)&sin.sin_port ;
    sprintf (send_buf, "PORT %d,%d,%d,%d,%d,%d\r\n", a[0], a[1], a[2], a[3],
            p[0], p[1]) ;

#ifdef DEBUG
    fprintf (stderr, "PORT %d,%d,%d,%d,%d,%d\r\n", a[0], a[1], a[2], a[3],
            p[0], p[1]) ;
#endif

    LOG("send",send_buf);
    send (control, send_buf, strlen (send_buf), 0) ;
    if (rc=getresponse (control, line) != 200) /* port command successful */
        return rc ;

    /* get a file descriptor pointing at the right place */
    if(strcmp(outfile, "-") == 0)
        fd = 1;
    else
        if((fd = open(outfile, open_flags, 0644)) < 0) 
        {
            fprintf(stderr, "Unable to open output file %s for writing.\n", outfile);
            return(1);
        }

    /* if the resume option is set, get the size of the local file and tell
       the ftp server to restart the transfer at that offset in the remote
       file
       - xach */
    if(resume){
        file_offset = getsize(u.u_file);
        sprintf(send_buf, "REST %ld\r\n", file_offset);
        send(control, send_buf, strlen(send_buf), 0);
        if(rc=getresponse(control, line) != 350)
            return rc ;
    }

    /* if URL ends in a /, we ask for a directory listing */    

    if (strcmp (u.u_file, ""))
    {
        if ( upload_stdin)
        { op=STOR;
            sprintf (send_buf, "STOR %s\r\n", u.u_file) ;
        }
        else
        { op=RETR;
            sprintf (send_buf, "RETR %s\r\n", u.u_file) ;
        }
    }
    else
    {
        sprintf (send_buf, "LIST\r\n") ;
        op=LIST;
    }

    LOG("send",send_buf);

    send (control, send_buf, strlen (send_buf), 0) ;

    getresponse (control, line); 

    /*
       if (rc=getresponse (control, line) != 150)
       return rc ;
     */

    data = accept (s, (struct sockaddr *)&sin, &tmp) ;
    if (data < 0)
    {
        fprintf (stderr, "Accept error.\n") ;
        return 1 ;
    }


    switch(op)
    {
        case LIST:
        case RETR:


            LOG("dump data\n","");
            dump_data(data, fd);
            break;
        case STOR:
            LOG("dump stdin\n","");
            upload(data);
            close(data);

    }

    getresponse (control, line) ;
    sprintf (send_buf, "QUIT\r\n") ;
    LOG("send",send_buf);
    send (control, send_buf, strlen (send_buf), 0) ;
    getresponse (control, line) ;

    close (fd) ;
    close (control) ;
    close (data) ;
    close (s) ;
    return 0 ;
}


/* Gets replies from server, ignores informational lines, and returns
   reply code */

static int getresponse (int fd, char *line) 
{
    int read ;

    while ((read = readline (fd, line)) > 0)
    {
#ifdef DEBUG
        fprintf(stderr, "%c\n", line[0]);
#endif
        if (verbose) 
            fprintf (stderr, "%s", line) ;
        if (line[3] != '-' && (isdigit(line[0])))
            /* if it's not some informative 
               message */
            /* - actually, microsoft ftp servers (and perhaps others) are lame
               enough to reply with status messages that have a space as the 
               first letter. stupid! i think. -zpb 
               whoops...the rfc says you can do that, heh. you have an initial
               response that starts with a number and a dash, then iwhatever you
               like, and then a line with the actual status number - zpb */
            break ;
    }
    if (read <= 0) 
        return read ;
    else
        return atoi (line) ;
}


/* this function returns the size of a file (if it exists) or
   zero if it doesnt */
off_t getsize(const char *file){
    struct stat file_info;

    if(stat(file, &file_info) == -1)
        return(0);
    else
        return(file_info.st_size);
}


main(int argc, char **argv)
{
    int i=1;
    struct url u;

    u.u_port=21;
    sprintf(u.user,"anonymous");
    sprintf(u.password,"a@b.it");
    sprintf(u.u_path,"/");
    sprintf(u.u_file,"");
    sprintf(u.u_host,"");
    sprintf(u.cmd,"");

    while (i<=argc-1)
    {

        if ( argv[i][0] == '-' )
            switch( argv[i][1] )
            {
                case 'h': { i++; strcpy(u.u_host,argv[i]); break;}
                case 'u': { i++; strcpy(u.user,argv[i]); break;}
                case 'p': { i++; strcpy(u.password,argv[i]); break;}
                case 'd': { i++; strcpy(u.u_path,argv[i]); break;}
                case 'f': { i++; strcpy(u.u_file,argv[i]); break;}
                case 'c': { i++; strcpy(u.cmd,argv[i]); break;}
                case 'i': { upload_stdin=1; break;}
                case 'v': { verbose=1; break;}
                default: usage();
            }
        i++;
    }

    if ( strcmp(u.u_host,"") == 0 ) usage();

    /*
       fprintf(stderr,
       "host=%s, user=%s, pass=%s, \ndir=%s, file=%s\n",
       u.u_host,u.user,u.password,u.u_path, u.u_file
       );
     */

    return ftp_get(u,"-");
}
