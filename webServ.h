#ifndef _WEBSERV_H
#define _WEBSERV_H
class webServ
{
	public:
		webServ(int _port);
		void servStart();
		void servClose();
		~webServ();
	private:
		void _openListenfd();
		void _doit();
		void _clientError();
		int listenfd,connfd,port;
};
#endif
