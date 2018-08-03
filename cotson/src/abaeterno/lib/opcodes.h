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

// $Id: opcodes.h 105 2010-08-20 15:13:59Z paolofrb $
#ifndef OPCODES_H
#define OPCODES_H

#include <boost/pool/pool_alloc.hpp>
#include <hashmap.h>
#include <vector>

class Opcode
{
public:
    typedef std::vector<uint8_t> regs;

	Opcode(const uint8_t* o,uint32_t l,int t,bool f,
	       const regs& src,
		   const regs& dst,
		   const regs& mem):
        opcode(o),length(l),type(t),fp(f),
		src_regs(src),dst_regs(dst),mem_regs(mem){}

	inline const uint8_t* getOpcode() const { return opcode; }
	inline uint32_t getLength() const { return length; }
	inline int getType() const { return type; }
	inline bool is_fp() const { return fp; }
	inline const regs& getSrcRegs() const { return src_regs; }
	inline const regs& getDstRegs() const { return dst_regs; }
	inline const regs& getMemRegs() const { return mem_regs; }
private:
    const uint8_t* opcode;
	uint32_t length;
	int type; 
	bool fp;
	const regs src_regs, dst_regs, mem_regs;
};

class Opcodes
{
private:
	typedef HashMap<int64_t,Opcode> Map;
	Map data;
	boost::pool<>* pul;

public:
    enum {pool_size=32, hash_size=200000}; // constants

	Opcodes();
	~Opcodes();

	// memory management
	void clear();
	inline uint8_t* malloc(int length)
	{
		int n=length/pool_size+1;
		if (n>1)
	        return static_cast<uint8_t*>(pul->ordered_malloc(n));
	    return static_cast<uint8_t*>(pul->malloc());
	}

	inline const Opcode* find(uint64_t index) const 
	{
		Map::const_iterator i = data.find(index); 
	    return i == data.end() ? 0 : &(i->second);
	}

	inline const Opcode* insert(uint64_t index, const Opcode& op)
	{
		return &(data.insert(std::make_pair(index,op)).first->second);
	}
};

#endif
