#include "webServ.h"
#include "parse.h"
#include<cstdio>
webServ::webServ(int _port)
	:port(_port)
{
}
bool webServ::_openListenfd()
{
	int optval=1;
	struct sockaddr_in servaddr;
	if ((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
		return false;
	// Eliminates "Address already in use" error from bind 
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)(&optval),sizeof(int))<0)
		return false;

	bzero((char*)(&servaddr),sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons((unsigned short)port);
	if (bind(listenfd,(SA*)&servaddr,sizeof(servaddr))<0)
		return false;
	if (listen(listenfd,LISTENQ)<0)
		return false;
	return true;
}
void webServ::servStart()
{
	struct sockaddr_in clientaddr; //save client's addr
	int clientlen;
	_openListenfd();
	while(1)
	{
		clientlen=sizeof(clientaddr);
		connfd=Accept(listenfd,(SA*)&clientaddr,(socklen_t*)&clientlen);
		_doit();
		Close(connfd);
	}
}
void webServ::_doit()
{
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
	Rio_readinitb(&rio,connfd);
	Rio_readlineb(&rio,buf,MAXLINE);
	sscanf(buf,"%s %s %s",method,uri,version);
	if (strcasecmp(method,"GET"))//Get or GET
	{
		_clientError(method,"501","Not Implemented","Tiny does not implement this method");
		return;
	}
	//_printRequest();
	parse parseUri((string(uri)));
	filename=parseUri.filename().c_str();
	cgiargs=parseUri.cgiargs().c_str();
	filetype=parseUri.filetype().c_str();
	is_static=parseUri.isStatic();
	
	/* post method */
	if (strcasecmp("POST",method)) 
	{
		_postRequestHdrs();
		_postDynamic();
	}
	
	/*get method*/
	if (stat(filename, &sbuf) < 0) {                     
		_clientError(filename, "404", "Not found",
				"Tiny couldn't find this file");
		return;
	}                                                    
	if (is_static) { /* Serve static content */          
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { 
			_clientError(filename, "403", "Forbidden",
					"Tiny couldn't read the file");
			return;
		}
		filesize=sbuf.st_size;
		_servStatic();        
	}
	else { /* Serve dynamic content */
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) { 
			_clientError(filename, "403", "Forbidden",
					"Tiny couldn't run the CGI program");
			return;
		}
		_servDynamic();            
	}

}
void webServ::_getStatic()
{
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXBUF];

	/* Send response headers to client */
	sprintf(buf, "HTTP/1.0 200 OK\r\n");    
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(connfd, buf, strlen(buf));       

	/* Send response body to client */
	srcfd = Open(filename, O_RDONLY, 0);    
	srcp = (char*)Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
	Close(srcfd);                           
	Rio_writen(connfd, srcp, filesize);         
	Munmap(srcp, filesize);                 
}
void webServ::_getDynamic()
{
	char buf[MAXLINE], *emptylist[] = { NULL };

	/* Return first part of HTTP response */
	sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
	Rio_writen(connfd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(connfd, buf, strlen(buf));

	if (Fork() == 0) { /* child */ 
		/* Real server would set all CGI vars here */
		setenv("QUERY_STRING", cgiargs, 1); 
		Dup2(connfd, STDOUT_FILENO);         /* Redirect stdout to client */ 
		Execve(filename, emptylist, environ); /* Run CGI program */ 
	}
	Wait(NULL); /* Parent waits for and reaps child */ 
}
void webServ::_postDynamic()
{
	char buf[MAXLINE], *emptylist[] = { NULL }; /*parameter*/
	char data[MAXLINE];
	int p[2]; /*Pipe*/
	Pipe(p);

	if (Fork() == 0) /* child */
	{
		Close(p[0]);
		Rio_readnb(&rio,data,Content-length);
		Rio_writen(p[1],buf,Content-length);
		exit(0);
	}
	
	/* Return first part of HTTP response */
	sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
	Rio_writen(connfd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(connfd, buf, strlen(buf));
	
	Dup2(p[0],STDIN_FILENO);
	Close(p[0]);
	Close(p[1]);
	setenv("CONTENT-LENGTH",length , 1);
	Dup2(connfd,STDOUT_FILENO);
	Execve(filename, emptylist, environ);
}
void webServ::_clientError(const char *cause,const char *errnum,const char *shortmsg,const char *longmsg)
{
	char buf[MAXLINE], body[MAXBUF];

	/* Build the HTTP response body */
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

	/* Print the HTTP response */
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(connfd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(connfd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(connfd, buf, strlen(buf));
	Rio_writen(connfd, body, strlen(body));
}
void webServ::_getRequestHdrs()
{
	char buf[MAXLINE];
	Rio_readlineb(&rio,buf,MAXLINE);
	while(strcmp(buf,"\r\n"))
	{
		Rio_readlineb(&rio,buf,MAXLINE);
		printf("%s",buf);
	}
	return;
}
void webServ::_postRequestHdrs()
{
	char buf[MAXLINE];
	char *p;
	Rio_readlineb(&rio,buf,MAXLINE);
	while(strcmp(buf,"\r\n"))
	{
		Rio_readlineb(&rio,buf,MAXLINE);
		if ((strcasecmp(buf,"Content-length")==0))
		{
			p=&buf[15];
			p+=strspn(p," \t");
			contentlength=atoi(p);
		}
		printf("%s",buf);
	}
	return;
}
void webServ::servClose()
{

}
webServ::~webServ()
{
}
