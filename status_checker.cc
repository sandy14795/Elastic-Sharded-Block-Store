#include<iostream>
#include "client_lib.h"

using namespace std;


int main() {
    HafsClient client1(grpc::CreateChannel("server-0.test1.uwmadison744-f21.emulab.net:50052", grpc::InsecureChannelCredentials()), "0.0.0.0:8090", false);
    HafsClient client2(grpc::CreateChannel("server-1.test1.uwmadison744-f21.emulab.net:50053", grpc::InsecureChannelCredentials()), "0.0.0.0:8091", false);

    while(true) {
        cout << "Primary: " << HeartBeatResponse_Health_Name(client1.getReplicatorHealth()) << endl;
        cout << "Secondary: " << HeartBeatResponse_Health_Name(client2.getReplicatorHealth()) << endl;

        usleep(1000000);
        
    }
    return 0;
}