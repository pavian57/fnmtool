#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include "rfcmail.h"

using namespace std;

int main(int argc, char *argv[])
{

string sender, recipient, ssize, subject ;
int		size;

sender = argv[1];
recipient = argv[2];

stringstream ss(ssize);
ss >> size;

char *tmpname = strdup("/tmp/tmpfileXXXXXX");
mkstemp(tmpname);
        
// don't skip the whitespace while reading
  cin >> noskipws;

// use stream iterators to copy the stream to a string
  istream_iterator<char> it(cin);
  istream_iterator<char> end;
  string results(it, end);

  int pos = results.find("Subject:");
  string partresults = results.substr(pos);
  pos = partresults.find("\n");
  subject.assign(partresults,9,pos-9);
  

  cout << "From     =" << sender << endl;
  cout << "Recipient=" << recipient << endl;
  cout << "Subject  =" << subject << endl;
  cout << "Size     =" << size << endl;
 
  

 ofstream myfile;
 myfile.open(tmpname); // ("/tmp/example.txt");
 myfile <<  results;
 
 myfile << "sender=" << sender << ":" << endl; 
 myfile << "recipient=" << recipient  << ":" << endl; 
 myfile << "size=" << size  << ":" << endl;
 
 
 myfile.close();
 
}