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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h> /* native ::read - GCC 4.7.0 complains without this */
#include "timing_message.h"

#define NUM_RETRIES 5

#define __str(x) #x
#define ___str(x) __str(x)

using namespace std;

// Local utility
namespace TimingMessage {

ssize_t Base::sendto(int fd, const sockaddr_in* to) const
{
	const void* b = data();
	const ssize_t l = len();
	ssize_t n = 0;
	for (int t=0; t<NUM_RETRIES; ++t) {
        n = ::sendto(fd,b,l,0,reinterpret_cast<const sockaddr*>(to),sizeof(sockaddr_in));
        if (n == l)
            return n;
	}
	cout << "Cannot send message (size=" << l << " sent=" << n << ")" << endl;
    return -1;
}
ssize_t Base::recv(int fd)
{
    return ::read(fd,ptr_,sz_);
}

} // namespace

// TimeStamp messages
TimeStamp::TimeStamp(const void* p)
    : Base(&data_,sizeof(Data)) 
{ 
    Base::copy(p); 
}

TimeStamp::TimeStamp(uint16_t t, uint64_t ts, uint32_t id, uint16_t sn)
    : Base(&data_,sizeof(Data)) 
{
    data_.tstamp = ts;
    data_.nodeid = id;
    data_.seqno = sn;
    data_.type = htons(t);
}

TimeStamp::TimeStamp(uint16_t fd)
    : Base(&data_,sizeof(Data)) 
{
    ssize_t nb = ::read(fd,&data_,len());
    if (nb==len()) {
        switch(ntohs(data_.type)) {
            case TimingMessage::TimeStampMsg:
            case TimingMessage::NodeStartMsg:
            case TimingMessage::NodeStopMsg:
            case TimingMessage::TimeQueryMsg:
		    case TimingMessage::CpuidMsg:
		    case TimingMessage::TerminateMsg:
                return;
            default:
                break;
        }
    }
    data_.type = TimingMessage::UnknownMsg;
}

// GlobalTime messages
GlobalTime::GlobalTime()
    : Base(&data_,sizeof(Data)) 
{
    ::memset(&data_,0,len()); 
    data_.type = htons(TimingMessage::GTimeMsg);
}

GlobalTime::GlobalTime(uint16_t fd)
    : Base(&data_,sizeof(Data)) 
{
    ssize_t nb = ::read(fd,&data_,len());
    if (nb == len()) {
        switch(ntohs(data_.type)) {
		    case TimingMessage::GTimeMsg:
		    case TimingMessage::CpuidMsg:
		    case TimingMessage::TerminateMsg:
                return;
            default:
                break;
		}
	}
    data_.type = htons(TimingMessage::UnknownMsg);
}

GlobalTime::GlobalTime(uint64_t t0, uint32_t l, uint16_t sn)
    : Base(&data_,sizeof(Data)) 
{
    data_.gt = t0;
    data_.lat = l;
    data_.seqno = sn;
    data_.type = htons(TimingMessage::GTimeMsg);
}

// DataPort messages
DataPort::DataPort(uint16_t p) 
    : Base(&data_,sizeof(Data)) 
{
    ::memset(&data_,0,12);
    data_.data_port = p;
    data_.type = htons(TimingMessage::PortRequestMsg);
}

