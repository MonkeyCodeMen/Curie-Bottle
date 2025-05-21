#pragma once

#include <Arduino.h>


union cfgPar_u {
    uint32_t uint32;
	struct {
        uint8_t L; // least significant byte
        uint8_t H;
        uint8_t HH;
        uint8_t HHH; // most significant byte
    } byte;
};
typedef union cfgPar_u cfgPar_t;


class cfgPar{
	public:
    cfgPar (uint32_t p0=0,uint32_t p1=0,uint32_t p2=0,uint32_t p3=0,String str=""): 
            par0{.uint32=p0},
            par1{.uint32=p1},
            par2{.uint32=p2},
            par3{.uint32=p3},
			str(str) 
			{}
        cfgPar(const cfgPar & src) {*this = src;}		// copy constructor
		~cfgPar() = default;

		virtual void clear(){
			par0.uint32 = 0;
			par1.uint32  = 0;
			par2.uint32  = 0; 
			par3.uint32  = 0; 
			str          = "";
		}

		virtual void operator=(const cfgPar & src){
			par0  = src.par0;
			par1  = src.par1;
			par2  = src.par2;
			par3  = src.par3;
			str	  = src.str;
		}

        uint32_t getPar0(){ return par0.uint32; }
        uint32_t getPar1(){ return par1.uint32; }
        uint32_t getPar2(){ return par2.uint32; }
        uint32_t getPar3(){ return par3.uint32; }
        uint32_t getPar(uint8_t n){ 
            switch (n) {
                case 0: return par0.uint32; break;
                case 1: return par1.uint32; break;
                case 2: return par2.uint32; break;
                case 3: return par3.uint32; break;
            }
            return 0xFFFFFFFF;
        }

        void setPar0(uint32_t value) { par0.uint32 = value; }
        void setPar1(uint32_t value) { par0.uint32 = value; }
        void setPar2(uint32_t value) { par0.uint32 = value; }
        void setPar3(uint32_t value) { par0.uint32 = value; }
        void setPar(uint8_t n,uint32_t value){ 
            switch (n) {
                case 0: par0.uint32=value; break;
                case 1: par1.uint32=value; break;
                case 2: par2.uint32=value; break;
                case 3: par3.uint32=value; break;
            }
        }

		cfgPar_t    par0,par1,par2,par3;
		String	 	str;
};