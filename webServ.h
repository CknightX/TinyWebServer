#ifndef _WEBSERV_H
#define _WEBSERV_H
#include<string>
#include "csapp.h"
using std::string;
class webServ
{
	public:
		webServ(int _port);
		void servStart();
		void servClose();
		~webServ();
	private:
		bool _openListenfd();
		void _doit();
		void _clientError(const char *cause,const char *errnum,const char *shortmsg,const char *longmsg);
		void _printRequest(rio_t *rp);
		void _servStatic();
		void _servDynamic();
		int listenfd,connfd,port;
		rio_t rio;
		const char *filename,*cgiargs,*filetype;
		int filesize;
};
#endif
