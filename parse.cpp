#include "parse.h"
parse::parse(string uri)
	:uri(_uri)
{
	_parseUri();
}
void parse::_parseUri()
{
	if (uri.find("cgi-bin")==-1)  //Static content	
	{
		filename="."+uri+"index.html";
		cgiargs="";
	}
	else   //Dynamic content
	{
		int index;
		if ((index=uri.find("?"))==-1) //No parameter
		{
			filename="."+uri;
			cgiargs="";
		}
		else //Parameter
		{
			filename="."+uri.substr(0,index+1);
			cgiargs=uri.substr(index,uri.length()-index)
		}
	}
}
bool parse::isStatic()
{
	return (filename=="./index.html");
}
string parse::uri()
{
	return _uri;
}
string parse::filename()
{
	return _filename;
}
string parse::cgiargs()
{
	return _cgiargs;
}
