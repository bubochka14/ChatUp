#include <gtest/gtest.h>

int globalArgc; char** globalArgv;
int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	globalArgc = argc, globalArgv = argv;
	return RUN_ALL_TESTS();
}