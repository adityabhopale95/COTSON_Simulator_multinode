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

#include "disk_token_parser.h"
#include "tokens.h"
#include "logger.h"
#include "error.h"

using namespace std;
using namespace boost;

DiskTokenParser::DiskTokenParser(TokenQueue* q) : 
	queue(new CircularTokenQueue(q))
{
	q->anything=this;
	parseState = TYPE;
}

DiskTokenParser::~DiskTokenParser() 
{
	if(queue)
		delete queue;
}

void DiskTokenParser::run()
{
	LOG("disk token parser run: ",queue->size());
	queue->map(bind(&DiskTokenParser::nextState,this,_1));
}

void DiskTokenParser::nextState(uint64_t token)
{
	State s = TYPE;
	switch (parseState) {
		case TYPE:      s = Type(token); break;
		case BDSIZE:    s = bdSize(token); break;
		case BDNUM:     s = bdNum(token); break;
		case BDNREADY:  s = bdNotready(token); break;
		default:
			ERROR("Unkown parse state:", parseState);
	}
	parseState = s;
}

inline DiskTokenParser::State DiskTokenParser::Type(uint64_t t)
{
	LOG("Type",hex,t);

	int count = TOKEN_COUNT(t);
	switch(TOKEN_EVENT(t))
	{
		case EVENT_DISK_ID:
			ERROR_IF(count!=1&& count!=3,"type: incorrect EVENT_DISK_ID count");
			if(count == 1)
				return BDNREADY;
			return BDSIZE;

		default:
			ERROR("Unknown token type:", t, "; parseState: ", parseState);
			return TYPE;
	}
}

inline DiskTokenParser::State DiskTokenParser::bdSize(uint64_t token)
{
	LOG("bdSize",hex,token);
	if(blocksize)
		blocksize(token);
	return BDNUM;
}

inline DiskTokenParser::State DiskTokenParser::bdNum(uint64_t token)
{
	LOG("bdNum",hex,token);
	if(capacity)
		capacity(token);
	return TYPE;
}

inline DiskTokenParser::State DiskTokenParser::bdNotready(uint64_t token)
{
	LOG("bdNotready",hex,token);
	if(blocksize)
		blocksize(0);
	if(capacity)
		capacity(0);
	return TYPE;
}
