#ifndef CONSISTANCY_TEST_H
#define CONSISTANCY_TEST_H

#include<iostream>
#include<vector>
#include "client_lib.h"
#include "timer.h"
#include "metric.h"

using namespace std;

/*
Single Client 
All writes differnt address
*/
Metrics metric1,metric2;
int SingleClientConsistencyDiffAddr(int NumWrites)
{
    HafsClientFactory client("128.105.144.230:8093", "128.105.144.230:8094");
    //HafsClient client1(grpc::CreateChannel("155.98.36.86:8093", grpc::InsecureChannelCredentials()), "155.98.36.86:8093", false);
    //HafsClient client2(grpc::CreateChannel("155.98.36.88:8094", grpc::InsecureChannelCredentials()), "155.98.36.88:8094", false);
    string res;
    // // client.Read(0, &res);
    // // cout << "Data read: " << res << endl;

    vector<int> UsedAddr;
    Timer2 time;
    for(int i = 0; i < NumWrites; i++) {
        int CharId = rand()%26;
        string data = string(4096, 'a'+ CharId);
        time.start();
        client.Write(i*4096, data);
        time.stop();
        //cout<<time.get_time_in_nanoseconds()<<endl;
        //metric1.add(time.get_time_in_nanoseconds());
        UsedAddr.push_back(i*4096);
        /*client.Read(i*4096, &res);

        if(res!=data)
        {
            cout<<"Read Write Failed"<<endl;
            return 0;
        }*/
    }
    //cout<<time.get_time_in_nanoseconds()<<endl;
    
    for(int i=0;i<UsedAddr.size();i++)
    {
        if(client.CheckConsistancy(UsedAddr[i])!=true)
        {         
            return 0;
        }
    }

    usleep(10*1000000);
    return 1;
}


/*
Single Client 
All address Same
*/
int SingleClientConsistencySameAddr(int k,int NumWrites)
{
    HafsClientFactory client("128.105.144.230:8093", "128.105.144.230:8094");
    //HafsClient client1(grpc::CreateChannel("155.98.36.86:8093", grpc::InsecureChannelCredentials()), "155.98.36.86:8093", false);
    //HafsClient client2(grpc::CreateChannel("155.98.36.88:8094", grpc::InsecureChannelCredentials()), "155.98.36.88:8094", false);
    // // client.Read(0, &res);
    // // cout << "Data read: " << res << endl;
    int Addr = k*4096;
    Timer2 time;
    
    for(int i = 0; i < NumWrites; i++) {
        int CharId = rand()%26;
        string data = string(4096, 'a'+ CharId);
        time.start();
        client.Write(Addr, data);
        time.stop();
        //cout<<time.get_time_in_nanoseconds()<<endl;
        metric2.add(time.get_time_in_nanoseconds());
        if(client.CheckConsistancy(Addr)!=true)
            return 0;
        /*client.Read(i*4096, &res);

        if(res!=data)
        {
            cout<<"Read Write Failed"<<endl;
            return 0;
        }*/
    }
    
    //cout<<time.get_time_in_nanoseconds()<<endl;
    
    if(client.CheckConsistancy(Addr)!=true)
        return 0;
    usleep(10*1000000);
    return 1;
}


/*
Multiple Client 
Same Addr

int rtCodeCommon=1;
void ConsistencySameAddr(int k)
{
    int block=4096;
    int numWrites = 10000;
    HafsClientFactory client("155.98.36.86:8093", "155.98.36.88:8094");
    string res;
    for(int i=0;i<numWrites;i++)
    {
        int Addr = 4096;   //(k*block)+block;
        int CharId = rand()%26;
        string data = string(4096, 'a'+ CharId);
        client.Write(Addr, data);
        client.Read(Addr, &res);

        if(res!=data)
        {
            cout<<"Read Write Failed"<<endl;
            rtCodeCommon=0;
            return;
        }
        k++;
    }
}
int MultiClientConsistencySameAddr(int clientCnt)
{
    vector<thread> threads;
    for(int i=0;i<clientCnt;i++)
    {
        threads.push_back(thread(ConsistencySameAddr,0));
    }
    for(int i=0;i<clientCnt;i++)
    {
        threads[i].join();
    }
    if(rtCodeCommon)
        return 1;
    else
        return 0;

}*/

#endif
