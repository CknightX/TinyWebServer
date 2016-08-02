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
		_filename="."+_uri;
		if (_uri=="/")
			_filename+="index.html";
		_cgiargs="";
		int index=_filename.find_last_of('.');
		_filetype=_filename.substr(index,_filename.length()-index);
	}
	else   //Dynamic content
	{
		int index;
		if ((index=_uri.find("?"))==-1) //No parameter
		{
			_filename="."+_uri;
			_cgiargs="";
		}
		else //Parameter
		{
			_filename="."+_uri.substr(0,index+1);
			_cgiargs=_uri.substr(index,_uri.length()-index);
		}
	}
}
bool parse::isStatic()
{
	return (_uri.find("cgi-bin")==-1);
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
