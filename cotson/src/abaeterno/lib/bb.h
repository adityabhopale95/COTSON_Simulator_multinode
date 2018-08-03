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

// $Id: bb.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef BB_H
#define BB_H

class BB
{
	public:
	uint64_t addr;
	uint64_t next;
	std::deque<uint8_t> sizes;
	uint32_t bytes;
	uint8_t type;

	inline void clear()
	{
		addr=0;
		next=0;
		sizes.clear();
		bytes=0;
		type=0;
	}

	public:

	BB() :
		addr(0),
		next(0),
		bytes(0),
		type()
		{}

	inline void reset(uint64_t addr_)
	{
		clear();
		addr=addr_;
	}

	inline uint64_t start() const { return addr; } 
	inline void end(uint64_t next_,uint8_t type_)
	{
		next=next_;
		type=type_;
	} 

	void push_back(uint8_t nsize)
	{
		sizes.push_back(nsize);
		bytes+=nsize;
	}

	inline void advance()
	{
		uint8_t ss=sizes[0];
		addr += ss;
		bytes -= ss;
		sizes.pop_front();
	}

	inline uint size() const
	{
		return sizes.size();
	}

	friend std::ostream& operator<<(std::ostream& os,const BB& bb)
	{
		os << "addr [" << std::hex << bb.addr << "] "
			<< "next [" << bb.next << std::dec << "] "
			<< "lengths (" << bb.size() << "," << bb.bytes
			<< ") type '" << bb.type << "'" << std::dec;
		return os;
	}
};

#endif
