#include <stack>
#include <stdexcept>
#include <string>
#include <cstdlib> //abort()
#include <cstdio>  //snprintf()
#include <iostream>
#include <ctime> 
namespace jj17
{
void test_stack(long& value)
{
	cout << "\ntest_stack().......... \n";
     
stack<string> c;  	
char buf[10];
			
clock_t timeStart = clock();								
    for(long i=0; i< value; ++i)
    {
    	try {
    		snprintf(buf, 10, "%d", rand());
        	c.push(string(buf)); // 只有push    			 		
		}
		catch(exception& p) {
			cout << "i=" << i << " " << p.what() << endl;
			abort();
		}
	}
	cout << "milli-seconds : " << (clock()-timeStart) << endl;	
	cout << "stack.size()= " << c.size() << endl;
	cout << "stack.top()= " << c.top() << endl;	
	c.pop();
	cout << "stack.size()= " << c.size() << endl;
	cout << "stack.top()= " << c.top() << endl;
}
}