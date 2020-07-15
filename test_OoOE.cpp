#include"register.hpp"
#include"memory.hpp"
#include"predictor.hpp"

#include"issue.hpp"
#include"reservationstation.hpp"
#include"ALUnit.hpp"
#include"SLUnit.hpp"
#include"commondatabus.hpp"
#include"reorderbuffer.hpp"
#include<iostream>
Register reg;
Memory mem;
Predictor prd;
Issue IS(&mem,&reg,&prd);
ReservationStation res(&reg);
ALUnit ALU;
SLUnit SLU(&mem);
CommonDataBus CDB(&reg);
ReorderBuffer ROB(&mem,&reg,&prd);
void refresh()
{
    res.reset();
    ALU.reset();
    SLU.reset();
    CDB.reset();
    ROB.reset();
}
int main()
{
    // int cnt=0;
    // freopen("out","w",stdout);
    mem.init_read();
    while (!IS.empty()||!ROB.empty())
    {
        //deal with full condition inside run
        if (!ROB.stall()&&!IS.empty()) IS.run(&res,&ROB);
        res.check(&ALU,&SLU);
        ALU.run();
        SLU.run();
        if (!ALU.isLock()) CDB.push(ALU);
        if (!SLU.isLock()) CDB.push(SLU);
        if (!CDB.empty()) CDB.run(&res,&ROB);
        if (!ROB.empty()) ROB.run();
    }
    // std::cout<<cnt<<std::endl;
    int num=prd.tot-ROB.tot();
    printf("%d/%d %.2lf%\n",num,prd.tot,num*100.0/prd.tot);
    std::cout<<reg.output()<<std::endl;
    return 0;
}