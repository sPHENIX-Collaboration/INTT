# CDB Example #

Small example showing how to use the `CDBTTree` class.

`main.cc` has two functions:
* `make_cdbttree`
	* Creates a `CDBTTree` associated with file `foo.root`
	* Appends several integer values to `foo.root` in a branch called "foo"
	* Writes the number of entries in the branch "foo" in a separate, Single-Entry branch "size"
	* Commits and writes the `CDBTTree`
* `read_cdbttree`
	* Loads the `CDBTTree` associated with file `foo.root`
	* Gets and outputs the entries of branch "foo" in a loop
	* The number of entries is stored "size", determining the iterations of the loop

Notice that you can read and write files locally, and do no need to interface with the Calibrations Database, despite what the class name suggests.
