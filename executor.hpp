#include"RISC-V.h"
#include"instruction.hpp"
#include"register.hpp"
#include"memory.hpp"


#ifndef _EXECUTOR_
#define _EXECUTOR_
class Executor
{
    private:
        Instruction opt;
        // Register *reg;
        // Memory *mem;
        unsigned addr;
        unsigned temp_result,temp_resultpc;
        unsigned sext(unsigned x,int n) //sign-extend
        {
            return (x>>n)&1?x|0xffffffff>>n<<n:x;
        }     
        unsigned setlow0(unsigned x) {return (x|1)^1;}
    public:
        // Execute(Instruction *_opt,Register *_reg,Memory *_mem):reg(_reg),opt(_opt),mem(_mem) {}
        void init(Instruction _opt)
        {
            opt=_opt;
        }
        void run(Register *reg)
        {
            unsigned shamt=opt.rs2,imm=opt.imm;
            unsigned rs1=opt.rs1,rs2=opt.rs2;
            switch (opt.type)
            {
                case LUI:temp_result=imm;break;
                case AUIPC:temp_result=reg->getpc()+imm;break;
                //control instructions begin
                //jump
                case JAL:   //J type
                {
                    temp_result=reg->getpc();
                    temp_resultpc=sext(imm,20)-4;
                    break;
                }
                case JALR:  //I type
                {
                    temp_result=reg->getpc();
                    temp_resultpc=setlow0(reg->getdata(rs1)+sext(imm,11));
                    break;
                }
                //branch    //B type
                case BEQ:temp_resultpc=(sext(imm,12)-4)*(reg->getdata(rs1)==reg->getdata(rs2));break;
                case BNE:temp_resultpc=(sext(imm,12)-4)*(reg->getdata(rs1)!=reg->getdata(rs2));break;
                case BLT:temp_resultpc=(sext(imm,12)-4)*((int)reg->getdata(rs1)<(int)reg->getdata(rs2));break;
                case BGE:temp_resultpc=(sext(imm,12)-4)*((int)reg->getdata(rs1)>=(int)reg->getdata(rs2));break;
                case BLTU:temp_resultpc=(sext(imm,12)-4)*(reg->getdata(rs1)<reg->getdata(rs2));break;
                case BGEU:temp_resultpc=(sext(imm,12)-4)*(reg->getdata(rs1)>=reg->getdata(rs2));break;
                //load instructions begin   //I type
                case LB: 
                case LH: 
                case LW: 
                case LBU: 
                case LHU:
                case SB:    //S type
                case SH:
                case SW:addr=reg->getdata(rs1)+sext(imm,11);break;
                //arithmetic and logic instructions begin
                //I type
                case ADDI:temp_result=reg->getdata(rs1)+sext(imm,11);break;
                case SLTI:temp_result=((int)reg->getdata(rs1)<(int)sext(imm,11));break;
                case SLTIU:temp_result=(reg->getdata(rs1)<sext(imm,11));break;
                case XORI:temp_result=reg->getdata(rs1)^sext(imm,11);break;
                case ORI:temp_result=reg->getdata(rs1)|sext(imm,11);break;
                case ANDI:temp_result=reg->getdata(rs1)&sext(imm,11);break;
                case SLLI:temp_result=reg->getdata(rs1)<<shamt;break;    
                case SRLI:temp_result=reg->getdata(rs1)>>shamt;break;
                case SRAI:temp_result=(reg->getdata(rs1)>>shamt)|(reg->getdata(rs1)>>31<<31);break;
                //the original sign bit is copied into the vacated upper bits   ???
                //B type
                case ADD:temp_result=reg->getdata(rs1)+reg->getdata(rs2);break;
                case SUB:temp_result=reg->getdata(rs1)-reg->getdata(rs2);break;
                case SLL:temp_result=reg->getdata(rs1)<<reg->getdata(rs2);break;
                case SLT:temp_result=((int)reg->getdata(rs1)<(int)reg->getdata(rs2));break;
                case SLTU:temp_result=(reg->getdata(rs1)<reg->getdata(rs2));break;
                case XOR:temp_result=reg->getdata(rs1)^reg->getdata(rs2);break;
                case SRL:temp_result=reg->getdata(rs1)>>reg->getdata(rs2);break;
                case SRA:temp_result=(reg->getdata(rs1)>>reg->getdata(rs2))|(reg->getdata(rs1)>>31<<31);break;
                case OR:temp_result=reg->getdata(rs1)|reg->getdata(rs2);break;
                case AND:temp_result=reg->getdata(rs1)&reg->getdata(rs2);break;
            }
        }
        void memory_access(Memory *mem,Register *reg)
        {
            switch (opt.type)
            {
                //load and store instructions begin   
                //I type
                case LB:temp_result=sext(mem->load(addr,1),7);break;
                case LH:temp_result=sext(mem->load(addr,2),15);break;
                case LW:temp_result=mem->load(addr,4);break;
                case LBU:temp_result=mem->load(addr,1);break;
                case LHU:temp_result=mem->load(addr,2);break;
                //S type
                case SB:mem->store(addr,reg->getdata(opt.rs2),1);break;
                case SH:mem->store(addr,reg->getdata(opt.rs2),2);break;
                case SW:mem->store(addr,reg->getdata(opt.rs2),4);break;
            }
        } 
        void write_back(Register *reg)
        {
            unsigned rd=opt.rd;
            switch (opt.type)
            {
                //control instructions begin
                //jump
                case JAL:   //J type
                {
                    reg->setdata(rd,temp_result);
                    reg->getpc()+=temp_resultpc;
                    break;
                }
                case JALR:  //I type
                {
                    reg->setdata(rd,temp_result);
                    reg->getpc()=temp_resultpc;
                    break;
                }
                //branch    //B type
                case BEQ:;
                case BNE:
                case BLT:
                case BGE:
                case BLTU:
                case BGEU:reg->getpc()+=temp_resultpc;break;  
                //load and store instructions begin   
                //I type
                case LB: 
                case LH: 
                case LW: 
                case LBU: 
                case LHU:
                //arithmetic and logic instructions begin
                //U type
                case LUI:
                case AUIPC:   
                //I type
                case ADDI:
                case SLTI:
                case SLTIU:
                case XORI:
                case ORI:
                case ANDI:
                case SLLI:
                case SRLI:
                case SRAI:
                //the original sign bit is copied into the vacated upper bits   ???
                //B type
                case ADD:
                case SUB:
                case SLL:
                case SLT:
                case SLTU:
                case XOR:
                case SRL:
                case SRA:
                case OR:
                case AND:reg->setdata(rd,temp_result);break;
            }
        }
};
#endif
