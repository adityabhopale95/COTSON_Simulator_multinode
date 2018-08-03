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

// $Id: tagged_metric.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef TAGGED_METRIC_H
#define TAGGED_METRIC_H

#include "metric.h"
#include <vector>

class TaggedMetrics
{
	private:
	typedef std::map<std::string,std::vector<metric*> > TagMap;
	TagMap tag_to_metrics;

	public:
	static inline TaggedMetrics& get()
	{
		static TaggedMetrics singleton;
		return singleton;
	}
	void add(metric*,const std::string&);

	typedef std::vector<metric*>::iterator iterator;
	iterator begin(const std::string&);
	iterator end(const std::string&);
};

#endif
