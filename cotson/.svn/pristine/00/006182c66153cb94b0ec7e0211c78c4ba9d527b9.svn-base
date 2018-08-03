#include "timing_message.h"
#include "mac.h"

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

#include <vector>

using namespace std;

int N_ctrl=0;
int N_data=0;

struct EtherPacket : public TimingMessage::Base
{
    EtherPacket(const ether_header* eh, const void* p, size_t n) : 
	    Base(new uint8_t(sizeof(ether_header)+n), sizeof(ether_header)+n)
        {
            copy(eh,0,sizeof(ether_header));
            copy(p,sizeof(ether_header),n);
        }
    ~EtherPacket() { delete[] reinterpret_cast<uint8_t*>(Base::ptr_); }
};


class FakeNode
{
public:
    FakeNode(const string&, int, const string&, int, int);
    void send_timestamp(TimingMessage::Type, uint64_t);
    void send_message(MacAddress, const string&);
	bool ok() const { return !error; }
	uint8_t mac(uint i) const { return xmac[i]; }
private:
    int get_data_port();

	int id; // node id
    int seqno; // sequence number
    int med_sock; // socket for mediator communication
    int sync_sock; // synchronization socket
    sockaddr_in med_ctl_addr; // mediator ctrl address
    sockaddr_in med_data_addr; // mediator data address
	bool error; // was there an error?
	MacAddress xmac; // mac address
};

