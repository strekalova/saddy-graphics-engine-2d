#include "path.h"
#include <sadstring.h>

sad::String path::directory(const sad::String & path)
{
	if (path.length() == 0)
		return sad::String();
	sad::String escaped = path;
	// Extract path when on windows
	char delimiter[2]="/";
	if (escaped.getOccurence("\\")!=-1) 
	{
		delimiter[0]='\\';
	}

	if (escaped[escaped.length()-1] == delimiter[0])
		escaped.removeLastOccurence(delimiter);
	sad::StringList pathparts = escaped.split(delimiter[0]);
	pathparts.removeAt(pathparts.count()-1);
	if (pathparts.count() == 0) 
		return sad::String();
    sad::String result = "";
    if (path[0] == delimiter[0])
        result += delimiter;
    result += pathparts[0];
	for (unsigned int i=1;i<pathparts.count();i++) {
		result << delimiter << pathparts[i];
	}
	return result;
}

sad::String path::concat(const sad::String & parent,const sad::String & path)
{
	if (parent.length() ==0){
		return path;
	}
	sad::String escaped = parent;
	// Handle windows path
	if (escaped.getOccurence("\\")!=-1) 
	{
		if (escaped[escaped.length()-1] == '\\')
			escaped.removeLastOccurence("\\");
		sad::String escpath = path;
		if (escpath.getOccurence("/")!=-1)
			escpath.replaceAllOccurences("/","\\");
		if (escpath[0] == '\\')
			escpath.remove(0);
		return escaped + "\\" + escpath;
	}

	if (escaped[escaped.length()-1] == '/')
		escaped.removeLastOccurence("/");
	sad::String escpath = path;
	if (escpath.length() == 0)
		return sad::String();
	if (escpath[0] == '/')
		escpath.remove(0);
	return escaped + "/" + escpath;
}