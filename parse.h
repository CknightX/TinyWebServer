#ifndef _PARSE_H
#define _PARSE_H
#include<string>
using std::string;
class parse
{
	public:
		parse(string uri);
		bool isStatic();
		string filename();
		string cgiargs();
		string filetype();
	private:
		void _parseUri();
		string _uri;
		string _filename;
		string _cgiargs;
		string _filetype;
};
#endif
