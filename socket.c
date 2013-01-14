#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#include "ged.h"
#include "hash.h"
#include "croll.h"
#include "stringfunc.h"
#include "date.h"
#include "btree.h"
#include "chart.h"
#include "pathnames.h"
#include "token.h"
#include "socket.h"
#include "relationship.h"

extern int errno;

//#define TEST_IT

#ifdef TEST_IT
#define PORT 9998
#else
#define PORT 9999
#endif

int run = 1;
int departmental_connect_count = 0;
int local_connect_count = 0;
int connect_count = 0;
time_t start_time;

static void catch_children
(
    int sig 
#ifndef LINUX
	,
    int code,
    struct sigcontext *scp
#endif
)
{
int status;
int ThePid;

    /*SIGCHLD signals are blocked  by the system when entering this function*/
    while((ThePid = wait3(&status,WNOHANG,0)) != 0 && ThePid != -1);
    /*SIGCHLD signals are unblocked  by the system when leaving this function*/
#ifdef LINUX
	signal(sig, catch_children);
#endif
}

int do_reload = 0;

static void catch_SIGHUP
(
    int sig 
#ifndef LINUX
	,
    int code,
    struct sigcontext *scp
#endif
)
{
	do_reload = 1;
#ifdef LINUX
	signal(sig, catch_SIGHUP);
#endif
}

int main_loop()
{
int s;
int sd;
struct sockaddr_in record;
char buf[1024];
int i, j;
char *p;
int  on = 1;
int result = 0;
struct sockaddr_in address;
size_t addr_len;
int net_A, net_B, net_C;

	time(&start_time);
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {   printf("socket creation failed %d\n",errno);
        exit(0);
    }
    else
    	printf("Socket opened %d\n", s);
    	
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) == -1)
	printf("setsockopt failed: ignoring this\n");

    record.sin_family = AF_INET;
    record.sin_addr.s_addr = INADDR_ANY;
    record.sin_port = htons(PORT);
    if(bind(s, (struct sockaddr *)&record, sizeof(record)) == -1)
    {   printf("bind failed %d\n",errno);
        exit(0);
    }
    if(listen(s,5) == -1)
    {   printf("listen failed %d\n",errno);
        exit(0);
    }

		
	signal(SIGHUP, catch_SIGHUP);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, catch_children);
		
    for(;run == 1;)
    {
	addr_len = sizeof(address);
		if(do_reload)
		{
			do_reload = 0;
			reload(); //rereads the files.
		}
		
       	if((sd = accept(s, (struct sockaddr *)&address, &addr_len)) == -1)
       	{   	//perror("accept failed");
           	continue;
       	}

		net_A = ( address.sin_addr.s_addr & 0x000000FF );
		net_B = ( address.sin_addr.s_addr & 0x0000FF00 ) >> 8;
		net_C = ( address.sin_addr.s_addr & 0x00FF0000 ) >> 16;

		if( net_A == 130 && net_B == 216)
		{
			if(net_C == 32 || net_C == 33
			|| net_C == 34 || net_C == 35 || net_C == 36
			|| net_C == 39 || net_C == 104 || net_C == 105 
			|| net_C == 106 || net_C == 107 || net_C == 108 )
				departmental_connect_count++;
			else
				local_connect_count++;
		}
		else
			connect_count++;
#ifndef TEST_IT
		if((result = fork()) == -1)
		{
			close(sd);
			continue;
		}
#endif
		if(result == 0)
		{
		
#ifndef TEST_IT
			close(s); /*close the listening socket for the child*/
			signal(SIGPIPE, SIG_DFL);
#endif
			for(j = 0; (i = read(sd, &buf[j], 1024 - j)) > 0 && j < 1024; j += i)
			{
				buf[j + i] = '\0';
#ifdef TEST_IT
	       		printf("%s", buf);
	       		fflush(stdout);
#endif
				if( ( (p = strrchr(buf, '\n')) && p != buf 
					&& ( *(p-1) == '\n'
						|| (*(p-1) == '\r' &&  p != &buf[1]  && *(p-2) == '\n') ) )
						|| ( (p = strrchr(buf, '\r')) && p != buf 
						&& ( *(p-1) == '\r'
						|| (*(p-1) == '\n' &&  p != &buf[1]  && *(p-2) == '\r') ) ) )
	       		{
		   			Parse_and_send_response(sd, buf);
		   			break;
		   		}
			}
       		close(sd);
#ifndef TEST_IT
			exit(0);	
#endif
		}
		else
			close(sd);
    }
    
    shutdown(s,0);
    close(s);
}

