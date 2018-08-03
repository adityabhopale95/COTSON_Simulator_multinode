// (C) Copyright 2006-2009 Hewlett-Packard Development Company, L.P.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

// $Id$
#ifndef INTERFACE_GOES_HERE_H
#define INTERFACE_GOES_HERE_H

#include "metric.h"

#include <vector>

class TokenQueue;

namespace Cotson {

const std::set<std::string>& statistics_names();

uint64_t nanos();
void reload_options();
void end_fastforward();

namespace Cpu {

    struct Stats {
        uint64_t ipc_insts;
        uint64_t ipc_cycles;
        difference insts;
        difference idles;
        uint64_t cycles;
        Stats():ipc_insts(0),ipc_cycles(0),insts(),idles(),cycles(0) {}
    };

    uint64_t instructions(uint64_t devid);
    uint64_t idlecount(uint64_t devid);
    uint64_t cycles(uint64_t devid);
    uint64_t statistic(uint64_t devid,const std::string&);
    void set_ipc(uint64_t devid,uint64_t insts,uint64_t cycles);
    const Stats& stats(uint64_t devid);
    void init(uint64_t devid);
    void flush(uint64_t devid);
    void force_flush();
    void set_quantum_s(); // simulation
    void set_quantum_f(); // functional

    namespace Tokens {
        void code_on(uint64_t devid);
        void code_off(uint64_t devid);

        void memory_on(uint64_t devid);
        void memory_off(uint64_t devid);

        void exception_on(uint64_t devid);
        void exception_off(uint64_t devid);

        void heartbeat_on(uint64_t devid);
        void heartbeat_off(uint64_t devid);

        void register_on(uint64_t devid);
        void register_off(uint64_t devid);

    }
}

namespace Disk { 
    namespace Tokens {
        void on(uint64_t devid);
    }
}

namespace Nic { 
    namespace Tokens {
        void on(uint64_t devid);
    }
}

uint64_t c2t(uint64_t);
uint64_t t2c(uint64_t);
void callback(uint64_t);
void exit(); 

namespace Inject {
    void code();
    void memory();
    void exception();
    void heartbeat();
    void token(uint64_t token);
    void token(uint64_t event,uint64_t len);
    void register_token(const char*);

    struct info_instruction {
        uint8_t *opcode;
        int length;
        uint64_t pc;
        bool is_cpuid;
        bool is_fp;
        bool is_cr3_change; 
        std::vector<uint8_t> src_regs, dst_regs, mem_regs;
    };

    struct info_opcode {
        const uint8_t *opcode;
        int length;
        info_opcode(int n,const uint8_t* op):opcode(op),length(n){}
    };

    enum tag_type { PREFETCH=1, CLFLUSH=2, ASM=3 };
    struct info_tag {
        tag_type type;
		union {
			struct {
		        int32_t base_reg;
		        int32_t index_reg;
                int64_t disp;
                int64_t scale;
                int64_t segment;
                int64_t size_mask;
		    } address;
			struct {
				int op;
			    void *xdata;
			} xasm;
	    } info;
    };

    const info_instruction& current_opcode(boost::function<uint8_t*(int)>);
    void tag(uint32_t tag,bool normal);
    void save_tag_info(uint64_t,uint32_t,tag_type);
    uint64_t PC(bool);
    info_opcode translate_info();
}

namespace Memory {
    uint64_t physical_address(uint64_t);
    void read_physical_memory(uint64_t address,uint32_t length,uint8_t*buf);
    void write_physical_memory(uint64_t address,uint32_t length,uint8_t*buf);
    void read_virtual_memory(uint64_t address,uint32_t length,uint8_t*buf);
    void write_virtual_memory(uint64_t address,uint32_t length,uint8_t*buf);
    uint64_t address_from_tag(const Inject::info_tag&);
}

namespace X86 {
    // GR regs (for reading)
    uint64_t IntegerReg(int);
    uint16_t SelectorES();
    uint16_t FlagsES();
    uint32_t LimitES();
    uint64_t BaseES();
    uint16_t SelectorCS();
    uint16_t FlagsCS();
    uint32_t LimitCS();
    uint64_t BaseCS();
    uint16_t SelectorSS();
    uint16_t FlagsSS();
    uint32_t LimitSS();
    uint64_t BaseSS();
    uint16_t SelectorDS();
    uint16_t FlagsDS();
    uint32_t LimitDS();
    uint64_t BaseDS();
    uint16_t SelectorFS();
    uint16_t FlagsFS();
    uint32_t LimitFS();
    uint64_t BaseFS();
    uint16_t SelectorGS();
    uint16_t FlagsGS();
    uint32_t LimitGS();
    uint64_t BaseGS();
    uint16_t SelectorGDT();
    uint16_t FlagsGDT();
    uint32_t LimitGDT();
    uint64_t BaseGDT();
    uint16_t SelectorLDT();
    uint16_t FlagsLDT();
    uint32_t LimitLDT();
    uint64_t BaseLDT();
    uint16_t SelectorIDT()  ;
    uint16_t FlagsIDT();
    uint32_t LimitIDT();
    uint64_t BaseIDT();
    uint16_t SelectorTR();
    uint16_t FlagsTR();
    uint32_t LimitTR();
    uint64_t BaseTR();
    uint64_t IORestartRIP();
    uint64_t IORestartRCX();
    uint64_t IORestartRSI();
    uint64_t IORestartRDI();
    uint64_t IORestartDword();
    uint8_t IORestartFlag();
    uint8_t HLTRestartFlag();
    uint8_t NMIBlocked();
    uint8_t CurrentPL();
    uint64_t EFERRegister();
    uint32_t MachineState();
    uint32_t Revision();
    uint64_t SMMBase();
    uint64_t RegisterCR4();
    uint64_t RegisterCR3();
    uint64_t RegisterCR0();
    uint64_t RegisterDR7();
    uint64_t RegisterDR6();
    uint64_t RegisterEFlags();
    uint64_t RegisterRIP();
    uint64_t R15();
    uint64_t R14();
    uint64_t R13();
    uint64_t R12();
    uint64_t R11();
    uint64_t R10();
    uint64_t R9();
    uint64_t R8();
    uint64_t RDI();
    uint64_t RSI();
    uint64_t RBP();
    uint64_t RSP();
    uint64_t RBX();
    uint64_t RDX();
    uint64_t RCX();
    uint64_t RAX();

