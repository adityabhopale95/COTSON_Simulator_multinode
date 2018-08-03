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

// $Id: packet.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef PACKET_H_
#define PACKET_H_

#include <cstring>
#include <stdint.h>

#include "mac.h"
#include "port.h"

struct Packet
{
	enum { T_REQUEST_PORT=0x0700 };

	Packet() : len(0) {}

	void receive(int handle,Port& addr);
	void load(const uint8_t*,int);
	void send(int handle,const Port& addr) const;
	void send_to_slirp() const;

	enum {MAXBUFLEN = 16*1024};
	char buf[MAXBUFLEN];
	int len;

	uint16_t type;
	MacAddress src;
	MacAddress dst;
};

struct PortAnswer : public Packet
{
	PortAnswer(int port); 
};

#endif /*PACKET_H_*/
