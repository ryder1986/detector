#include <iostream>
#include  "../../tool.h"
using namespace std;
#include "test.h"
//int TestRtmp::tmp_loop=0;
int main()
{
#if 0
     TestRtmp tr;
#else
    TestH264Encode t;
    t.run();
#endif
    // H264Send send;
    cout << "Hello World!" << endl;

    PAUSE_HERE
            return 0;
}

