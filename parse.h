#ifndef _PARSE_H
#define _PARSE_H
#include<string>
using std::string;
class parse
{
	public:
		parse(string _uri);
		bool isStatic();
		string url();
		string filename();
		string cgiargs();
	private:
		void _parseUri();
		string _uri;
		string _filename;
		string _cgiargs;
};
#endif
