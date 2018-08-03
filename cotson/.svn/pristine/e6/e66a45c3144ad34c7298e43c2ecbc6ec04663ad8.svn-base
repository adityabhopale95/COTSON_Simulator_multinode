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
#include "circular_token_queue.h"
#include "error.h"

bool CircularTokenQueue::next(uint64_t& token)
{
	uint64_t *p = queue->head;
	if(p == queue->tail)
		return false;
	
	token = static_cast<uint64_t>(*p);
	p = (p == queue->last) ? queue->first : p + 1;
	queue->head = p;
	return true;
}

uint CircularTokenQueue::size() const
{
	if(queue->head <= queue->tail)
		return queue->tail-queue->head;

	return (queue->last-queue->head)+(queue->tail-queue->first);
}

void CircularTokenQueue::map(boost::function<void (uint64_t)> mapfunc)
{
	uint64_t *pNext = queue->head;
	uint64_t *tail = queue->tail;
	uint64_t *last = queue->last;
	uint64_t *first = queue->first;
	while (pNext != tail) {
		mapfunc(*pNext);
		pNext = (pNext == last) ? first : pNext + 1;
	}
	queue->head = pNext;
}
