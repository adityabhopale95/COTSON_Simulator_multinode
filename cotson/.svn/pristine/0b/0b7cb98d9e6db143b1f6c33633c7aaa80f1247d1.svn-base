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
#include "abaeterno_config.h"
#include "command_socket.h"
#include "error.h"
#include "callbacks.h"
#include "liboptions.h"
#include "network_timing.h"

using namespace std;
using namespace boost;

namespace {

option  o1("cmdport",                    "", "Command port");
option  o2("cmdfile",                    "", "File where to write command port");

scoped_ptr<CommandSocket> unique_command_socket;

void command_socket_init() {
	if(Option::has("cmdfile")) 
	{
		int s=CommandSocket::find_free_port(20000,30000);
		if(!s) 
			cout << "could not get a free port for command socket" << kill;
		string nf=Option::get<string>("cmdfile");
		ofstream a(nf.c_str());
		a << s << endl;
		a.close();

		unique_command_socket.reset(new CommandSocket(s,100,"commands")); 
	}
	else if(Option::has("cmdport")) 
	{
		int port=Option::get<int>("cmdport");
		unique_command_socket.reset(new CommandSocket(port,100,"commands")); 
	}
}

void command_socket_terminate() {
	unique_command_socket.reset(0);
}

run_at_init      f1(&command_socket_init);
run_at_terminate f2(&command_socket_terminate);

bool socket_free(int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) return false;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	bool valid=(::bind(sockfd,(struct sockaddr *)&addr, sizeof(addr)) != -1);
	close(sockfd);
	return valid;
}

}

int CommandSocket::find_free_port(int start_port,int end_port)
{
	for(int i=start_port;i<=end_port;i++)
		if(socket_free(i))
			return i;
	return 0;
}

	
bool CommandSocket::next(const char* response, char* command, int bufsiz) 
{
	ERROR_IF(!command,"ReturnNextCommand has null command");

	if (!unique_command_socket || !unique_command_socket->valid())
	    return false;

	// First, send back pending response from last command
	unique_command_socket->write(response);

	// Now, get first pending command
	vector<uint64_t> params;
	CommandSocket::Type cmd_type = CommandSocket::UndefCmd;
	string newcmd = unique_command_socket->read();

	// Finally, parse command
	unique_command_socket->parse_command(newcmd,cmd_type,params,command,bufsiz);
	uint64_t val=0;
	if(!params.empty())
		val=params[0];

	if(cmd_type == CommandSocket::NetworkCmd)
		NetworkTiming::Delay(val);

	return true; 
}

#define CMDBUFLEN 16384

CommandSocket::CommandSocket(int p, int backlog, const char *name) : 
    sockfd_(0),clientsockfd_(0),valid_(false),name_(name),
	kb_(10) // 10m stroke delay
{
    if (p > 0) {
	    // Open a TCP port in listen mode for commands
        if ((sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
			fatal("opening socket");

        sockaddr_in serv_addr;
        ::memset(&serv_addr, 0, sizeof(sockaddr_in));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(p);

		// Let the port be reused while in TIME_WAIT, so that when we
		// restart abaeterno we don't need to wait for the port linger
		int32_t yes = 1;
	    if (::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int32_t)) < 0)
	        fatal("setsockopt reuseaddr"); 

		// Bind and listen to the port
        if (::bind(sockfd_,reinterpret_cast<sockaddr*>(&serv_addr),
		           sizeof(sockaddr_in)) < 0)
			fatal("bind to port",p);
        if (::listen(sockfd_,backlog) < 0)
		    fatal("listen to port",p);

	    valid_ = true;
		cout << "Reading " << name << " from port " << p << endl;

    }
}

CommandSocket::~CommandSocket()
{
    if (sockfd_)
	    ::close(sockfd_);
    if (clientsockfd_)
	    ::close(clientsockfd_);
}

string CommandSocket::read()
{
	if (!valid_)
	    return string("");

	if (!kb_.end())
	    return kb_.next();

	sockaddr cli_addr;
	socklen_t clilen = sizeof(sockaddr_in);
	// This blocks until we get a command
	if ((clientsockfd_ = accept(sockfd_,&cli_addr, &clilen)) < 0)
	    fatal("accept");
	char buf[CMDBUFLEN];
	size_t n = ::recv(clientsockfd_,buf,CMDBUFLEN-1,0);
	if (n < 0)
	    fatal("read", n);
	buf[n] = 0;

	if (n > 1 && buf[0] == '+')
	    return kb_.begin(buf+1);
	else
        return string(buf,n);
}

