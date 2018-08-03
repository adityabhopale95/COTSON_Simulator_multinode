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
#ifndef COMMAND_SOCKET_H
#define COMMAND_SOCKET_H

class CommandSocket {
public:
	enum Type { UndefCmd, NetworkCmd };

	static bool next(const char* response, char* command, int bufsiz);
	static int find_free_port(int start_port,int end_port);

	CommandSocket(int,int,const char*);
	~CommandSocket();
	std::string read();
	void write(const char *);
	void write(const uint8_t*,ssize_t);
	bool valid() const { return valid_; }
   void parse_command(const std::string&,Type&,std::vector<uint64_t>&,char* const,size_t);
private:
	class Keyboard {
	public:
		Keyboard(uint32_t);
		std::string begin(const char *);
	    std::string next();
	    bool end();
    private:
	    std::string text;
	    const char* index;
        const char* keymap[256];
		const uint64_t delay;
    };
	
	void fatal(const std::string&,int=0);

	int sockfd_;
	int clientsockfd_;
	bool valid_;
	const char* const name_;
	Keyboard kb_;
};

#endif
