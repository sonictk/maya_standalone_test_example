/**
 * @brief  	This is a program that reads in a given list of Maya scene files
 * 			and writes out their contents to the terminal.
 */
#include <maya/MFnDependencyNode.h>
#include <maya/MFileIO.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MIOStream.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MLibrary.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <stdio.h>
#include <string.h>

const char *helpText = "usage: [-h/help] readAndWrite fileName1 fileName2 ...\n"
					   "Each specified file will be loaded and the transform nodes "
					   "in the scene will be printed out.\n";


int main(int argc, char *argv[])
{
	// Help flag
	if (argc == 1)
	{
		std::cout << helpText << std::endl;;
		return 0;
	}
	for (int i=1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || (argv[i][0] == '-'))
		{
			std::cout << helpText << std::endl;
			return 0;
		}
	}

	// Else start the Maya standalone session
	std::cout << "Initializing Maya libraries..." << std::endl;

	MStatus stat = MLibrary::initialize(true, argv[0], true);
	if (!stat) {
		stat.perror("Failed to initialize standalone Maya libraries!");
		return 1;
	}

	for (int i=1; i < argc; i++) {
		// Check if the file can be opened
		char *fileName = argv[i];
		FILE *fp = fopen(fileName, "r");
		if (!fp)
		{
			std::cerr << "File does not exist on disk: " << fileName << std::endl;
			continue;
		}
		else
		{
			std::cout << "Loading \"" << fileName << "\" ... " << std::endl;
			fclose(fp);
		}
		MFileIO::newFile(true);

		// Load the file into Maya
		stat = MFileIO::open(fileName);
		if (!stat) {
			stat.perror("Error while opening file!");
			continue;
		}

		// Find all the mesh nodes in the scene and write their names to the console
		MItDag itDag;
		for (; !itDag.isDone(); itDag.next())
		{
			MObject curItem = itDag.currentItem();
			MFnDependencyNode fnNode;
			MFnNumericAttribute fnNumAttr;
			if (curItem.apiType() == MFn::kMesh)
			{
				fnNode.setObject(curItem);
				MGlobal::displayInfo(fnNode.name());
			}
		}
	}

	// Need to exit Maya cleanly and de-allocate all Maya structures.
	// By default, this also terminates the application.
	MLibrary::cleanup();
	return 0;
}
