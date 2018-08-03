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

// $Id: options.cpp 745 2016-11-25 18:35:57Z robgiorgi $

#include "options.h"
#include "fileutil.h"
#include "static_set.h"

#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

using namespace boost;
using namespace std;

lua_State *Option::globalL;
vector<string> Option::command_line_;
vector<string> Option::unknown_;
map<string,string> Option::requested_;
map<string,Option*> Option::options_;

luabind::scope definition();

void Option::divide(const string& data)
{
	typedef escaped_list_separator<char> sep;
	typedef tokenizer<sep> args;

	args tokens(data,sep('\\',' ','\0'));

	for(args::iterator i=tokens.begin();i!=tokens.end();i++)
		if(string(*i) != "")
			command_line_.push_back(*i);
}

string Option::getSimpleValue(luabind::iterator& i)
{
	return cast(i);	
}

string Option::getFromTable(const luabind::object& t,const luabind::object& s)
{
	const luabind::object& i=t[s];
	int type=luabind::type(i);	
	if(type==LUA_TBOOLEAN)
		return (i==true)?"1":"0";

	if(type==LUA_TNUMBER || type==LUA_TSTRING)
		return lexical_cast<string>(i);

	return "";
}

Parameters Option::from_lua_table(
    luabind::object& table,
	const set<string>& required,
	const string& name)
{
	if(luabind::type(table) != LUA_TTABLE)
		throw runtime_error(name +" needs a table as parameter");

	Parameters p;

	set<string> unused;
	for(luabind::iterator j(table), end; j != end; ++j)
		unused.insert(lexical_cast<string>(j.key()));
									
	for(luabind::iterator j(table), end; j != end; ++j)
	{
		string sss=lexical_cast<string>(j.key());
		if(required.find(sss)==required.end())
			continue;
		if(luabind::type(table[j.key()])==LUA_TTABLE)
		{
			for(luabind::iterator k(table[j.key()]), end2; k != end2; ++k)
			{
				string value=getSimpleValue(j);
				if(value!="")
					p.set(sss,value);
			}
		}
		else
		{
			string value=getFromTable(table,j.key());
			if(value=="")
				throw runtime_error(name +" needs a parameter called "+sss);
			p.set(sss,value);
			unused.erase(sss);
		}
	}
	
	for(luabind::iterator j(table), end; j != end; ++j)
	{
		string sss=lexical_cast<string>(j.key());
		if(unused.find(sss)==unused.end())
			continue;
		if(luabind::type(table[j.key()])==LUA_TTABLE)
		{
			for(luabind::iterator k(table[j.key()]), end2; k != end2; ++k)
			{
				string value=getSimpleValue(k);
				if(value!="")
					p.set(sss,value);
			}
		}
		else
		{
			string value=getFromTable(table,j.key());
			if(value!="")
				p.set(sss,value);
		}
	}

	return p;
}

Parameters Option::from_named_option(
    const string& name,
	const set<string>& required,
	const string& class_name)
{
	const Option* opt=getP(name);
	if(!opt)
		throw runtime_error("option " + name +" is not defined");
	if(!opt->raw())
		throw runtime_error(name +" should have been a raw option");
	
	luabind::object& table=const_cast<luabind::object&>(opt->get_object());
	if(!table)
		throw runtime_error("option " + name +" is not defined as a table");
	return Option::from_lua_table(table,required,class_name);
}

Parameters Option::from_named_option(
    const vector<string>& name,
	const set<string>& required,
	const string& class_name)
{
	vector<string>::const_iterator i=name.begin();
	const Option* opt=getP(*i);
	
	if(!opt)
		throw runtime_error("option " + *i +" is not defined");
	if(!opt->raw())
		throw runtime_error(*i +" should have been a raw option");
	
	luabind::object& table=const_cast<luabind::object&>(opt->get_object());
	if(!table)
		throw runtime_error("option " + *i +" is not defined as a table");
 	if(i==name.end())
	{
		return Option::from_lua_table(table,required,class_name);
	}
	
	for(i++;i!=name.end();i++)
	{
		bool found=false;
		for(luabind::iterator j(table), end; j != end; ++j)
		{
			string key=lexical_cast<string>(j.key());
			if(key.compare(*i)==0) 
			{
				if(luabind::type(table[j.key()])!=LUA_TTABLE)
					throw runtime_error("option " + *i +" is not defined as a table");
				else
				{
					found=true;
					table=table[j.key()];				
					break;
				}
		    }
		}	
		if(!found)
			throw runtime_error("option " + *i +" was not defined");
	}
	return Option::from_lua_table(table,required,class_name);
}
 
