#include "webServ.h"
webServ::webServ(int _port)
	:port(_port)
{
}
bool webServ::_openListenfd()
{
	sockaddr_in servaddr;
	if ((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
		return false;

	// Eliminates "Address already in use" error from bind 
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,static_cast<const void *>(&optval),sizeof(int))<0)
		return false;

	bzero(static_cast<char*>(&servaddr),sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(static_cast<unsigned short>port);
	if (bind(listenfd,static_cast<SA*>&servaddr,sizeof(servaddr))<0)
		return false;
	if (listen(listenfd,LISTENQ)<0)
		return false;
	return true;
}
void webServ::servStart()
{
	sockaddr_in clientaddr; //save client's addr
	int clientlen;
	listenfd=_openListenfd(port);
	while(1)
	{
		clientlen=sizeof(clientaddr);
		connfd=Accept(listenfd,static_cast<SA*>&clientaddr,&clientlen);
		_doit();
		Close(connfd);
	}
}
void webServ::_doit()
{
	int is_static;
	stat sbuf;
	char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
	char filename[MAXLINE],cgiargs[MAXLINE];
	Rio_readinitb(&rio,fd);
	Rio_readlineb(&rio,buf,MAXLINE);
	sscanf(buf,"%s %s %s",method,uri,version);
	if (strcasecmp(method,"GET"))//Get or GET
	{
		_clientError(method,"501","Not Implemented","Tiny does not implement this method");
		return;
	}
	read_requesthdrs(&rio);
	is_static=parse_uri(uri,filename,cgiargs);
	if (stat(filename,&sbuf)<0)
	{
		_clientError(filename,"403","Forbidden","Tiny couldn't read the file");
		return;
	}
	if (is_static) // Static content
	{
		if (!(S_ISREG(sbuf.st_mode))||!(S_IRUSR & sbuf.st_mode))
		{
			_clientError(filename,"403","Forbidden","Tiny couldn't read the file");
			return;
		}
		servStatic(filename,sbuf.st_size);
	}
	else  //Dynamic content
	{
		if (!(S_ISREG(sbuf.st_mode))||!(S_IXUSR & sbuf.st_mode))
		{
			clienterror(filename,"403","Forbidden","Tiny couldn't run the CGI program");
			return;
		}
		servDynamic(filename,cgiargs);
	}

}
void webServ::servClose()
{
	
}
