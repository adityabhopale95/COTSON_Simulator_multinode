#include "custom_asm.h"

// This example models 3 custom instructions to manipulate a hidden table 
// Table is allocated in the simulator. 
// Only 3 commands are supported: init, read, write
//
void fill_table(int n)
{
    // Fill the table with semirandom numbers
	uint64_t x = 123456;
	for(int i=0;i<n;++i) {
	    custom_write(i,x);
		x += 43;
    }
}

uint64_t add_table(int n)
{
    // Add up all elements of the table
	uint64_t x = 0;
	for(int i=0;i<n;++i)
	   x += custom_read(i);
    return x;
}

uint64_t check(int n)
{
	uint64_t a = 0;
	uint64_t x = 123456;
	for(int i=0;i<n;++i) a+=x,x+=43; 
	return a;
}

#include <iostream>
using namespace std;

int main(int argc, const char *argv[])
{
	int n = 10;
	cout << "Init table" << endl;
    custom_init(n);

	cout << "Fill table" << endl;
	fill_table(n);

	cout << "Add table ";
	uint64_t x = add_table(n);
	cout << x << endl;
	if (x == check(n)) 
	    cout << "PASS" << endl; 
    else  
	    cout << "FAIL" << endl;
	return 0;
}
