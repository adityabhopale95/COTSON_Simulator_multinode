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
#ifndef DISK_TIMER_H
#define DISK_TIMER_H

#include "libmetric.h"

class DiskTimer : public metric
{
public: 
	virtual ~DiskTimer() {}
	virtual uint32_t accept(bool,bool,uint64_t,uint64_t)=0;
	virtual void setBlocksize(uint64_t)=0; 
	virtual void setCapacity (uint64_t)=0; 
	uint32_t disk_id;
};
#endif

