#include<iostream>
#include<thread>
#include<chrono>
#include "client_lib.h"

using namespace std;

void writeToSamePlace(char a) {
    HafsClientFactory client("0.0.0.0:8090", "0.0.0.0:8091");

    client.Write(24, string(4096, a));
}


int main() {
    string res;

    thread t1(writeToSamePlace, 'a');
    thread t2(writeToSamePlace, 'b');
    thread t3(writeToSamePlace, 'c');
    thread t4(writeToSamePlace, 'd');
    thread t5(writeToSamePlace, 'e');
    thread t6(writeToSamePlace, 'f');
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    HafsClient client1(grpc::CreateChannel("0.0.0.0:8090", grpc::InsecureChannelCredentials()), "0.0.0.0:8090", false);
    HafsClient client2(grpc::CreateChannel("0.0.0.0:8091", grpc::InsecureChannelCredentials()), "0.0.0.0:8091", false);

    client1.Read(0,  &res);
    cout << "client1: " << res << endl << endl;

    client2.Read(0, &res);
    cout << "client2: " << res << endl << endl;

    client1.Read(24,  &res);
    cout << "client1: " << res << endl << endl;

    client2.Read(24, &res);
    cout << "client2: " << res << endl << endl;

    client1.Read(4096,  &res);
    cout << "client1: " << res << endl << endl;

    client2.Read(4096, &res);
    cout << "client2: " << res << endl << endl;

    return 0;
}