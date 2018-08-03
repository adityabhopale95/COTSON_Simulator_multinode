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
#ifndef RAS_H_INCLUDED
#define RAS_H_INCLUDED

#include "error.h"

class rasT
{
	public:
	
	rasT(uint32_t ps) : size(ps), target(size,0), ptr(0), push(0), pop(0)
	{
		ERROR_IF((size&(size-1))!=0,"ras size must be power of 2");
		//target.reset(new uint64_t[size]);
	}

	// Note: RAS underflow is allowed, as in a circular LIFO	
	uint64_t	Pop()
	{
		pop++;
		return target[--ptr % size];
	}

	void Push(uint64_t pc)
	{
		push++;
		target[ptr++ % size] = pc;
	}

	uint32_t	BackupIndex() { return ptr; }
	void		RestoreIndex(uint32_t index) {ptr = index; }
		
	private:
		
	uint64_t Top() { return target[(ptr-1)%size]; }

	const uint32_t size;

	std::vector<uint64_t> target;
	uint32_t ptr; 

	uint64_t push;
	uint64_t pop;
};
#endif
