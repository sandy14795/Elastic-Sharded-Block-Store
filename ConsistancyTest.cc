#include "ConsistancyTest.h"

using namespace std;

int main() {
    
    int TC = 2;
    
    int code; 
    ofstream myfile;
    for (int i=0;i<1;i++)
    {
        int k=0;
        code = SingleClientConsistencyDiffAddr(10);
        if(code)
            k++;
        code = SingleClientConsistencySameAddr(12,10);
        if(code)
            k++;
        if(k!=TC)
            cout<<"Cosistancy failed\n";
        else
            cout<<"Success"<<endl;       
        
    }
    /*myfile.open("ConsistancyResults.csv");
    string header = "Min, Max, Mean, Median, StdDev \n";
    string result1 = metric1.get_metrics();
    string result2 = metric2.get_metrics();
    myfile<< result1 + "\n";
    myfile<< result2 + "\n";  
    myfile.close();*/
    /*for(int i=1;i<=1;i++)
    {
        cout<<"No of Client = "<<i*4<<endl;
        int code = MultiClientConsistencySameAddr(i*4);
        if(code==0)
            cout<<"MultiThread Consistancy failed\n";
        else
            cout<<"MultiThread Consistancy Success"<<endl;
    }*/
    return 0;
}