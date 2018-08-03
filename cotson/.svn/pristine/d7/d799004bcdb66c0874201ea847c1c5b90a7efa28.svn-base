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
#include "abaeterno_config.h"

#include "instructions.h"
#include "logger.h"

using namespace std;

Instructions::Instructions() : 
	in_(0), out_(0), elems_(0), safe_size_(0), total_size_(0), pul(0) {}

Instructions::~Instructions()
{
	clear();
	if(pul) delete[] pul;
}

void Instructions::create(uint s,uint p)
{
	if(pul) delete[] pul;
	in_=0;
	total_size_=s+p;
	safe_size_=s;
	pul=new Instruction[total_size_];
}

void Instructions::resize()
{
	LOG("resizing to ",total_size_*2);
	Instruction* npul=new Instruction[total_size_*2];
	for(uint i=0;i<elems_;i++)
		npul[i].clone(pul[(out_+i)%total_size_]);

	old_puls.insert(pul);	
	pul=npul;
	in_=elems_;
	out_=0;
	safe_size_+=total_size_;
	total_size_+=total_size_;
}

void Instructions::clear()
{
	LOG("clearing");
	for(std::set<Instruction*>::iterator i=old_puls.begin(); i!=old_puls.end();++i)
		delete[] *i;
	old_puls.clear();

	elems_=0;
	in_=0;
	out_=0;
}

