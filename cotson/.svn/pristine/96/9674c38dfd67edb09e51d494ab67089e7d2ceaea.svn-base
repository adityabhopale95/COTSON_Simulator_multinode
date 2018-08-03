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
#ifndef DISK_H
#define DISK_H

#include "disk_timer.h"
#include "libmetric.h"
#include "states.h"
#include "lua.hpp"

class TokenQueue;
class DiskTokenParser;

class Disk : public metric
{
	public:
	Disk(TokenQueue*,uint64_t,const std::string&);
	~Disk();

	void timer(luabind::object); // lua hook
	uint32_t read(uint64_t block_number,uint64_t block_count,bool dma);
	uint32_t write(uint64_t block_number,uint64_t block_count,bool dma);
	uint32_t end();

	uint64_t id() const { return devid; }
	const std::string& name() const { return devname; }

	protected:
	const uint64_t devid;
	const std::string devname;

	private:
	const uint64_t devnum;
	const uint64_t devclass;
	boost::scoped_ptr<DiskTokenParser> parser;
	boost::scoped_ptr<DiskTimer> timing;
};

#endif
