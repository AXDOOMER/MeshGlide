#include <fstream>

using namespace std;

// Returns -1 if it fails
int filesize(const char* filename)
{
	ifstream is(filename, ios::ate | ios::binary);
	return is.tellg(); 
}

char* loadfile(const char* filename)
{
	int length = filesize(filename);

	if (length < 0)
		return nullptr;

	char* buffer = new char[length];

	// Read data as a block
	ifstream is(filename, ifstream::binary);
//	if (is)
//	{
		is.read(buffer, length);
		is.close();
//	}
//	else
//	{
//		return nullptr;
//	}

	return buffer;
}


// XXX: read whole file to string https://stackoverflow.com/a/2602060


