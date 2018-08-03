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
#ifndef Nic_H
#define Nic_H

#include "nic_timer.h"
#include "libmetric.h"
#include "states.h"
#include "lua.hpp"

class TokenQueue;
class NicTokenParser;

class Nic : public metric
{
public:
	Nic(TokenQueue*,uint64_t,const std::string&);
	~Nic();

	void timer(luabind::object); // lua hook
	uint32_t read(uint64_t length,void* data,bool dma);
	uint32_t write(uint64_t length,void* data,bool dma);
	uint32_t end();

	uint64_t id() const { return devid; }
	const std::string& name() const { return devname; }

private:
	const uint64_t devid;
	const std::string devname;

	boost::scoped_ptr<NicTokenParser> parser;
	boost::scoped_ptr<NicTimer> timing;
};

#endif