    // FP regs
    struct uint128_t { uint64_t hi,lo; uint128_t(uint64_t,uint64_t); };
    uint128_t XmmReg(int); // xmm registers
    uint16_t  fcw();            // fpu control word
    uint16_t  fsw();            // fpu status word
    uint16_t  ftw();            // fpu tag word (8bits)
    uint16_t  fop();            // fpu opcode
    uint32_t  ip();             // fpu instruction pointer offset
    uint16_t  cs();             // fpu instruction pointer selector
    uint32_t  dp();             // fpu data pointer offset
    uint16_t  ds();             // fpu data pointer selector
    uint32_t  mxcsr();          // MXCSR register state
    uint32_t  mxcsrMask();      // changed jray (mask for MXCSR)
    uint64_t  mmx0Mant();
    uint16_t  mmx0Exp();
    uint64_t  mmx1Mant();
    uint16_t  mmx1Exp();
    uint64_t  mmx2Mant();
    uint16_t  mmx2Exp();
    uint64_t  mmx3Mant();
    uint16_t  mmx3Exp();
    uint64_t  mmx4Mant();
    uint16_t  mmx4Exp();
    uint64_t  mmx5Mant();
    uint16_t  mmx5Exp();
    uint64_t  mmx6Mant();
    uint16_t  mmx6Exp();
    uint64_t  mmx7Mant();
    uint16_t  mmx7Exp();

    // Set functions
    void IntegerReg(int,uint64_t);
    void R15(uint64_t);
    void R14(uint64_t);
    void R13(uint64_t);
    void R12(uint64_t);
    void R11(uint64_t);
    void R10(uint64_t);
    void R9(uint64_t);
    void R8(uint64_t);
    void RDI(uint64_t);
    void RSI(uint64_t);
    void RBP(uint64_t);
    void RSP(uint64_t);
    void RBX(uint64_t);
    void RDX(uint64_t);
    void RCX(uint64_t);
    void RAX(uint64_t);

    // Save and restore regs
    size_t RegSize(); // total regsize
    void SaveRegs(uint8_t*);
    void RestoreRegs(const uint8_t*);
    void UpdateRegs(void);

    // Other utilities
    inline bool is_cpuid(const uint8_t* b)
    {
        return b[0]==0x0F && b[1]==0xA2;
    }
    inline bool is_prefetch(const uint8_t* b)
    {
        return b[0]==0x0F && (b[1]==0x18 || b[1]==0x0D);
    }
	inline bool is_cotson_asm(const uint8_t* op)
	{
	    // prefetchnta 0x2dafxxxx(%r1,%r2,1)
		// 0f 18 84 rr xx xx af 2d
		//  0  1  2  3  4  5  6  7
	    return   op[0]==0x0F && op[1]==0x18 && op[2]==0x84 && 
		         op[7]==0x2d && op[6]==0xaf;
	}
	inline uint8_t cotson_asm_reg(const uint8_t* op) { return op[3]; }
	inline uint8_t cotson_asm_opc(const uint8_t* op) { return op[4]; }
	inline uint8_t cotson_asm_imm(const uint8_t* op) { return op[5]; }

    inline bool is_cr3mov(const uint8_t* b, const uint8_t *m)
    {
        return b[0]==0x0F && b[1]==0x22 && (m[0]&0x38)==0x18;
    }
    inline bool is_clflush(const uint8_t* b, const uint8_t *m)
    {
        return b[0]==0x0F && b[1]==0xAE && (m[0]&0x38)==0x38;
    }
    inline bool is_fp(const uint8_t* b)
    {
        switch(b[0]) 
        {
            case 0xd8: case 0xd9: case 0xda: case 0xdb:
            case 0xdc: case 0xdd: case 0xde: case 0xdf: // X87
                return true;
            default:
                return false;
        }
    }
}

const std::set<TokenQueue*>& queues();

}

#endif
