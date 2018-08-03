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

#include "nic_token_parser.h"
#include "logger.h"
#include "error.h"

NicTokenParser::NicTokenParser(TokenQueue* q) :
	queue(new CircularTokenQueue(q))
{
	q->anything=this;
}

NicTokenParser::~NicTokenParser() 
{
    if(queue)
		delete queue;
}

void NicTokenParser::run()
{
    LOG("nic token parser run: ",queue->size());
    queue->clear();
}
