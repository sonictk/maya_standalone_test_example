/**
 * @brief  	This is a program that reads in a given list of Maya scene files
 * 			and writes out their contents to the terminal.
 *
 */

#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MFileIO.h>
#include <maya/MLibrary.h>
#include <maya/MIOStream.h>
#include <string.h>

const char* helpText = "usage: [-h/help] readAndWrite fileName1 fileName2 ...\n\
       Each specified file will be loaded and the transform nodes in the scene \
       will be printed out.\n";


int main(int argc, char **argv)
{
	MStatus stat;

	argc--, argv++;

	if (argc == 0) {
		std::cout << helpText << std::endl;;
		return(1);
	}

	for (; argc && argv[0][0] == '-'; argc--, argv++) {
		if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "-help")) {
			cerr << helpText;
			return(1);
		}

		if (argv[0][0] == '-') {
			// Unknown flag
			cerr << helpText;
			return(1);
		}
	}

	stat = MLibrary::initialize (argv[0]);
	if (!stat) {
		stat.perror("MLibrary::initialize");
		return 1;
	}

	for (; argc; argc--, argv++) {
		MString	fileName(argv[0]);
		MString fileType;

		MFileIO::newFile(true);

		cerr << "Loading \"" << fileName << "\" ... ";
		// Load the file into Maya
		stat = MFileIO::open(fileName);
		if ( !stat ) {
			stat.perror(fileName.asChar());
			continue;
		}
		cerr << " done.\n";

		// Get the file type
		fileType = MFileIO::fileType();

		// Don't overwrite the existing file
		MString newFile;

		// Find the extension if one exists
		int loc = fileName.rindex('.');
		if (loc == -1) {
			newFile = fileName + ".updated";
		} else {
			newFile = fileName.substring(0, loc-1);
			newFile += ".updated";
			newFile += fileName.substring(loc, fileName.length()-1);
		}

		stat = MFileIO::saveAs(newFile, fileType.asChar());
		cerr << "    ";
		if (stat) {
			cerr << "resaved as "
			     << MFileIO::currentFile()
				 << endl;
		} else {
			stat.perror(newFile.asChar());
		}
	}

	MLibrary::cleanup();
	return 0;
}
