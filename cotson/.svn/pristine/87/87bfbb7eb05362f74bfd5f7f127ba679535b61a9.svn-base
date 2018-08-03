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
// #define _DEBUG_THIS_

#include "abaeterno_config.h"

#include "cpu_token_parser.h"
#include "tokens.h"
#include "liboptions.h"
#include "logger.h"
#include "error.h"
#include "cotson.h"
#include "interleaver.h"
#include "cpuid_call.h"

using namespace std;
using namespace boost;

CpuTokenParser::CpuTokenParser(TokenQueue* q,vector<uint64_t>& h,uint64_t d) :
    devid(d),
    trace_needs(0),
    queue(q),
    heartbeat(h),
    cur_cr3(0),
	init_cr3(false)
{
    queue->anything=this;
    Cotson::Cpu::Tokens::code_off(devid);
}

void CpuTokenParser::provide(const TraceNeeds *tn)
{
    trace_needs=tn;
    Interleaver::get().config(devid,insns,tn);
}

void CpuTokenParser::process_pending(Instruction *inst)
{
	size_t np = prefetch_address.size();
    if (np)
    {
		if (!inst)  
		{
            inst=insns.alloc();
            Instruction::init(inst,0,0,8,cur_cr3,0,InstType::PREFETCH); 
		}
		else
            inst->Type(InstType::PREFETCH);
		for (size_t i=0;i<np;++i)
            inst->LoadsPush(prefetch_address[i]);
		prefetch_address.clear();
    }

	size_t nf = clflush_address.size();
	if (nf)
    {
		if (!inst)  
		{
            inst=insns.alloc();
            Instruction::init(inst,0,0,8,cur_cr3,0,InstType::CLFLUSH);
		}
		else 
            inst->Type(InstType::CLFLUSH);
		for (size_t i=0;i<nf;++i)
            inst->LoadsPush(clflush_address[i]);
		clflush_address.clear();
    }
}