void Option::processInit(const string& base)
{
	if(globalL)
		throw runtime_error("cant init twice");

	globalL=lua_open();

	luabind::open(globalL);
	luabind::module(globalL)
	[
		definition()
	];

	static const luaL_Reg useTheseLibs[] =
	{
		{"", luaopen_base},
		// 	{LUA_LOADLIBNAME, luaopen_package}, // since it needs -ldl
		// 	{LUA_TABLIBNAME, luaopen_table},
		// 	{LUA_IOLIBNAME, luaopen_io},
			{LUA_OSLIBNAME, luaopen_os},
		// 	{LUA_STRLIBNAME, luaopen_string},
		// 	{LUA_MATHLIBNAME, luaopen_math},
		// 	{LUA_DBLIBNAME, luaopen_debug},
		{NULL, NULL}
	};

	for(const luaL_Reg *lib = useTheseLibs;lib->func; lib++)
	{
		lua_pushcfunction(globalL,lib->func);
		lua_pushstring(globalL, lib->name);
		lua_call(globalL, 1, 0);
	}

	string pre_state=
		"include=dofile;\n"
		"options={};\n"
		"simnow={};\n"
		"mediator={};\n";

	luaL_dostring(globalL,pre_state.c_str());

	for(vector<string>::iterator i=command_line_.begin();
		i!=command_line_.end();
		++i)
	{
		int result;
		const char *istr = i->c_str();
		if(File::exists(istr))
			result = luaL_dofile(globalL,istr);
		else
			result = luaL_dostring(globalL,istr);

		if(result!=0)
			throw invalid_argument(lua_tostring(globalL,-1));
	}

	typedef StaticSet< boost::shared_ptr<Option> > Opts;
	for(Opts::iterator i=Opts::begin();i!=Opts::end(); ++i)
		options_[(*i)->Name()]=i->get();

	for(luabind::iterator i(luabind::globals(globalL)[base]), end;
		i != end; ++i)
	{
		string key=lexical_cast<string>(i.key());
		const Option* opt=getP(key);
		if(!opt)
		{
			unknown_.push_back(key);
			continue;
		}

		// Hack to work around missing const members in luabind
		Option* wopt=const_cast<Option*>(opt);
		if(luabind::type(*i)==LUA_TTABLE)
		{
			if(opt->raw())
				wopt->value(*i);
			else
			{
				vector<string> v;
				for(luabind::iterator j(*i), end2; j != end2; ++j)
					v.push_back(getSimpleValue(j));
				wopt->value(v);
			}
		}
		else
			wopt->value(getSimpleValue(i));
	}
}

void Option::track(const string& option,const string& value)
{
	requested_[option]=value;
}

bool Option::has(const char*name)
{
	const Option* opt=getP(name);
	if(!opt)
		throw invalid_argument((string("option ")+
			name+" has not been subscribed").c_str());

	if(opt->raw())
		return opt->get_object() ? true : false;
	
	bool exists=opt->has_values();
	if(exists)
		requested_[name]="true";
	return exists;
}

void Option::init(int argc,char*argv[],const string& base)
{
	for(int i=1;i<argc;i++)
	{
		string arg(argv[i]);
		if(arg != "")
			command_line_.push_back(arg);
	}
	processInit(base);
}

void Option::init(const string& data,const string& base)
{
	divide(data);
	processInit(base);
}

const Option* Option::getP(const string& name)
{
	map<string,Option*>::iterator where=options_.find(name);
	if(where==options_.end())
		return 0;
	return where->second;
}

void Option::print(ostream& o,const string& progname)
{
	format fmt("%1% %2%%|50t|%3%");

	o << progname << " Options" << endl << endl;
	for(map<string,Option*>::iterator i=options_.begin();
		i!=options_.end();++i)
	{
		Option* opt=i->second;
		string def;
		if(opt->has_default_)
			def="("+opt->default_+")";
		o << fmt 
			% opt->name_ 
			% def 
			% opt->description_ 
		<< endl;
	}
}

void Option::run_function(const string& func_name)
{
	luabind::object func=luabind::globals(globalL)[func_name];
	if(luabind::type(func) != LUA_TFUNCTION)
		throw invalid_argument("configuration function '"+func_name+
			"' is not defined");

	try
	{
		func();
	}
	catch(luabind::error& er)
	{
		throw invalid_argument("configuration function '"+func_name+
			"' produced the following error: "+ lua_tostring(globalL,-1));
	}
}

const string& Option::getDefault() const
{
    if(!has_default_)
        throw invalid_argument((string("option ")+
            name_+" does not have a default value").c_str());
    return default_;
}

void Option::has_many()
{
    if(has_default_)
        throw invalid_argument((string("option ")+
        name_+" can not have a default and request many values").c_str());
    many_=true;
}

void Option::is_raw()
{
    if(has_default_)
        throw invalid_argument((string("option ")+
        name_+" can not have a default and request being raw").c_str());
    raw_=true;
}

