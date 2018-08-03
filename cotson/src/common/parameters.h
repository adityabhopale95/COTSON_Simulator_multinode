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

// $Id: parameters.h 727 2016-01-05 18:57:13Z robgiorgi $
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector>
#include "cast.h"

class Parameters
{
public:
	typedef std::map<std::string,std::vector<std::string> > Map;
	typedef Map::iterator iterator;
	typedef Map::const_iterator const_iterator;

private:
	Map map_;

public:
	Parameters() {}

	bool has(const std::string& i) const { return find(i)!=map_.end(); }
	void erase(const std::string& i) { map_.erase(i); }

	template<typename T> T get(const std::string&) const;
	template<typename T> T get(const std::string&,const std::string&,bool trk=true) const;
	template<typename T> std::vector<T> getV(const std::string&) const;
	void set(const std::string& i,const std::string& v) { map_[i].push_back(v); }
	void track();

private:
	void track(const std::string&,const std::string&) const;
	std::string name() const;
	iterator find(const std::string& i) { return map_.find(i); }
	const_iterator find(const std::string& i) const { return map_.find(i); }
	bool has(const const_iterator& i) const { return i!=map_.end(); }
public:
	const_iterator begin() const { return map_.begin(); }
	const_iterator end() const { return map_.end(); }
};

#include "param_factory.h"

template<typename T>
T Parameters::get(const std::string& index) const
{
	const_iterator i=find(index);
	if(!has(i))
	{
		std::string n=name();
		if(n=="unknown")
			throw std::invalid_argument("no value for '"+index+"'");
		else
			throw std::invalid_argument("no value for '"+index+"' in element with name '"+n+"'");
	}
	return cast<T>(i->second[0],index);
}

template<typename T>
T Parameters::get(const std::string& index,const std::string& def_value,bool trk) const
{
	std::string name="unknown";
	if (trk) 
	{
	    const_iterator n=find("name");
	    if(has(n))
		    name=n->second[0];
    }

	const_iterator i=find(index);
	if(!has(i))
	{
		if (trk)
		    track(name+"."+index,def_value);
		return cast<T>(def_value,index);
	}

	if (trk)
	    track(name+"."+index,i->second[0]);
	return cast<T>(i->second[0],index);
}

template<typename T>
std::vector<T> Parameters::getV(const std::string& index) const
{
	const_iterator i=find(index);
	if(!has(i))
	{
		std::string n=name();
		if(n=="unknown")
			throw std::invalid_argument("no value for '"+index+"'");
		else
			throw std::invalid_argument("no value for '"+index+"' in element with name '"+n+"'");
	}
	std::vector<T> elems;
	for(std::vector<std::string>::const_iterator j=i->second.begin();j!=i->second.end();++j)
		elems.push_back(cast<T>(*j,index));
	return elems;
}
#endif
