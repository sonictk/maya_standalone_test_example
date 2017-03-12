/**
 * @brief	This is the definition of the entry point for the unit test runner
 *			for this application.
 */
#define CATCH_CONFIG_RUNNER

#include <maya/MLibrary.h>
#include <maya/MFnMesh.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include "../include/catch.hpp"


/**
 * This is a function that calculates the number of points in the given ``mesh``
 * object.
 *
 * @param mesh	The Maya object that represents a mesh.
 *
 * @return		The number of points in the mesh.
 */
int calculateNumPts(MObject &mesh)
{
	MFnMesh fnMesh(mesh);
	return fnMesh.numVertices();
}


/**
 * The test cases are defined here.
 */
TEST_CASE("Test mesh point function", "[maya_standalone_unit_test]")
{
	MFileIO::newFile(true);
	MGlobal::executeCommand("polyCube;");
	MSelectionList selList;
	selList.add("pCubeShape1");
	MObject node;
	selList.getDependNode(0, node);

	REQUIRE(calculateNumPts(node) == 8);
}


/**
 * The entry point of the test runner application. This runs the unit tests for
 * the application. This overrides the main() that Catch normally uses in order
 * to initialize a standalone Maya session instead before running the tests.
 *
 * @param argc	Number of args that the program was run with.
 *
 * @param argv	Pointer to the first element of an array of pointers to
 * 			null-terminated multibyte strings that represent the arguments
 * 			passed to the program.
 *
 * @return		The exit code.
 */
int main(int argc, char *argv[])
{
	// Global setup
	MStatus stat = MLibrary::initialize(true, argv[0], true);
	if (!stat) {
		stat.perror("Failed to initialize standalone Maya libraries!");
		return 1;
	}

	// Run the test suite using Catch
	int result = Catch::Session().run(argc, argv);

	// global clean-up
	// Cleanup of Maya libraries and structs
	MLibrary::cleanup(0, false);
	return (result < 0xff ? result : 0xff);
}
