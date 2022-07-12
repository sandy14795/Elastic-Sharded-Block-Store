#include<iostream>
#include "client_lib.h"
#include "common.cc"

using namespace std;

int main(int argc, char **argv) {
    //HafsClientFactory client("0.0.0.0:8080", "0.0.0.0:8081");
      HafsClientFactory client("server-0.test1.uwmadison744-f21.emulab.net:50052", 
     "server-1.test1.uwmadison744-f21.emulab.net:50053");
 
    std::string testCase;
    if(!getArg(argc, argv, "tc", &testCase, 1)){
        exit(1);
    }
    
    std::cout << "testCase " << testCase << endl;
    string res;

        //1. Test case : primary receives write request and fails 
        // output : not persisted on either on primary & backup
        if(testCase == "primaryFail"){
            client.Write(4096, string(4096, 'a'));  
            //manual diff for now 
        }
        
        if(testCase == "primaryFailAfterBackupTemp") {
            //2. Test case : primary receive write request, backup writes it to temp and send ack to primary
            // and then while writing to primary it fails
            // output : since we have written to backup's temp file, neither p or b contains this request  
            client.Write(8192, string(4096, 'b'));
        }
        
        if(testCase == "commitBlockInconsistency"){        
            // primary fails after writing 
            // (that means client doesn't received ack, but both primary and backup is having data)
            client.Write(12288, string(4096,'c'));
        }

                
        if(testCase == "onlyAckMissing"){        
            //at-most once semantics
            //both primary and backup contains data
            client.Write(16384, string(4096,'c'));
        }

              
        if(testCase == "backupFail"){        
            //messages added in queue
            client.Write(20480, string(4096,'e'));
        }
}