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

// $Id: heartbeat.h 320 2012-05-21 10:58:36Z paolofrb $
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "metric.h"
#include "aggregator.h"

#include <vector>
#include <boost/shared_ptr.hpp>

class HeartBeat
{
public:
	HeartBeat(): mets() {}
	virtual ~HeartBeat() {} 

	static void add(metric&); 
	static void add_aggregable(metric&);
	static void beat();
	static void last_beat();

protected:
	std::vector<metric*> mets;
	std::vector< boost::shared_ptr<Aggregator> > aggr;

	virtual void do_beat() = 0;
	virtual void do_last_beat() = 0;

	static void init();
	static void new_heartbeat(const std::string&);
	static std::vector< boost::shared_ptr<HeartBeat> > hearts;
};
#endif
