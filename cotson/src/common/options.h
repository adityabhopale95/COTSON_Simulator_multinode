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

// $Id: options.h 745 2016-11-25 18:35:57Z robgiorgi $
#ifndef OPTIONS_H
#define OPTIONS_H

#include "cast.h"
#include "parameters.h"

#include <set>
#include <boost/shared_ptr.hpp>
#include <lua.hpp>

class Option
{
	std::string name_;
	bool has_default_;
	std::string default_;
	std::string description_;
	std::vector<std::string> values_;
	bool many_;
	bool raw_;
	luabind::object obj;

	const std::string& getDefault() const;

public:
	Option(const std::string& n,const std::string& d,const std::string& desc) :
		name_(n),
		has_default_(true),
		default_(d),
		description_(desc),
		many_(false),
		raw_(false) {}

	Option(const std::string& n,const std::string& desc) :
		name_(n),
		has_default_(false),
		description_(desc),
		many_(false),
		raw_(false) {}

	void has_many();
	void is_raw();
	void value(const std::string& s);
	void value(const std::vector<std::string>& s);
	void value(const luabind::object& o);
	const luabind::object& get_object() const;
	const std::string& value() const;
	const std::vector<std::string>& values() const;

	inline bool raw() const { return raw_; }
	inline bool has_values() const { return !values_.empty(); }
	inline bool operator<(const Option&two) const { return name_ < two.name_; }
	inline const std::string& Name() const { return name_; }

	// here comes the static part
private:	
	static lua_State *globalL;
	static std::vector<std::string> command_line_;
	static std::vector<std::string> unknown_;
	static std::map<std::string,std::string> requested_;
	static std::map<std::string,Option*> options_;

	static void divide(const std::string&);
	static const Option* getP(const std::string&);
	static void processInit(const std::string&);
	
	static std::string getSimpleValue(luabind::iterator& i);
	static std::string getFromTable(const luabind::object&,const luabind::object&);

public:	
	static void init(int argc,char*argv[],const std::string& base="options");
	static void init(const std::string& commandLine,const std::string& base="options");
	
	static bool has(const char*name);
	template<typename T> static T get(const char*name);
	template<typename T> static T get(const char*name,const T& defval);
	template<typename T> static std::vector<T> getV(const char*name);
	static const std::vector<std::string>& unknown() { return unknown_; }
	static const std::map<std::string,std::string>& requested() { return requested_; }
	
	static void track(const std::string&,const std::string&);
	static void print(std::ostream&,const std::string& progname="Abaeterno");
	
	static Parameters from_lua_table(luabind::object&,const std::set<std::string>&,const std::string&);
	static Parameters from_named_option(const std::string&,const std::set<std::string>&,const std::string&);
	static Parameters from_named_option(const std::vector<std::string>&,const std::set<std::string>&,const std::string&);
	static void run_function(const std::string&);	
	static luabind::object& run_function(const std::string&,uint64_t);
	static luabind::object& run_function(const std::string&,const std::string&);
	static luabind::object& run_function(const std::string&,bool,uint64_t);

	friend class Parameters;
};

// the following structs allow you to define options
// use it this way

// option  whatever("name",default_value,"description");
// option  whatever("name","description");
// options whatever("name","description");

// whatever is the name of a unique variable that should not collide with anything
// this name is not used for anything,
//    "name" is the boost program_options description of the parameter
//    "description" is what gets printed out when help is provided
//    default_value is the default value of the parameter

// options defines a particular option which is composing, i.e. may appear
// several times in the command line

//#include "static_set.h"
struct option
{
	option(const char*,const char*,const char*);
	option(const char*,const char*);
};
struct options { options(const char*,const char*); };
struct optionR { optionR(const char*,const char*); };

template<typename T>
T Option::get(const char*name)
{
    const Option* opt=getP(name);
    if(!opt)
        throw std::invalid_argument((std::string("option ")+"'"+
            name+"' does not exist").c_str());

    const std::string& v=opt->value();
    track(name,v);
    return cast<T>(v,name);
}

template<typename T>
T Option::get(const char*name, const T& defval)
{
    return has(name) ? get<T>(name): defval;
}

template<typename T>
std::vector<T> Option::getV(const char*name)
{
    const Option* opt=getP(name);
    if(!opt)
        throw std::invalid_argument((std::string("option ")+"'"+
            name+"' does not exist").c_str());

    std::vector<T> ts;
    std::string trackable="";
    const std::vector<std::string>& v=opt->values();
    for(std::vector<std::string>::const_iterator i=v.begin();i!=v.end();i++)
    {
        trackable=trackable+*i+", ";
        ts.push_back(cast<T>(*i,name));
    }
    track(name,trackable);
    return ts;
}

#endif
