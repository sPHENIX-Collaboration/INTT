#include <filesystem>
#include <iostream>
#include <string>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

std::string filename = "foo.root";

int make_cdb();
int check_cdb();

int main() {
	make_cdb();
	check_cdb();

	return 0;
}

int make_cdb() {
	CDBTTree cdb_ttree(filename);

	int size = 0;
	while(size < 10) {
		int foo = size * size + 1;
		cdb_ttree.SetIntValue(size, "foo", foo);
		++size;
	}
	cdb_ttree.SetSingleIntValue("size", size);

	cdb_ttree.Commit();
	cdb_ttree.CommitSingle();
	cdb_ttree.WriteCDBTTree();

	return 0;
}

int check_cdb() {
	CDBTTree cdb_ttree(filename);
	cdb_ttree.LoadCalibrations();

	int size = 0;
	std::cout << "\nfoo values:\n";
	while(size < cdb_ttree.GetSingleIntValue("size")) {
		std::cout << "\tindex: " << size << " foo: " << cdb_ttree.GetIntValue(size, "foo") << "\n";
		++size;
	}
	std::cout << "\n";

	std::cout << "Trying to get a branch that does not exist:\n\t";
	cdb_ttree.GetIntValue(0, "branch_that_does_not_exist");	// Will output a warning and returns std::numeric_limits<int>::min()
	std::cout << "\n";

	std::cout << "Trying to get an entry that's out of range:\n\t";
	cdb_ttree.GetIntValue(++size, "foo");			// Will output a warning and returns std::numeric_limits<int>::min()
	std::cout << "\n";

	// You should manutally verify local files exist
	CDBTTree dne_ttree("file_that_probably_does_not_exist.root");
	// dne_ttree.LoadCalibrations();			// Segfaults

	// Need to implement your own guard clauses, e.g.,
	// if(!std::filesystem::exists("file_that_probably_does_not_exist.root")) {
	// 	return 1;
	// }

	return 0;
}
