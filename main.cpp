#include <iostream>
#include "test.h"
int main()
{
    try{
    Test t("res/config.json");
    PAUSE_HERE_FOREVER
    }catch(exception e){
        prt(info,"err");
    }

    return 0;
}
