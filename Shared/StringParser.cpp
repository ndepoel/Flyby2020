#include "../Common.h"
#include "StringParser.h"

#define MAX_TOKEN_LENGTH	256
char *StringParser::Parse( char **dataptr )
{
	static char token[MAX_TOKEN_LENGTH];
	int len = 0;
	char *data = *dataptr;
	char c;
	
	memset( token, 0, sizeof( token ) );

	if ( !data )
	{
		*dataptr = NULL;
		return token;
	}

	while ( true )
	{
		// Skip whitespaces
		while ( ( c = *data ) <= ' ' )
		{
			if ( !c )
			{
				*dataptr = NULL;
				return token;
			}
			data++;
		}

		// Skip // comments
		if ( *data == '/' && data[1] == '/' )
		{
			while ( *data && *data != '\n' && *data != '\r' )
				data++;
		}
		// Skip /* */ comments
		else if ( *data == '/' && data[1] == '*' )
		{
			data += 2;
			while ( *data && !( *data == '*' && data[1] == '/' ) )
				data++;
			
			if ( *data )
				data += 2;
		}
		else
		{
			break;
		}
	}

	// Handle quoted strings
	if ( *data == '\"' )
	{
		data++;
		while ( *data && *data != '\"' )
		{
			if ( len < MAX_TOKEN_LENGTH )
			{
				token[len] = *data;
				len++;
			}
			data++;
		}
		data++;

		*dataptr = data;
		return token;
	}

	// Parse single word
	while ( c = *data )
	{
		if ( c <= ' ' )
			break;
		if ( c == '/' && data[1] == '/' )
			break;
		if ( c == '/' && data[1] == '*' )
			break;

		if ( len < MAX_TOKEN_LENGTH )
		{
			token[len] = c;
			len++;
		}
		data++;
	}

	*dataptr = data;
	return token;
}

int StringParser::ParseInt( char *str )
{
	if ( str )
		return atoi( str );
	else
		return 0;
}

float StringParser::ParseFloat( char *str )
{
	if ( str )
		return (float)atof( str );
	else
		return 0.0f;
}

Vector3D StringParser::ParseVector3D( char *str )
{
	float vec[3] = { 0, 0, 0 };

	for ( int i = 0; i < 3; i++ )
	{	
		char *token = Parse( &str );
		if ( !*token )
			break;
		
		vec[i] = (float)atof( token );
	}

	return Vector3D( vec );
}

