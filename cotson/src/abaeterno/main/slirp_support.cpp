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

// $Id: slirp_support.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"
#include "slirp_support.h"
#include "packet.h"
#include "logger.h"
#include "liboptions.h"
#include "error.h"
#include "callbacks.h"

#include "../slirp/libslirp.h"

namespace {
option  o1("slirp",              "false", "should we start a internal slirp mediator");
option  o2("slirp_control_port", "8196",  "control port for internal slirp mediator");
option  o3("slirp_data_port",    "8197",  "data port for internal slirp mediator");
optionR o4("slirp_redir_tcp",             "tcp redirections for internal slirp");
optionR o5("slirp_redir_udp",             "udp redirections for internal slirp");
option  o6("slirp_network",   "192.168.10.0",  "slirp network"); 

void slirp_init() {
	if(Option::get<bool>("slirp"))
		slirp_enable();
}

run_at_init i1(&slirp_init);
}

using namespace std;

namespace { 
	int control_handle=-1;
	int data_handle=-1;
	Port latest_port;
}

extern "C" {
	int slirp_can_output(void) { return 1; }
	void slirp_output(const uint8_t *pkt, int pkt_len)
	{
		Packet p;
		p.load(pkt,pkt_len);
		LOG("(SLIRP)", "src", p.src, "dst", p.dst, "type", p.type, "size", p.len);
		LOG("(SLIRP) to",latest_port);
		p.send(data_handle,latest_port);
	}
}

int open_and_bind(uint16_t port)
{
	Port addr;
	memset(&addr, 0, sizeof(addr));

	int fd = ::socket (AF_INET, SOCK_DGRAM, 0);
	if(fd==-1) 
		throw runtime_error("Unable to obtain network");

	addr.sin_family = AF_INET;
	addr.sin_port = htons (port);
	addr.sin_addr.s_addr = htonl (INADDR_ANY);

	if(bind(fd,reinterpret_cast<struct sockaddr*>(&addr),sizeof(addr)) != 0) 
		throw runtime_error("Unable to bind socket");
	return fd;
}

void at_end()
{
	if(control_handle!=-1)
		::close(control_handle);
	if(data_handle!=-1)
		::close(data_handle);
}

#define __STRINGIFY(a) #a
#define STRINGIFY(a) __STRINGIFY(a)

void slirp_controller()
{
	int control_port=Option::get<int>("slirp_control_port");
	int data_port=Option::get<int>("slirp_data_port");
	std::string network=Option::get<string>("slirp_network");
	std::string localip =   network.substr(0,network.find_last_of('.')+1)
	                      + STRINGIFY(SLIRP_START_ADDR);

	atexit(at_end);
	::strcpy(slirp_hostname,"slirp.cotson.com");
	slirp_init(false,network.c_str());
	try 
	{
		if(Option::has("slirp_redir_tcp"))
		{
			Parameters p=Option::from_named_option("slirp_redir_tcp",set<string>(),"slirp_redir_tcp");
			p.set("name", "slirp_redir_tcp");
			p.track();

			struct in_addr guest_addr;
			if(!inet_aton(localip.c_str(), &guest_addr))
				throw runtime_error("error constructing guest address");
			for(Parameters::const_iterator i=p.begin();i!=p.end();++i)
			{
				if(i->first !="name")
				{
					int dst_port=cast<int>(i->first,"slirp_redir_tcp."+i->first);
					int src_port=p.get<int>(i->first);
					if(slirp_redir(false,src_port,guest_addr,dst_port))
						throw runtime_error("error redirecting tcp port "+i->first);
				}
			}
		}

		if(Option::has("slirp_redir_udp"))
		{
			Parameters p=Option::from_named_option("slirp_redir_udp",set<string>(),"slirp_redir_udp");
			p.set("name", "slirp_redir_udp");
			p.track();

			struct in_addr guest_addr;
			if(!inet_aton(localip.c_str(), &guest_addr))
				throw runtime_error("error constructing guest address");
			for(Parameters::const_iterator i=p.begin();i!=p.end();++i)
			{
				if(i->first !="name")
				{
					int dst_port=cast<int>(i->first,"slirp_redir_udp."+i->first);
					int src_port=p.get<int>(i->first);
					if(slirp_redir(true,src_port,guest_addr,dst_port))
						throw runtime_error("error redirecting udp port "+i->first);
				}
			}
		}

		control_handle=open_and_bind(control_port);
		data_handle=open_and_bind(data_port);

		if(control_handle==-1 || data_handle==-1)
			throw runtime_error("could not open control or data for slirp");

		for(;;) 
		{
			fd_set reads,writes,exceptions;
			FD_ZERO(&reads);
			FD_ZERO(&writes);
			FD_ZERO(&exceptions);
			
			FD_SET(control_handle, &reads);
			FD_SET(data_handle, &reads);
			int max_fd = max(control_handle,data_handle);

			slirp_select_fill(&max_fd, &reads, &writes, &exceptions);

			if (select(max_fd+1,&reads,&writes,&exceptions,0) == -1)
				throw runtime_error("error in select");

			if(FD_ISSET(control_handle, &reads))
			{
				Port addr; 
				Packet p; 
				p.receive(control_handle,addr);

				if(p.type==Packet::T_REQUEST_PORT)
				{
					PortAnswer pa(data_port);
					pa.send(control_handle,addr);
					LOG("(CTRL) port", addr);
				}
				else
					LOG("(CTRL) unknown command",p.type);
			}

			if(FD_ISSET(data_handle, &reads))
			{
				Port addr; 
				Packet p; 
				p.receive(data_handle,addr);

				latest_port=addr;

				LOG("(DATA) addr", addr, "src", p.src, "dst", p.dst, "type", p.type, "size", p.len);
				LOG("(DATA) slirp'ed");
				p.send_to_slirp();
			}

			slirp_select_poll(&reads, &writes, &exceptions);
		}
	}
	catch (runtime_error &e) 
	{
		ERROR(e.what());
	}
}

void slirp_enable()
{
	boost::thread run(slirp_controller);
}

