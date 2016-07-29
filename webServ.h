#ifndef _WEBSERV_H
#define _WEBSERV_H
class webServ
{
	public:
		webServ(int _port);
		void ServStart();
		void ServClose();
	private:
		void _OpenListenfd();
		int port;
		int listenfd,connfd;
};
#endif