void Option::value(const string& s)
{
    if(!many_ && !values_.empty())
        throw invalid_argument((string("option ")+
        name_+" does not allow many values").c_str());
    values_.push_back(s);
}
    
void Option::value(const vector<string>& s)
{
    if(!many_)
        throw invalid_argument((string("option ")+
            name_+" does not allow many values").c_str());
    values_=s;
}

void Option::value(const luabind::object& o)
{
    if(!raw_)
        throw invalid_argument((string("option ")+
            name_+" does not allow raw values").c_str());
    obj=o;
}

const luabind::object& Option::get_object() const
{
    if(!raw_)
        throw invalid_argument((string("option ")+
            name_+" does not allow raw values").c_str());
    return obj;
}

const string& Option::value() const
{
    if(many_)
        throw invalid_argument((string("option ")+
            name_+" has many values and cant decide which one to give back").c_str());
    if(values_.empty())
        return getDefault();
    return values_.front();
}

const vector<string>& Option::values() const
{
    if(!many_)
        throw invalid_argument((string("option ")+
            name_+" has one value and has been asked for many").c_str());
    if(values_.empty())
        throw invalid_argument((string("option ")+
            name_+" does not have a default value").c_str());
    return values_;
}

option::option(const char* name,const char* def,const char* description)
{
    try
    {
        boost::shared_ptr<Option> opt(new Option(name,def,description));
        registerSetParam<boost::shared_ptr<Option> > a(opt);
    }
    catch(invalid_argument& arg)
    {
        if(arg.what()==string("duplicate"))
            throw invalid_argument(string("Option ")+name+" is duplicated");
        else
            throw;
    }
}

option::option(const char* name,const char* description)
{
    try
    {
        boost::shared_ptr<Option> opt(new Option(name,description));
        registerSetParam<boost::shared_ptr<Option> > a(opt);
    }
    catch(invalid_argument& arg)
    {
        if(arg.what()==string("duplicate"))
            throw invalid_argument(string("Option ")+name+" is duplicated");
        else
            throw;
    }
}

options::options(const char* name,const char* description)
{
    try
    {
        boost::shared_ptr<Option> opt(new Option(name,description));
        opt->has_many();
        registerSetParam<boost::shared_ptr<Option> > a(opt);
    }
    catch(invalid_argument& arg)
    {
        if(arg.what()==string("duplicate"))
            throw invalid_argument(string("Option ")+name+" is duplicated");
        else
            throw;
    }
}

optionR::optionR(const char* name,const char* description)
{
    try
    {
        boost::shared_ptr<Option> opt(new Option(name,description));
        opt->is_raw();
        registerSetParam<boost::shared_ptr<Option> > a(opt);
    }
    catch(invalid_argument& arg)
    {
        if(arg.what()==string("duplicate"))
            throw invalid_argument(string("Option ")+name+" is duplicated");
        else
            throw;
    }
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

luabind::object ttttt;

// CAUTION!! This function is NOT reentrant. It always returns a & to the
// same variable 
luabind::object& Option::run_function(const string& func_name, uint64_t param)
{
	luabind::object func=luabind::globals(globalL)[func_name];

	if(luabind::type(func) != LUA_TFUNCTION)
		throw invalid_argument("configuration function '"+func_name+
			"' is not defined");

	try
	{
		return ttttt=func(param);
	}
	catch(luabind::error& er)
	{
		throw invalid_argument("configuration function '"+func_name+
			"' with parameter '"+lexical_cast<string>(param)+
			"' produced the following error: "+lua_tostring(globalL,-1));
	}
	
}

// CAUTION!! This function is NOT reentrant. It always returns a & to the
// same variable 
luabind::object& Option::run_function(const string& func_name, const string& param)
{
	luabind::object func=luabind::globals(globalL)[func_name];

	if(luabind::type(func) != LUA_TFUNCTION)
		throw invalid_argument("configuration function '"+func_name+
			"' is not defined");

	try
	{
		return ttttt=func(param);
	}
	catch(luabind::error& er)
	{
		throw invalid_argument("configuration function '"+func_name+
			"' with parameter '"+lexical_cast<string>(param)+
			"' produced the following error: "+lua_tostring(globalL,-1));
	}
}

// CAUTION!! This function is NOT reentrant. It always returns a & to the
// same variable 
luabind::object& Option::run_function(const string& func_name,bool a,uint64_t b)
{
	luabind::object func=luabind::globals(globalL)[func_name];

	if(luabind::type(func) != LUA_TFUNCTION)
		throw invalid_argument("configuration function '"+func_name+
			"' is not defined");

	try
	{
		return ttttt=func(a,b);
	}
	catch(luabind::error& er)
	{
		throw invalid_argument("configuration function '"+func_name+
			"' with parameters '"+lexical_cast<string>(a)+
			"' and '"+lexical_cast<string>(b)+
			"' produced the following error: "+lua_tostring(globalL,-1));
	}
}

