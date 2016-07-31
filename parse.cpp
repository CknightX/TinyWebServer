#include "parse.h"
parse::parse(string uri)
	:_uri(uri)
{
	_parseUri();
}
void parse::_parseUri()
{
	if (_uri.find("cgi-bin")==-1)  //Static content	
	{
		_filename="."+uri;
		if (uri=="/")
			_filename+="index.html";
		_cgiargs="";
		int index=uri.find_last_of('.');
		_filetype=uri.substr(index,uri.length()-index);
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
	return (uri.find("cgi-bin")==-1);
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
string parse::filetype()
{
	return _filetype;
}
