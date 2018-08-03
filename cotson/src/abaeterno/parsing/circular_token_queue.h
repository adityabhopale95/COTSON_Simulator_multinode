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

// $Id: circular_token_queue.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "token_queue.h"

class CircularTokenQueue : public boost::noncopyable
{
	public:
	TokenQueue *queue;

	public:

	CircularTokenQueue(TokenQueue* q) : queue(q) {}
	virtual ~CircularTokenQueue() {}
	bool operator==(const TokenQueue* q) { return q==queue; }

	void clear() { queue->head=queue->tail; }

	virtual bool next(uint64_t& token);
	virtual uint size() const;
	virtual void map(boost::function<void (uint64_t)>);
};

#endif
