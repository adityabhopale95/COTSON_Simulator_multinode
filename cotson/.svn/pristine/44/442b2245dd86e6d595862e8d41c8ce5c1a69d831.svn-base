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
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "instruction.h"

// We optimize instruction creation by taking instructions off 
// a preallocated pool which wraps around (leaving a space
// for instructions given to timers). 

class Instructions
{
	uint in_, out_, elems_, safe_size_, total_size_;
	Instruction* pul;
	std::set<Instruction*> old_puls;

	void resize();

public:
	Instructions();
	~Instructions();

	uint elems() const { return elems_; }

	void create(uint,uint);

	inline Instruction* alloc()
	{
	    if(!pul)
	    	throw std::runtime_error("no instruction pool set");
	    if(elems_==safe_size_)
		    resize();
	    elems_++;
	    Instruction *p=&pul[in_++];
	    if(in_>= total_size_) 
		    in_=0;
	    return p;
	}

	inline const Instruction* next()
	{
	    const Instruction* p=&pul[out_++];
	    elems_--;
	    if(out_>=total_size_) 
		    out_=0;
	    return p;
	}
	void clear();
};

#endif

