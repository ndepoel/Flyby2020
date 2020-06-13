#ifndef __STRINGPARSER_H
#define __STRINGPARSER_H

#include "../Math3D/Math3D.h"

class StringParser
{
public:
	static char		*Parse( char **data );

	static int ParseInt( char *str );
	static float ParseFloat( char *str );
	static Vector3D	ParseVector3D( char *str );
};

#endif

