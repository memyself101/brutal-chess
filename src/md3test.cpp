#include "md3model.h"

#include <iostream>
using std::string;	using std::cin;	using std::cout;

int main(int argc, char * argv[])
{
	string filename;
	
	MD3Model model(argv[1]);

	model.load();

	model.printHeader();
	model.printTags();
	model.printMeshHeader();
	model.printSkins();
	model.printTriangles();
	model.printTexVecs();
	model.printVertices();
}
