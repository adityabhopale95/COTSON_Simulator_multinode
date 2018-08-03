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
#ifndef CPU_TOKEN_PARSER_H
#define CPU_TOKEN_PARSER_H

#include "token_parser.h"
#include "trace_needs.h"
#include "opcodes.h"
#include "instructions.h"
#include "state_observer.h"
#include "token_queue.h"
#include "cotson.h"

class CpuTokenParser : public TokenParser, public StateObserver
{
public:
	CpuTokenParser(TokenQueue*,std::vector<uint64_t>&,uint64_t d);
private:
	const uint64_t devid;

public:
	void run();
	void inject(InjectState);
	void tag(uint32_t,const Cotson::Inject::info_tag&);
	bool execute(uint32_t);
	void provide(const TraceNeeds*);

protected:
	void doSetState(SimState);
private:
	const TraceNeeds* trace_needs;
	TokenQueue* queue;
	std::vector<uint64_t>& heartbeat;
	Instructions insns;
	Opcodes opcodes;
	uint64_t cur_cr3;
	bool init_cr3;

	// map to store tag->info correspondence
	typedef std::map<uint32_t,Cotson::Inject::info_tag> tag_map;
	tag_map tagged_insts;

    inline void add_tokens(size_t, const uint64_t*);

    void process_pending(Instruction *);
    std::vector<Memory::Access> prefetch_address;
	std::vector<Memory::Access> clflush_address;
};

#endif
