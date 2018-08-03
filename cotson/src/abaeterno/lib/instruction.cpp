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

// $Id: instruction.cpp 95 2010-07-14 17:08:21Z paolofrb $
#include "abaeterno_config.h"
#include "instruction.h"
#include "error.h"
#include "../distorm/distorm.h"

using namespace boost;
using namespace std;

namespace {
static const int PFX_REPZ     = (1 << 0);
static const int PFX_REPNZ    = (1 << 1);
static const int PFX_LOCK     = (1 << 2);
static const int PFX_CS       = (1 << 3);
static const int PFX_SS       = (1 << 4);
static const int PFX_DS       = (1 << 5);
static const int PFX_ES       = (1 << 6);
static const int PFX_FS       = (1 << 7);
static const int PFX_GS       = (1 << 8);
static const int PFX_DATA     = (1 << 9);
static const int PFX_ADDR     = (1 << 10);
static const int PFX_REX      = (1 << 11);
static const int PFX_FWAIT    = (1 << 12);
static const int PFX_count    = 13;
static const uint16_t prefix_map_x86_64[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, PFX_ES, 0, 0, 0, 0, 0, 0, 0, PFX_CS, 0,
  0, 0, 0, 0, 0, 0, PFX_SS, 0, 0, 0, 0, 0, 0, 0, PFX_DS, 0,
  PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX, PFX_REX,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, PFX_FS, PFX_GS, PFX_DATA, PFX_ADDR, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, PFX_FWAIT, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  PFX_LOCK, 0, PFX_REPNZ, PFX_REPZ, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
}

InstType::InstType(const uint8_t *CodeBytes)
{
	/* Only control instructions are decoded! */
	/* The rest are 'F' instructions          */
	
	type = DEFAULT; /* by default */

	/* Decode prefixes */
	
	// @@@ From the text in the AMD architecture manuals, it seems that 
	// branches can not contain prefixes. Explicitly, the manual says that 
	// branches can not have REP prefix, and that near branches can not 
	// have REX prefix. It doesn't specify whether other kind of branches 
	// can have REX or any other prefix type.
	// I have performed some tests and haven't found any branch with a prefix;
	// however, I will keep this prefix decoding code here for safety reasons 
	// (it will not change the behaviour of the decoding algorithm, although
	// it will add a slight overhead)

  	uint32_t prefixes = 0;
  	uint8_t rex = 0;
  	for (;;) 
	{
    	uint8_t b = *CodeBytes;
    	// AYOSE: Assume we execute x86_64 code!
 		uint32_t prefix = prefix_map_x86_64[b];
    	if (!prefix)
			break;
    	if (rex) {
      		// REX is ignored when followed by another prefix:
      		rex = 0;
      		prefixes &= ~PFX_REX;
    	}
    	prefixes |= prefix;
    	if (prefix == (uint32_t) PFX_REX) { rex = b; }
    	CodeBytes++;
  	}
	
	int op = *CodeBytes++;
	int modrm_reg;
	/* Decode rest of bytes */
	switch (op)
	{
	  case 0xff: /* GRP5 */
	  	op = *CodeBytes++;
		modrm_reg = (op >> 3) & 7;
		switch(modrm_reg) 
		{
		  case 2: /* call near */ 	
		  case 3: /* call far  */ 	
			type = CTRL_CALL;
			break;
		  case 4: /* jmp near */	
		  case 5: /* jmp far  */	
			type = CTRL_JUMP;
			break;
		  default:
			break;
		};
		break;
			
	  case 0xc2: /* ret im */		
	  case 0xc3: /* ret */		
	  case 0xca: /* lret im */		
	  case 0xcb: /* lret */		
		type = CTRL_RET;
		break;

	  case 0xcf: /* iret */		
		type = CTRL_IRET;
		break;
			
   	  case 0xe8: /* call im */	
	  case 0x9a: /* lcall im ... shouldn't appear in 64-bit mode! */
		type = CTRL_CALL;
		break;
			
	  case 0xe9: /* jmp */		
	  case 0xea: /* ljmp im ... shouldn't appear in 64-bit mode!*/	
  	  case 0xeb: /* jmp Jb */		
		type = CTRL_JUMP;
		break;
    	
	  case 0x70 ... 0x7f:   /* jcc Jb */	
		type = CTRL_BRANCH;
		break;

	  case 0xcc: /* int3 */	
	  case 0xcd: /* int N */	
	  case 0xce: /* into */	
//	  case 0x62: /* bound */ /* INVALID IN 64-BIT MODE (invalid-opcode exception) */
		type = CTRL_CALL;
		break;
	 
	  case 0xe0: /* loopnz */
	  case 0xe1: /* loopz */
	  case 0xe2: /* loop */
	  case 0xe3: /* jecxz */
		type = CTRL_BRANCH;
		break;
			
	  case 0x180 ... 0x18f: /* jcc Jv */	
		type = CTRL_BRANCH;
		break;
		
	  case 0x105: /* syscall */
	  case 0x134: /* sysenter */
		type = CTRL_CALL;
		break;
		
	  case 0x107: /* sysret */
		type = CTRL_RET;
		break;
	}
}

uint64_t Instruction::unique_id = 0; // static member

ostream& operator<<(ostream& o,const Instruction& insn)
{
	stringstream ss;
	const Opcode* const opc = insn.opcode;
	if(opc) 
	{
		const uint8_t* opbytes = opc->getOpcode();
		for(uint i=0;i<opc->getLength();i++)
			ss << format("%02X ") % static_cast<uint>(opbytes[i]);
	}
	o << format("insn 0x%016X 0x%016X%|50t| (%2d) [%d] %s\n") 
		% insn.pc.virt
		% insn.pc.phys
		% insn.pc.length
		% (int)insn.type
		% ss.str();
	for(Instruction::MemIterator i=insn.loads.begin(); i!=insn.loads.end();i++)
		o << format("%|12t|load%|20t|0x%016X 0x%016X (%d)\n") 
			% i->virt % i->phys % i->length;
	for(Instruction::MemIterator i=insn.stores.begin(); i!=insn.stores.end();i++)
		o << format("%|12t|store%|20t|0x%016X 0x%016X (%d)\n") 
			% i->virt % i->phys % i->length;

	return o;
}

DumpGzip& operator<<(DumpGzip& gz,const Instruction& inst)
{
	ERROR_IF(!inst.pc.virt,"instructions must have pc to be dumped");
	ERROR_IF(!inst.type,"instructions must have type to be dumped");
	ERROR_IF(!inst.opcode,"instructions must have opcode to be dumped");
	ERROR_IF(!inst.pc.length,"instructions must have length to be dumped");
	
	gz << inst.pc << inst.type;
	for(uint i=0;i<inst.pc.length;i++)
			gz << inst.opcode[i];

	gz << static_cast<uint32_t>(inst.loads.size());
	for(Instruction::MemIterator i=inst.loads.begin(); i!=inst.loads.end();i++)
		gz << *i;

	gz << static_cast<uint32_t>(inst.stores.size());
	for(Instruction::MemIterator i=inst.stores.begin(); i!=inst.stores.end();i++)
		gz << *i;

	return gz;
}


void Instruction::disasm(std::ostream& outfile) const	
{
	if (pc.length == 0 || opcode == NULL)
		return;
	
// From distorm:		
// 	The number of the array of instructions the decoder function will use to return the disassembled instructions.
// 	Play with this value for performance...
#define MAX_INSTRUCTIONS (1000)

	// Holds the result of the decoding.
	_DecodeResult res;
	// Decoded instruction information.
	_DecodedInst decodedInstructions[MAX_INSTRUCTIONS];
	// decodedInstructionsCount holds the count of filled instructions' array by the decoder.
	uint32_t decodedInstructionsCount = 0;
	res = distorm_decode(pc.virt, opcode->getOpcode(), pc.length, Decode64Bits, 
				decodedInstructions, MAX_INSTRUCTIONS, &decodedInstructionsCount);
	if (res == DECRES_INPUTERR) 
	{
		// Null buffer? Incorrect dt?
		ERROR("Incorrect parameters for instruction disassembler");
	}

	for (uint32_t i = 0; i < decodedInstructionsCount; i++)
		 outfile << format("%08x %016x (%02d) %|-24| %s %s\n")
			% cr3   // CR3 register
			% (uint64_t)decodedInstructions[i].offset // PC
			% decodedInstructions[i].size   //size in bytes
			% (char*)decodedInstructions[i].instructionHex.p	// Instruction in HEX
			% decodedInstructions[i].mnemonic.p             	// Instruction in assembly
			% decodedInstructions[i].operands.p;    			// Instruction operands

	for(Instruction::MemIterator i=loads.begin(); i!=loads.end();i++)
		outfile << format("%|12t|load%|20t|0x%016x 0x%016x (%d)\n") 
			% i->virt % i->phys % i->length;
	for(Instruction::MemIterator i=stores.begin(); i!=stores.end();i++)
		outfile << format("%|12t|store%|20t|0x%016x 0x%016x (%d)\n") 
			% i->virt % i->phys % i->length;
}

