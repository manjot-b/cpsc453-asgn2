#include <glad/glad.h>
#include <iostream>
#include "ImageDisplayer.h"
using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << "<image filename>" << endl;
		return -1;
	}
	ImageDisplayer imgDisplayer(argv[1]);
	imgDisplayer.run();	
}