FakeNode::FakeNode(
    const string& med_host, 
	int med_port,
	const string& mcast_ip, 
	int sync_port,
    int nodeid) : id(nodeid),seqno(0),error(false)
{
	// open UDP control socket to the mediator
	med_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (med_sock == -1) {
		cerr << "Cannot create mediator socket" << endl;
		error=true;
		return;
	}
	const hostent *mh = ::gethostbyname(med_host.c_str());
	if (!mh) {
		cerr << "Cannot resolve mediator host " << med_host << endl;
	    ::close(med_sock);
		error=true;
		return;
	}
	::memset(&med_ctl_addr,0,sizeof(sockaddr_in));
	::memcpy((char *)&med_ctl_addr.sin_addr.s_addr,mh->h_addr_list[0],mh->h_length);
	med_ctl_addr.sin_family = mh->h_addrtype;
	med_ctl_addr.sin_port = htons(med_port);
    
    // Open UDP sync socket for timestamps from mediator
	sync_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (sync_sock == -1) {
		cerr << "Cannot create sync socket" << endl;
		error=true;
		return;
	}
	// Let port be reused by others
	int yes = 1;
	if (::setsockopt(sync_sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
		cerr << "Cannot reuse port addresses" << endl;
		::close(sync_sock);
		sync_sock = -1;
		error=true;
		return;
	}
	// Create and bind to multicast address for timestamps
	sockaddr_in mcaddr;
	::memset(&mcaddr,0,sizeof(sockaddr_in));
	mcaddr.sin_family=AF_INET;
	mcaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	mcaddr.sin_port=htons(sync_port);
	if (::bind(sync_sock,(sockaddr *) &mcaddr,sizeof(mcaddr)) < 0) {
		cerr << "Cannot bind to the multicast address" << endl;
		::close(sync_sock);
		sync_sock = -1;
		error=true;
		return;
	}
	// Setup multicast IP membership
	ip_mreq mreq;
	mreq.imr_multiaddr.s_addr=inet_addr(mcast_ip.c_str());
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (::setsockopt(sync_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP, &mreq,sizeof(mreq)) < 0) {
		cerr << "Cannot add multicast membership" << endl;
		::close(sync_sock);
		sync_sock = -1;
		error=true;
		return;
	}
	// Get data port
	memcpy((char*)&med_data_addr,&med_ctl_addr,sizeof(sockaddr_in));
	med_data_addr.sin_port = htons(get_data_port());

	// assign a MAC address
	xmac=MacAddress(0xfacd0100,nodeid);
}

int FakeNode::get_data_port()
{
    // send port request
    DataPort msg(med_sock);
    msg.sendto(med_sock,&med_ctl_addr); 
	N_ctrl++;
    // wait for port reply
    ssize_t nb = msg.recv(med_sock);
    if (nb!=msg.len()) { error=true; return 0; }
    cout << "Node " << id << ": got data port " << msg.port() << endl;
    return msg.port();
}

void FakeNode::send_timestamp(TimingMessage::Type t, uint64_t ts)
{
	N_ctrl += 2;
    TimeStamp(t,ts,id,++seqno).sendto(med_sock,&med_ctl_addr);
}

void FakeNode::send_message(MacAddress m, const string& data)
{
	ether_header eh;
    eh.ether_type = ETHERTYPE_IP;
	for(uint i=0;i<6;++i) {
	    eh.ether_shost[i]=mac(i);
	    eh.ether_dhost[i]=m[i];
	}
	EtherPacket msg(&eh,data.c_str(),data.size()+1);
    ssize_t l = msg.sendto(med_sock,&med_data_addr); 
	if (l != msg.len()) 
	    cerr << "Error sending packet: sent " << msg.len() << " returned " << l << endl;
	else
        N_data++;
}

int main(int argc, char **argv)
{
	if (argc < 8) {
	    cerr <<  "Usage: " 
		    << argv[0] 
			<< " med_host med_port sync_mcast_ip sync_port n_nodes n_timestamps mode" 
			<< endl;
		return 1;
	}
	string med_host(argv[1]);
	int med_port = atoi(argv[2]);
	string sync_mcast_ip(argv[3]);
	int sync_port = atoi(argv[4]);
	uint NN = atoi(argv[5]);
	uint NTS = atoi(argv[6]);
	int mode = atoi(argv[7]);

	vector<FakeNode> nodes;
	MacAddress destmac(0x01020304,0); // random address
	MacAddress node1(0xfacd0100,1);

	if (mode==0) { // timestamp before data
	    // create 10 nodes
	    for (uint i=0; i < NN; ++i) {
		    FakeNode n(med_host,med_port, sync_mcast_ip,sync_port, i+1);
		    if (!n.ok()) return 1;
	        nodes.push_back(n);
        }
	    // send initial timestamp
	    for (uint i=0; i < NN; ++i)
	        nodes[i].send_timestamp(TimingMessage::TimeStampMsg,10);

	    // send data messages
	    for (uint i=0; i < NN; ++i) {
	        nodes[i].send_message(destmac,"hello1");
	        nodes[i].send_message(node1,"hhh1");
	    }
	}
	if (mode==1) { // timestamp+data mixed
	    for (uint i=0; i < NN; ++i) {
		    FakeNode n(med_host,med_port, sync_mcast_ip,sync_port, i+1);
		    if (!n.ok()) return 1;
	        n.send_timestamp(TimingMessage::TimeStampMsg,10);
	        n.send_message(destmac,"hello2");
	        n.send_message(node1,"hhh2");
	        nodes.push_back(n);
	    }
	}
	if (mode==2) { // data before timestamps
	    // create 10 nodes
	    for (uint i=0; i < NN; ++i) {
		    FakeNode n(med_host,med_port, sync_mcast_ip,sync_port, i+1);
		    if (!n.ok()) return 1;
	        nodes.push_back(n);
        }
	    // send data messages
	    for (uint i=0; i < NN; ++i) {
	        nodes[i].send_message(destmac,"hello0");
	        nodes[i].send_message(node1,"hhh0");
		}
	    // send initial timestamp
	    for (uint i=0; i < NN; ++i)
	        nodes[i].send_timestamp(TimingMessage::TimeStampMsg,10);
	}
	if (mode==3) { // data+timestamp mixed (BAD)
	    for (uint i=0; i < NN; ++i) {
		    FakeNode n(med_host,med_port, sync_mcast_ip,sync_port, i+1);
		    if (!n.ok()) return 1;
	        n.send_message(destmac,"hello3");
	        n.send_message(node1,"hhh3");
	        n.send_timestamp(TimingMessage::TimeStampMsg,10);
	        nodes.push_back(n);
	    }
	}
	// send other timestamps
    uint64_t t = 100;
	for (uint i = 0; i < NTS; ++i) {
	    cout << "Send timestamp "<< t << endl;
	    for (uint i=0; i < nodes.size(); ++i)
	        nodes[i].send_timestamp(TimingMessage::TimeStampMsg,t+10*i);
	    t += 100;
	    sleep(0.2);
	}
	// send terminate to quit
	sleep(1.0);
	nodes[0].send_timestamp(TimingMessage::TerminateMsg,t);
	cout << "CTRL " << N_ctrl << endl;
	cout << "DATA " << N_data << endl;
    return 0;
}

