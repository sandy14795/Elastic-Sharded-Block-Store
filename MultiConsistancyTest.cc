#include "ConsistancyTest.h"

using namespace std;

int main()
{
    int code = SingleClientConsistencySameAddr(12,10);
    if(code==0)
        cout<<"Multi Consistancy failed\n";
    else
        cout<<"Multi Consistancy Success"<<endl;
}