void send_error(int sd, int error)
{
FILE *fp;
time_t t;

	if((fp = fdopen(sd, "w")) != 0)
	{
		time(&t);
		fprintf(fp, "HTTP/1.0 %d Not Found\n", error);
		fprintf(fp, "Date: %s", ctime(&t));
		fprintf(fp, "Server: gedserv/1.0\n");
		fprintf(fp, "Content-type: text/html\n\n");

		if(error == 404)
		{
			fprintf(fp, "<HEAD><TITLE>404 Not Found</TITLE></HEAD>\n");
			fprintf(fp, "<BODY><H1>404 Not Found</H1>\n");
			fprintf(fp, "The requested URL was not found on this server.\n");
			fprintf(fp, "</BODY>\n");
		}
		else
		{
			fprintf(fp, "<HEAD><TITLE>%d Error</TITLE></HEAD>\n", error);
			fprintf(fp, "<BODY><H1>Invalid Request</H1>\n");
			fprintf(fp, "Your client requested a transmission method other than those allowed by this server.\n");
			fprintf(fp, "</BODY>\n");
		}
		fflush(fp);
		fclose(fp);
	}
	else
		perror("fdopen");
	
}

void send_ok_header(FILE *fp, char *window)
{
time_t t;
extern time_t file_time;
char time_Buff[32];
struct tm result;
          
	time(&t);
	fprintf(fp, "HTTP/1.0 200\n");
	fprintf(fp, "Date: %s", asctime_r(gmtime_r(&t, &result), time_Buff ));
	fprintf(fp, "Server: gedserv/1.0\n");
	fprintf(fp, "Content-type: text/html\n");
//	if(window)
//		fprintf(fp, "Window-target: %s\n", window);
	fprintf(fp, "Last-modified: %s\n", asctime_r(gmtime_r(&file_time, &result), time_Buff ));
}

void stats(FILE *fp)
{
time_t t;
char time_Buff[32];
struct tm result;
          
	time(&t);
	fprintf(fp, "HTTP/1.0 200\n");
	fprintf(fp, "Date: %s", asctime_r(gmtime_r(&t, &result), time_Buff ));
	fprintf(fp, "Server: gedserv/1.0\n");
	fprintf(fp, "Content-type: text/html\n");
	fprintf(fp, "Last-modified: %s\n", asctime_r(gmtime_r(&t, &result), time_Buff ));

	fprintf(fp, "<html><head><title>Status</title></head>\n<body>\n");
	
	fprintf(fp, "Last Restart: %s<br>", asctime_r(gmtime_r(&start_time, &result), time_Buff ));
	fprintf(fp, "Current Date: %s<br>", asctime_r(gmtime_r(&t, &result), time_Buff ));
	fprintf(fp, "<hr><p>Number of Dept connections this run %d</p><p><hr>\n", departmental_connect_count);
	fprintf(fp, "<hr><p>Number of local connections this run %d</p><p><hr>\n", local_connect_count);
	fprintf(fp, "<p>Number of remote connections this run %d</p><p><hr>\n", connect_count);
	fprintf(fp, "</body></html>\n");
}

void robot(FILE *fp)
{
time_t t;
char time_Buff[32];
struct tm result;
          
	time(&t);
	fprintf(fp, "HTTP/1.0 200\n");
	fprintf(fp, "Date: %s", asctime_r(gmtime_r(&t, &result), time_Buff ));
	fprintf(fp, "Server: gedserv/1.0\n");
	fprintf(fp, "Content-type: text/plain\n");
	fprintf(fp, "Last-modified: %s\n", asctime_r(gmtime_r(&t, &result), time_Buff ));

	fprintf(fp, "User-agent: *\n");
	fprintf(fp, "Disallow: /html/\n");
	fprintf(fp, "Disallow: /ged/\n");
	fprintf(fp, "Disallow: /TA/\n");
	fprintf(fp, "Disallow: /TD/\n");
	fprintf(fp, "Disallow: /TDN/\n");
	fprintf(fp, "Disallow: /search\n");
}