void CommandSocket::write(const char *s)
{
    if (clientsockfd_ && valid_) {
	    if (s && *s) {
			ssize_t len = strlen(s);
	        ssize_t n = ::send(clientsockfd_,s,len,0);
			if (n != len)
			    fatal("write", n);
		}
	    else {
	        ssize_t n = ::send(clientsockfd_,".",1,0);
			if (n != 1)
			    fatal("write", n);
		}
		::close(clientsockfd_);
	    clientsockfd_ = 0;
	}
}

void CommandSocket::write(const uint8_t* buf, ssize_t len)
{
    if (clientsockfd_ && valid_ && buf && len > 0) {
		ssize_t n = ::send(clientsockfd_,buf,len,0);
		if (n != len)
		    fatal("write", n);
	    ::close(clientsockfd_);
	    clientsockfd_ = 0;
	}
}

void CommandSocket::parse_command(
	const string& newcmd,
	Type& type,
	vector<uint64_t>& params,
	char * const command,
	size_t bufsz
)
{
	type = UndefCmd;
	strncpy(command,newcmd.c_str(),bufsz);
	if (command[0] == '#') {
        string commandLine(command+1);
        if (commandLine.length() >= 5) {
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;   
            boost::char_separator<char> sep(",");
            tokenizer tok(commandLine, sep);
            tokenizer::iterator beg=tok.begin(); beg++; // skip first '#'
            switch (command[1]) {
                case 'n':
                case 'N':
					// Network Command
					// Syntax: #n latency,slowdown,target_slowdown
			        type = NetworkCmd;
			        params.push_back(::atoi((*beg).c_str())); beg++; // latency
				    // FIXME: remove slowdowns, obsolete
			        params.push_back(::atoi((*beg).c_str())); beg++; // slowdown
			        params.push_back(::atoi((*beg).c_str()));  // tgt_slowdown
                    break;
                default: 
		            break;
            }
	    }
		strncpy(command,"",bufsz); // wipe out command
    }
}

#include <errno.h>
void CommandSocket::fatal(const std::string& msg, int n)
{
    if (n)
		ERROR(name_, "socket:", msg, " (", n, ") : ", strerror(errno));
	else 
		ERROR(name_, "socket:", msg, ": ", strerror(errno));

// 	cerr << "ERROR: " << name_ << " socket: " << msg;
// 	if (n) 
// 	    cerr << " (" << n << ")";
// 	cerr << ": " << strerror(errno) << flush;
// 	::exit(1);
}


//================= Keyboard commands =======================

string CommandSocket::Keyboard::begin(const char *s)
{
    text = string(s);
    index = text.c_str();
	cout << "Begin Keyboard (" << text << ")" << endl;
	return next();
}

bool CommandSocket::Keyboard::end()
{
    if (!index)
	    return true;
	else if (!*index) {
		index = 0;
	    cout << "End Keyboard" << endl;
        return true;
	}
	return false;
}

string CommandSocket::Keyboard::next()
{
	string out("");
	if (end())
	    return out;
	uint8_t kx = *index++;
	const char *ks = keymap[kx];
	if (ks) {
	    out = string("keyboard.key ");
	    out += string(ks);
	    out += "\n";
	    cout << "    " << out << flush;
        timespec ts = {0,(long int)delay}; // stroke delay
                       /* added explicit cast required by struct timespec */
        ::nanosleep(&ts,0);
	}
	else 
	    cerr << "    Error: no keyboard scancode for char 0x" 
		     << hex << static_cast<uint32_t>(kx) << dec 
			 << endl;
    return out;
}

