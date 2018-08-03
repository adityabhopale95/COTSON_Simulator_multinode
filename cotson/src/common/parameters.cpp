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

// $Id: parameters.cpp 11 2010-01-08 13:20:58Z paolofrb $

#include "parameters.h"
#include "options.h"

using namespace std;
using namespace boost;

void Parameters::track()
{
	string n=name();
	for(const_iterator i=map_.begin();i!=map_.end();++i)
	{
		if(i->first=="name")
			continue;
		if(i->second.size()==1)
			Option::track(n+"."+i->first,i->second[0]);
		else
		{
			string v=i->second[0];
			vector<string>::const_iterator j=i->second.begin();
			for(++j;j!=i->second.end();++j)
				v+=", "+*j;
			Option::track(n+"."+i->first,v);
		}
	}
}

void Parameters::track(const string& n,const string& v) const
{
	Option::track(n,v);
}

string Parameters::name() const
{
	const_iterator i=find("name");
	if(has(i))
		return i->second[0];
	else
		return "unknown";
}
