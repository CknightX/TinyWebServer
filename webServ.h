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
		void _clientError(string _num,string _reason,string _text);
		void _printRequest(rio_t *rp);
		void _servStatic();
		void _servDynamic();
		int listenfd,connfd,port;
		string filename,cgiargs,filetype;
		int filesize;
};
#endif
