#include "webServ.h"
webServ::webServ(int _port)
	:port(_port)
{
}
bool webServ::_OpenListenfd()
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
void webServ::ServStart()
{
	socket_in addr;
	addr
}
