#include "webServ.h"
#include "parse.h"
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
	Rio_readinitb(&rio,fd);
	Rio_readlineb(&rio,buf,MAXLINE);
	sscanf(buf,"%s %s %s",method,uri,version);
	if (strcasecmp(method,"GET"))//Get or GET
	{
		_clientError(method,"501","Not Implemented","Tiny does not implement this method");
		return;
	}
	read_requesthdrs(&rio);
	parse parseUri(string(buf));
	filename=parseUri.filename();
	cgiargs=parseUri.cgiargs();
	filetype=parseUri.filetype();
	is_static=parseUri.isStatic();
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
void webServ::_printRequest(rio_t *rp)
{
	char buf[MAXLINE];
	Rio_readlineb(rp,buf,MAXLINE);
	while(strcmp(buf,"\r\n"))
	{
		Rio_readlineb(rp,buf,MAXLINE);
		printf("%s",buf);
	}
	return;
}
void webServ::_servStatic()
{
	string header="";
	//Send response headers to client
	header+="HTTP/1.0 200 OK\r\n";
	header+="Server: Tiny Web Server\r\n";
	header+=("Content-length: "+filesize+"\r\n");
	header+=("Content-type: "+filetype+"\r\n\r\n");
	Rio_write(connfd,header.c_str(),header.length());

	//send response body to client
	char *srcp;
	int srcfd;
	srcfd=Open(filename,O_RDONLY,0);
	srcp=Mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);
	Close(srcfd);
	Rio_writen(fd,srcp,filesize);
	Munmap(srcp,filesize);
}
void webServ::_servDynamic()
{
	string header="";
	//Send response headers to client
	header+="HTTP/1.0 200 OK\r\n";
	header+="Server: Tiny Web Server\r\n";
	header+=("Content-length: "+filesize+"\r\n");
	header+=("Content-type: "+filetype+"\r\n\r\n");
	Rio_write(connfd,header.c_str(),header.length());

}
void webServ::servClose()
{
	
}