void send_url(int sd, char *url, int headeronly)
{
ged_type *g, *h;
FILE *fp;
char *p;
int c;

	if(url == 0)
	{
		send_error(sd, 404)	;
		return;
	}

	if((fp = fdopen(sd, "w")) != 0)
	{
		
		if(strncmp(url, "/surname/index_", 15) == 0) //surname only index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				dump_Surname_as_html(fp, url[15], 0 );
			}
			else
				send_ok_header(fp, 0);
		}
		else if(strncmp(url, "/surnamef/index_", 16) == 0) //surname only index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				dump_Surname_as_html(fp, url[16], 1);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/index/index_", 13) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				dump_index_as_html(fp, url[13], 0);
			}
			else
				send_ok_header(fp, 0);

		}
		else if (strncmp(url, "/indexf/index_", 14) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				dump_index_as_html(fp, url[14], 1);
			}
			else
				send_ok_header(fp, 0);

		}
		else if (strncmp(url, "/index/?Surname=", 16) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				while((p = strchr(&url[16], '+')) != 0) *p = '.';
				while((p = strchr(&url[16], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[16], ' ')) != 0) *p = '.';
				dump_shortindex_as_html(fp,  &url[16], 0);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/search?Surname=", 16) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				while((p = strchr(&url[16], '+')) != 0) *p = '.';
				while((p = strchr(&url[16], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[16], ' ')) != 0) *p = '.';
				dump_shortindex_as_html(fp,  &url[16], 0);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/index/?Match=", 14) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				while((p = strchr(&url[14], '+')) != 0) *p = '.';
				while((p = strchr(&url[14], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[14], ' ')) != 0) *p = '.';
				dump_by_match(fp,  &url[14]);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/search?Match=", 14) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				while((p = strchr(&url[14], '+')) != 0) *p = '.';
				while((p = strchr(&url[14], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[14], ' ')) != 0) *p = '.';
				dump_by_match(fp,  &url[14]);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/index/", 7) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");
				while((p = strchr(&url[7], '+')) != 0) *p = '.';
				while((p = strchr(&url[7], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[7], ' ')) != 0) *p = '.';
				dump_shortindex_as_html(fp,  &url[7], 0);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/indexf/?Firstname=", 19) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");

				while((p = strchr(&url[19], '+')) != 0) *p = '.';
				while((p = strchr(&url[19], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[19], ' ')) != 0) *p = '.';
				dump_shortindex_as_html(fp,  &url[19], 1);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/search?Firstname=", 18) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");

				while((p = strchr(&url[18], '+')) != 0) *p = '.';
				while((p = strchr(&url[18], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[18], ' ')) != 0) *p = '.';
				dump_shortindex_as_html(fp,  &url[18], 1);
			}
			else
				send_ok_header(fp, 0);
		}
		else if (strncmp(url, "/indexf/", 8) == 0) //main index
		{
			if(!headeronly)
			{
				send_ok_header(fp, "IndexWindow");

				while((p = strchr(&url[8], '+')) != 0) *p = '.';
				while((p = strchr(&url[8], '%')) != 0) 
				{
					if(*(p+1) && *(p+2))
					{
						sscanf( p+1, "%x", &c);
						*p = (char) c ;
						strcpy(p+1, p+3);
					}
					else
						*p = '\0';
				}
				while((p = strchr(&url[8], ' ')) != 0) *p = '.';

				dump_shortindex_as_html(fp,  &url[8], 1);
			}
			else
				send_ok_header(fp, 0);
		}
		else if( strncmp(url, "/TA", 3) == 0) //create ancestor tree
		{
		int max_depth = 0;

			if((p = strchr(url, '.')) != 0)
				*p = '\0';

			if( isdigit(url[3]) )
			{
				sscanf(&url[3], "%d", &max_depth);
				if((p = strchr(&url[3], '/')) != 0)
					p++;
			}
			else
				p = &url[4];

			if( p != NULL && *p != '\0' && (g = find_hash(p)) != NULL )
			{
				if(!headeronly)
				{
					send_ok_header(fp, "IndexWindow");
					output_pedigree(fp, g, max_depth);
				}
				else
					send_ok_header(fp, 0);
			}
			else
				send_error(sd, 404)	;	
		}
		else if( strncmp(url, "/TD", 3) == 0 && url[3] != 'N') //create descendant tree
		{
		int max_depth = 0;

			if((p = strchr(url, '.')) != 0)
				*p = '\0';

			if( isdigit(url[3]) )
			{
				sscanf(&url[3], "%d", &max_depth);
				if((p = strchr(&url[3], '/')) != 0)
					p++;
			}
			else
				p = &url[4];

			if( p != NULL && *p != '\0' && (g = find_hash(p)) != NULL )
			{
				if(!headeronly)
				{
					send_ok_header(fp, "IndexWindow");
					output_decendants(fp, g, max_depth);
				}
				else
					send_ok_header(fp, 0);
			}
			else
				send_error(sd, 404)	;	

		}
		else if( strncmp(url, "/TDN", 4) == 0) //create descendant tree of this name only
		{
		int max_depth = 0;

			if((p = strchr(url, '.')) != 0)
				*p = '\0';

			if( isdigit(url[4]) )
			{
				sscanf(&url[4], "%d", &max_depth);
				if((p = strchr(&url[4], '/')) != 0)
					p++;
			}
			else
				p = &url[5];

			if( p != NULL && *p != '\0' && (g = find_hash(p)) != NULL )
			{
				if(!headeronly)
				{
					send_ok_header(fp, "IndexWindow");
					output_decendants_of_name(fp, g, max_depth);
				}
				else
					send_ok_header(fp, 0);
			}
			else
				send_error(sd, 404)	;	
		}
		else if( strncmp(url, "/html/", 6) == 0) //create family page
		{
			if((p = strchr(url, '.')) != 0)
				*p = '\0';
				
			if(	g = find_hash(&url[6]))
			{
				if(!headeronly)
				{
					send_ok_header(fp, "FamWindow");
					if(strcmp(g->type, "INDI") == 0)
						Indi_html_dump(fp, g);
					else
						Family_html_dump(fp, g);
				}
				else
					send_ok_header(fp, 0);
			}
			else
				send_error(sd, 404)	;	
		}
		else if( strncmp(url, "/ged/", 5) == 0) //create family page
		{
			if((p = strchr(url, '.')) != 0)
				*p = '\0';
			if(	g = find_hash(&url[5]))
			{
				if(!headeronly)
				{
					send_ok_header(fp, "FamWindow");
					Family_GED_dump(fp, g);
				}
				else
					send_ok_header(fp, 0);
			}
			else
				send_error(sd, 404)	;		
		}
		else if( strncmp(url, "/sour/", 6) == 0) //create family page
		{
			if((p = strchr(url, '.')) != 0)
				*p = '\0';
			if(	g = find_hash(&url[6]))
			{
				if(!headeronly)
				{
					send_ok_header(fp, "FamWindow");
					Family_GED_dump(fp, g);
				}
				else
					send_ok_header(fp, 0);
			}
			else
				send_error(sd, 404)	;		
		}
		else if( strncmp(url, "/repo/", 6) == 0) //create family page
		{
			if((p = strchr(url, '.')) != 0)
				*p = '\0';
			if(	g = find_hash(&url[6]))
			{
				if(!headeronly)
				{
					send_ok_header(fp, "FamWindow");
					Family_GED_dump(fp, g);
				}
				else
					send_ok_header(fp, 0);
			}
			else
				send_error(sd, 404)	;		
		}
		else if( strncmp(url, "/robot", 6) == 0) //Handle Robot Search engines
		{
			robot(fp);
		}
		else if( strncmp(url, "/status", 7) == 0) //Return Stats
		{
			stats(fp);
		}
		else if( strncmp(url, "/rel", 4) == 0) //create family page
		{
		int max_depth = 0;
		char *q;

			if( isdigit(url[4]) )
			{
				sscanf(&url[4], "%d", &max_depth);
				if((q = strchr(&url[4], '/')) != 0)
					q++;
			}
			else
				q = &url[5];

			if(q != NULL && *q != '\0' && (p = strchr(q, '/')) != 0)
			{
				*p++ = '\0';
				if(	(g = find_hash(q)) && (h = find_hash(p)))
				{
					if(!headeronly)
					{
						send_ok_header(fp, "IndexWindow");
#ifdef TEST_IT
						{
							ged_type *name;
						if(name = find_type(g, NAME))
							printf("Finding relationship of %s", name->data);
						else
							printf("Finding relationship of ?");
						if(name = find_type(h, NAME))
							printf("and %s\n", name->data);
						else
							printf("and ?\n");
						}
#endif
						find_relationship(fp, h, g, max_depth);
					}
					else
						send_ok_header(fp, 0);
				}
				else
					send_error(sd, 404)	;		
			}
			else
			{
				send_error(sd, 404)	;
			}
		}
		else
		{
 			send_error(sd, 404)	;	
 		}
		fflush(fp);
		fclose(fp);
	}	
	else
		perror("fdopen");
}

void Parse_and_send_response(int sd, char * buf)
{
char url[128];

	if(buf == 0)
	{
		send_error(sd, 400);
		return;
	}

	switch( token_to_num(Commands, strtok(buf, " \t")) )
	{
		case GET:
			send_url(sd, strtok(0,  " \t"), 0);
			break;
		case GET_HEAD:
			send_url(sd, strtok(0,  " \t"), 1);
			break;
		default:
			send_error(sd, 400);
			break;
	}
}