CommandSocket::Keyboard::Keyboard(uint32_t sd):
    text(),index(0),delay(sd*1000000)
{
    const struct keys_t { char c; const char* s; } keys[] = {
        {'a',"1E 9E"}, {'b',"30 B0"}, {'c',"2E AE"}, {'d',"20 A0"},
        {'e',"12 92"}, {'f',"21 A1"}, {'g',"22 A2"}, {'h',"23 A3"},
        {'i',"17 97"}, {'j',"24 A4"}, {'k',"25 A5"}, {'l',"26 A6"},
        {'m',"32 B2"}, {'n',"31 B1"}, {'o',"18 98"}, {'p',"19 99"},
        {'q',"10 9D"}, {'r',"13 93"}, {'s',"1F 9F"}, {'t',"14 94"},
        {'u',"16 96"}, {'v',"2F AF"}, {'w',"11 91"}, {'x',"2D AD"},
        {'y',"15 95"}, {'z',"2C AC"}, {'1',"02 82"}, {'2',"03 83"},
        {'3',"04 84"}, {'4',"05 85"}, {'5',"06 86"}, {'6',"07 87"},
        {'7',"08 88"}, {'8',"09 89"}, {'9',"0A 8A"}, {'0',"0B 8B"},
        {'`',"29 A9"}, {'-',"0C 8C"}, {'=',"0D 8D"}, {'\\',"2B AB"},
        {'[',"1A 9A"}, {']',"1B 9B"}, {';',"27 A7"}, {'\'',"28 A8"},
        {',',"33 B3"}, {'.',"34 B4"}, {'/',"35 B5"}, {' ',"39 B9"},

        {'A',"2A 1E 9E AA"}, {'B',"2A 30 B0 AA"}, {'C',"2A 2E AE AA"}, {'D',"2A 20 A0 AA"},
        {'E',"2A 12 92 AA"}, {'F',"2A 21 A1 AA"}, {'G',"2A 22 A2 AA"}, {'H',"2A 23 A3 AA"},
        {'I',"2A 17 97 AA"}, {'J',"2A 24 A4 AA"}, {'K',"2A 25 A5 AA"}, {'L',"2A 26 A6 AA"},
        {'M',"2A 32 B2 AA"}, {'N',"2A 31 B1 AA"}, {'O',"2A 18 98 AA"}, {'P',"2A 19 99 AA"},
        {'Q',"2A 10 9D AA"}, {'R',"2A 13 93 AA"}, {'S',"2A 1F 9F AA"}, {'T',"2A 14 94 AA"},
        {'U',"2A 16 96 AA"}, {'V',"2A 2F AF AA"}, {'W',"2A 11 91 AA"}, {'X',"2A 2D AD AA"},
        {'Z',"2A 2C AC AA"}, {'!',"2A 02 82 AA"}, {'@',"2A 03 83 AA"}, {'#',"2A 04 84 AA"},
        {'$',"2A 05 85 AA"}, {'%',"2A 06 86 AA"}, {'^',"2A 07 87 AA"}, {'&',"2A 08 88 AA"},
        {'*',"2A 09 89 AA"}, {'(',"2A 0A 8A AA"}, {')',"2A 0B 8B AA"}, {'~',"2A 29 A9 AA"},
        {'_',"2A 0C 8C AA"}, {'+',"2A 0D 8D AA"}, {'|',"2A 2B AB AA"}, {'{',"2A 1A 9A AA"},
        {'}',"2A 1B 9B AA"}, {':',"2A 27 A7 AA"}, {'"',"2A 28 A8 AA"}, {'<', "2A 33 B3 AA"},
        {'>',"2A 34 B4 AA"}, {'?',"2A 35 B5 AA"},
        {'\r',"1C 9C"}, {'\n',"1C 9C"}, {'\t',"0F 8F"}, {'\b',"0E 8E"},{'',"01 81"}, 
        {0,0}

	    // TODO: deal with missing keyboard translation of special characters
        // {'^M',"1D"}, {'^B',"9D"}, {'ESCm',"38"}, {'ESCb',"B8"}
        // {'F1',"3B BB"}, {'F2',"3C BC"}, {'F3',"3D BD"}, {'F4',"3E BE"}, {'F5',"3F BF"},
        // {'F6',"40 C0"}, {'F7',"41 C1"}, {'F8',"42 C2"}, {'F9',"43 C3"}, {'F10',"44 C4"},
        // {'del',"E0 53 E0 D3"}, {'up', "E0 48 E0 C8"}, {'down',"E0 50 E0 D0"}, 
	    // {'left',"E0 4B E0 CB"}, {'right',"E0 4D E0 CD"},
    };

	// Initialize keyboard scancode data
    ::memset(keymap,0,256);
    const keys_t *pk = keys;
    while (pk->s) {
        keymap[static_cast<uint8_t>(pk->c)] = pk->s;
        ++pk;
    }
}

