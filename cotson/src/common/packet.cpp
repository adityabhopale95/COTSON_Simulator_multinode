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

// $Id: packet.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "packet.h"
#include "../slirp/libslirp.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>

using namespace std;

namespace {
	struct EtherHeader
	{
		 uint8_t  dst[6];
		 uint8_t  src[6];
		 uint16_t type;
	};
}

void Packet::receive(int handle,Port& addr)
{
	socklen_t addr_len = sizeof(addr);
	len=::recvfrom(handle, buf, MAXBUFLEN-1 , 0, reinterpret_cast<sockaddr*>(&addr), &addr_len);
	if(len==-1)
		throw runtime_error("Unable to recvfrom");
	buf[len]='\0';

	const EtherHeader *eh=reinterpret_cast<const EtherHeader*>(buf);
	type=ntohs(eh->type);
	src=MacAddress(eh->src);
	dst=MacAddress(eh->dst);
}

void Packet::load(const uint8_t*buf_,int len_)
{
	if(len_>MAXBUFLEN-1)
		throw runtime_error("loaded packet too big");
	memcpy(buf,buf_,len_);
	len=len_;
	buf[len]='\0';

	const EtherHeader *eh=reinterpret_cast<const EtherHeader*>(buf);
	type=ntohs(eh->type);
	src=MacAddress(eh->src);
	dst=MacAddress(eh->dst);
}

void Packet::send(int handle,const Port& addr) const
{
	int s=::sendto(handle,buf,len,0,
		reinterpret_cast<const sockaddr*>(&addr),
		sizeof(addr));
	if(s!=len)
		throw runtime_error("error sending packet");
}

void Packet::send_to_slirp() const
{
	slirp_input(reinterpret_cast<const uint8_t*>(buf),len);
}

PortAnswer::PortAnswer(int port) 
{
	struct { uint8_t  e[12]; uint16_t t; uint16_t p; } a;
	::memset(a.e,0,12); 
	a.t=htons(T_REQUEST_PORT);
	a.p=port;
	memcpy(buf,&a,sizeof(a));
	len=sizeof(a);
}
