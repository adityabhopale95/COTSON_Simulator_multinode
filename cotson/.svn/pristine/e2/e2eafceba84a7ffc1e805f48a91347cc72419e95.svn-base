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
#include "tagged_metric.h"
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

void TaggedMetrics::add(metric* met,const string& tag_string) 
{
	tokenizer<> tags(tag_string); 

	tokenizer<>::iterator i;
	for(i=tags.begin();i!=tags.end();i++)
	{
		TagMap::iterator j=tag_to_metrics.find(*i);
		if(j==tag_to_metrics.end())
		{
			vector<metric*> nv;
			nv.push_back(met);
			tag_to_metrics[*i]=nv;
		}
		else
		{
			iterator k=find(j->second.begin(),j->second.end(),met);
			if(k==j->second.end())
				j->second.push_back(met);
		}
	}
}

TaggedMetrics::iterator TaggedMetrics::begin(const string&tag) 
{
	TagMap::iterator j=tag_to_metrics.find(tag);
	if(j==tag_to_metrics.end())
	{
		vector<metric*> nv;
		return nv.end();
	}
	else
		return j->second.begin();
}

TaggedMetrics::iterator TaggedMetrics::end(const string& tag) 
{
	TagMap::iterator j=tag_to_metrics.find(tag);
	if(j==tag_to_metrics.end())
	{
		vector<metric*> nv;
		return nv.end();
	}
	else
		return j->second.end();
}
