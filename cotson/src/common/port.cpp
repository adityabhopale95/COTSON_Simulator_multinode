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

// $Id: port.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "port.h"

#include <string>
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>

using namespace std;

ostream& operator<<(ostream& o,const Port& a)
{
	o << string(::inet_ntoa(a.sin_addr)) 
		<< ":" 
		<< boost::lexical_cast<string>(a.sin_port);
	return o;
}

bool operator==(const Port &a, const Port &b)
{
	return a.sin_port == b.sin_port && 
		a.sin_addr.s_addr == b.sin_addr.s_addr;
}

bool operator!=(const Port &a, const Port &b)
{
	return !(a==b);
}

bool operator<(const Port &a, const Port &b) 
{
	return (a.sin_addr.s_addr < b.sin_addr.s_addr) || 
		(a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port < b.sin_port);
}