void CpuTokenParser::run()
{
    uint64_t *tail = queue->tail;
    LOG(hex,static_cast<void*>(this),"empty queue: tokens=", dec, tail-queue->head);

    const StateNeeds& sn=trace_needs->st[sim_state()];
	const bool memory_only  = sn.needs_memory && !sn.needs_code;

    Instruction* current_inst=0;
    int count = 0;
    for(const uint64_t *p=queue->head;p!=tail;p+=count)
    {
        uint64_t p0 = p[0];
        uint64_t p1 = p[1];
        count = TOKEN_COUNT(p0);

        switch(TOKEN_EVENT(p0))
        {
            case EVENT_INSTRUCTION:
            {
                if (count!=4 && count!=5) ERROR("incorrect instruction event token count");
                ERROR_IF(TOKEN_OPTIONS(p0)!=INSTRUCTIONS_ON, "type: INSTRUCTIONS_ON");
                uint64_t p2 = p[2];
                const Opcode *op = opcodes.find(p2);
                if (op)
                {
					uint len = op->getLength();
                    ERROR_IF(p[count-1]!=len, "opcode length mismatch");
                    current_inst=insns.alloc();
                    Instruction::init(
					    current_inst,
						p2,p1,len, // p2 virtual, p1 physical
						cur_cr3,
						op,op->getType()); 
                    LOG("EVENT_INSTRUCTION ", Memory::Access(p2,p1,len), 
					    hex, static_cast<void*>(this), dec);
					process_pending(current_inst);
                }
                else
                {
                    WARNING("no opcode at PC ", p2, "(",  static_cast<void*>(this), ")");
                    current_inst = 0;
                }
            }
            break;

            case EVENT_MEMORY:
            {
                ERROR_IF(TOKEN_OPTIONS(p0)!=MEMORY_ON, "type: MEMORY_ON");
                ERROR_IF(!(MEMORY_ISREAD(p1)||MEMORY_ISWRITE(p1)),
                         "read or write should be set");
                if (MEMORY_HASPHYS(p1)) // skip if there's no physical address
                {
                    if (memory_only && !current_inst) {
						// Process pending stuff and make a fake inst
					    process_pending(0);
                        current_inst=insns.alloc();
                        Instruction::init(current_inst,0,0,8,cur_cr3,0,InstType::MEMFAKE); 
                    }
					if (current_inst) 
					{
                        if (count!=5) ERROR("incorrect memory event token count");
					    Memory::Access addr(p[4],p[3],p[2]);
                        if (MEMORY_ISREAD(p1)) {
                            LOG("EVENT_MEMORY read ",addr);
                            current_inst->LoadsPush(addr);
					    }
                        if (MEMORY_ISWRITE(p1)) {
                            LOG("EVENT_MEMORY write",addr);
                            current_inst->StoresPush(addr);
				        }
					}
                }
            }
            break;

            case EVENT_EXCEPTION:
            {
                if(count!=2) ERROR("an exception should have 2 tokens->");
                ERROR_IF(TOKEN_OPTIONS(p0)!=EXCEPTION_ON,"type: EXCEPTION_ON");

                uint64_t type=TOKEN_COUNT(p1)<<2;
                heartbeat[type-1]++;
            }
            break;

            case EVENT_HEARTBEAT:
            {
                if(count!=3) ERROR("a heartbeat should have 3 tokens");
                ERROR_IF(TOKEN_OPTIONS(p0)!=HEARTBEAT_ON, "type: heaHEARTBEAT_ON");
                heartbeat[p1-1]++;
            }
            break;
            
            case EVENT_CPUID:
            {
                if(count!=9) ERROR("a cpuid should have 9 tokens");
                if(IS_COTSON_CPUID(p[2])) // RAX
                {
                    uint64_t RAX=p[2];
                    uint64_t RDI=p[4];
                    uint64_t RSI=p[6];
                    uint64_t RBX=p[8];

                    LOG("EVENT_CPUID: devid",devid,"RAX",hex,RAX);
                    LOG("\tRDI",hex,RDI,"RSI",RSI,"RBX",RBX);
                    // Fake instruction for the interleaver
                    Instruction* cpuid_ins = insns.alloc();
                    Instruction::init(cpuid_ins,0,0,8,cur_cr3,0,InstType::CPUID);
					CpuidCall::add_cpuid_xdata(cpuid_ins,RAX,RDI,RSI,RBX);
				    // See lib/cpuid_call.h
                }
            }
            break;

            case EVENT_ASM:
			{
				// We actually generate a CPUID type instruction (see lib/cpuid_call.h)
                if(count!=3) ERROR("a ASM should have 3 tokens");
                LOG("EVENT_ASM: devid",devid,"op",p[1],"xdata",hex,p[2]);
                Instruction* asm_ins = insns.alloc();
                Instruction::init(asm_ins,0,0,8,cur_cr3,0,InstType::CPUID);
				CpuidCall::add_asm_xdata(asm_ins,p[1],p[2]);
			}
            break;

            case EVENT_PREFETCH: // p1:virt.addr, p2:phys.addr 
            {
                // Note: the prefetch address is injected *before* CODEFETCH
                // by TaggedExecution, so we save it and add it to the next inst
                if(count!=3) ERROR("a prefetch should have 3 tokens");
                prefetch_address.push_back(Memory::Access(p1,p[2],8));
            }
            break;

            case EVENT_CLFLUSH: // p1:phys.addr
            {
                // Note: the clflush address is injected *before* CODEFETCH
                // by TaggedExecution, so we save it and add it to the next inst
                if(count!=3) ERROR("a clflush should have 3 tokens");
                clflush_address.push_back(Memory::Access(p1,p[2],8));
            }
            break;

            case EVENT_CR3:
            {
                if(count!=3) ERROR("A cr3 event should have 3 tokens");
                LOG("CR3 event",hex,cur_cr3);
                cur_cr3=p[2];
            }
            break;
            
            default:
                ERROR("Event '",TOKEN_EVENT(p0),"' not defined!");
        }
    }
    queue->head = tail;
}

void CpuTokenParser::inject(InjectState state)
{
	if (!init_cr3)
	{
        LOG(hex,static_cast<void*>(this),"CR3 init");
        Cotson::Inject::token(EVENT_CR3,3); // length: 3
        Cotson::Inject::register_token("CR3");
		init_cr3=true;
	}
    switch(state)
    {
        case CODE:
        {
            const Cotson::Inject::info_instruction& ii=
                Cotson::Inject::current_opcode(
				    bind(&Opcodes::malloc,&opcodes,_1));
            opcodes.insert(ii.pc, 
			    Opcode(ii.opcode,ii.length,
				    InstType(ii.opcode), ii.is_fp,
				    ii.src_regs,ii.dst_regs,ii.mem_regs));
            LOG(hex,static_cast<void*>(this),"new pc",hex,ii.pc);

            if(ii.is_cr3_change)
            {
                LOG(hex,static_cast<void*>(this),"CR3 change detected (static code)");
                Cotson::Inject::token(EVENT_CR3,3); // length: 3
                Cotson::Inject::register_token("CR3");
            }

            if(ii.is_cpuid)
            {
                LOG(hex,static_cast<void*>(this),"CPUID detected (static code)");
                Cotson::Inject::token(EVENT_CPUID,9); // length: 9
                Cotson::Inject::register_token("RAX");
                Cotson::Inject::register_token("RDI");
                Cotson::Inject::register_token("RSI");
                Cotson::Inject::register_token("RBX");
            }
            Cotson::Inject::code();
            break;
        }
        case MEMORY:
            Cotson::Inject::memory();
            break;
        case EXCEPT:
            Cotson::Inject::exception();
            break;
        case HEARTBEAT:
            Cotson::Inject::heartbeat();
            break;
        default:
            ERROR("Inject state '",state,"' not valid");
    }
}

