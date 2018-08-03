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
#ifndef DISK_TOKEN_PARSER_H
#define DISK_TOKEN_PARSER_H

#include "token_parser.h"
#include "circular_token_queue.h"

class DiskTokenParser : public TokenParser
{
public:
	DiskTokenParser(TokenQueue*);
	~DiskTokenParser();
	void run();

	void setCapacity(boost::function<void (uint64_t)> c) { capacity=c; }
	void setBlocksize(boost::function<void (uint64_t)> b) { blocksize=b; }
	
private:	
	typedef enum { TYPE = 0, BDSIZE, BDNUM, BDNREADY } State;
	State parseState;
	CircularTokenQueue *queue;

	void nextState(uint64_t);
	inline State Type(uint64_t);
	inline State bdSize(uint64_t);
	inline State bdNum(uint64_t);
	inline State bdNotready(uint64_t);

	boost::function<void (uint64_t)> capacity;
	boost::function<void (uint64_t)> blocksize;
};

#endif
