#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include "config.h"
#include "log.h"
#include "global.h"
#include "rfcmail.h"
#include "rfc2ftnversion.h"


using namespace std;

int main(int argc, char *argv[])
{

  string sender, recipient, prgname, logstr ;
  CRfcmail rfcmail;
  
  prgname = argv[0];
  sender = argv[1];
  recipient = argv[2];

  
  cfg = new CConfig;
	log = new CLog(cfg->s_Log);
	log->intro(prgname);
	
#ifdef __linux__
	ofstream myfile;
	int fo;
  if (cfg->debug) {  
    char *tmpname = strdup("/tmp/tmpfileXXXXXX");
    fo = mkstemp(tmpname);

		if (fo != -1) 
	    myfile.open(tmpname); // ("/tmp/example.txt");
  }
#endif
	
	
	
	int rc = initApi();
	
  stringstream rcode ;//create a stringstream
  rcode << rc ;
	logstr="initApi returned=";
	logstr += rcode.str();
  log->add(2,logstr);

	if (rc==0) {
    

// don't skip the whitespace while reading
  cin >> noskipws;

// use stream iterators to copy the stream to a string
  istream_iterator<char> it(cin);
  istream_iterator<char> end;
  string results(it, end);
#ifdef __linux__  
  if (cfg->debug) {
    myfile << "Sender->" << sender << ":" <<  endl;
    myfile << "Recipient->" << recipient << ":" <<   endl;
    myfile << "program->" << basename(prgname.c_str())<< ":" << endl;
    myfile << "results->" << results << ":" <<  endl;
  }
#endif  
  rc = rfcmail.parse(sender,recipient,results);
  rcode.str("");
  rcode << rc ;
	logstr="parse mail returned=";
	logstr += rcode.str();
  log->add(2,logstr);
#ifdef __linux__  
	if (cfg->debug) {
		if (fo != -1)
	    myfile << "rcode->" << rcode << ":" <<  endl;
  }
#endif  

  if (rc==0) {
    rc = rfcmail.sendmail();
  rcode.str("");
  rcode << rc ;
	logstr="send mail returned=";
	logstr += rcode.str();
  log->add(2,logstr);
  } else {
  }
  closeApi();
  }
  
	log->add(9, "finishing");
	log->outro();
  delete log;
  delete cfg;
#ifdef __linux__  
  if (cfg->debug) {  
		if (fo != -1)
   		myfile.close();
  }
#endif  
  
}
