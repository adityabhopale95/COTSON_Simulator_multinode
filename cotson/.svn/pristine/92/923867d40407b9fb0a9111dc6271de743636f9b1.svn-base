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
#include "aggregator.h"
#include "error.h"

Aggregator::Aggregator() : met(0),first(true) {}
Aggregator::~Aggregator() {}

void Aggregator::init(metric* m) 
{
	if(met) 
		throw std::runtime_error("metric already set for this aggregator");
	met=m;
}

void Aggregator::beat()
{
	if(first)
	{
		first=false;
		do_first_beat();
	}
	else
		do_beat();	

}

void Aggregator::last_beat()
{
	do_end();
}