// Tagged CODE injection and execution

inline void CpuTokenParser::add_tokens(size_t n, const uint64_t* tokens)
{
    if (queue->tail + n >= queue->last)
    {
        LOG("add_tokens: empty queue");
        run(); // no space: empty the queue
        queue->head = queue->tail = queue->first;
    }
    (void)memcpy(queue->tail,tokens,n*sizeof(uint64_t));
    queue->tail += n;
}

void CpuTokenParser::tag(uint32_t tag, const Cotson::Inject::info_tag& ti)
{
    tagged_insts[tag]=ti;
}

bool CpuTokenParser::execute(uint32_t tag)
{
    ERROR_IF(tag==0,"Attempting to execute with tag==0");
    tag_map::const_iterator i = tagged_insts.find(tag);
    if (i == tagged_insts.end())
        return false;

    const Cotson::Inject::info_tag& ti = i->second;
    switch (ti.type)
    {
        case Cotson::Inject::PREFETCH:
        {
            uint64_t va = Cotson::Memory::address_from_tag(ti);
            uint64_t pa = Cotson::Memory::physical_address(va);
            uint64_t tokens[3] = { MAKE_EVENT(EVENT_PREFETCH,3), va, pa };
            add_tokens(3,tokens);
        }
        break;

        case Cotson::Inject::CLFLUSH:
        {
            uint64_t va = Cotson::Memory::address_from_tag(ti);
            uint64_t pa = Cotson::Memory::physical_address(va);
            uint64_t tokens[3] = { MAKE_EVENT(EVENT_CLFLUSH,3), va, pa };
            add_tokens(3,tokens);
        }
        break;

        case Cotson::Inject::ASM:
        {
            uint64_t op = ti.info.xasm.op;
            uint64_t xdata = (uint64_t)ti.info.xasm.xdata;
			LOG("asm tokens: op",op,"xdata",hex,xdata);
            uint64_t tokens[3] = { MAKE_EVENT(EVENT_ASM,3), op, xdata };
            add_tokens(3,tokens);
        }
        break;

        default:
            ERROR("Unknown tag type",ti.type);
    }
    return true;
}

void CpuTokenParser::doSetState(SimState new_state)
{
    if(new_state==sim_state()) 
        return;

    LOG("doSetState", devid, new_state);
    
    bool flush_now=false;
    const StateNeeds& sn_old=trace_needs->st[sim_state()];
    const StateNeeds& sn_new=trace_needs->st[new_state];

    if(sn_old.needs_code != sn_new.needs_code)
    {
        flush_now=true;
        sn_new.needs_code ? 
            Cotson::Cpu::Tokens::code_on(devid) : 
            Cotson::Cpu::Tokens::code_off(devid) ;
    }

    if(sn_old.needs_memory != sn_new.needs_memory)
    {
        flush_now=true;
        sn_new.needs_memory ? 
            Cotson::Cpu::Tokens::memory_on(devid) : 
            Cotson::Cpu::Tokens::memory_off(devid) ;
    }

    if(sn_old.needs_exception != sn_new.needs_exception)
    {
        flush_now=true;
        sn_new.needs_exception ? 
            Cotson::Cpu::Tokens::exception_on(devid) : 
            Cotson::Cpu::Tokens::exception_off(devid) ;
    }

    if(sn_old.needs_heartbeat != sn_new.needs_heartbeat)
    {
        flush_now=true;
        sn_new.needs_heartbeat ? 
            Cotson::Cpu::Tokens::heartbeat_on(devid) : 
            Cotson::Cpu::Tokens::heartbeat_off(devid) ;
    }

    if(sn_old.needs_register != sn_new.needs_register)
    {
        flush_now=true;
        sn_new.needs_register ? 
            Cotson::Cpu::Tokens::register_on(devid) : 
            Cotson::Cpu::Tokens::register_off(devid) ;
    }

    if(flush_now)
    {
        insns.clear();
        tagged_insts.clear();
		prefetch_address.clear();
		clflush_address.clear();
        opcodes.clear();
    }
}
