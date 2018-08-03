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
#ifndef DUMP_GZIP_H
#define DUMP_GZIP_H

#include <zlib.h>
#include <string>

class DumpGzip
{
	public: 
	DumpGzip(std::string gzName, const char* mode="wb") : ok(true),itsopen(false)
	{
		f=gzopen(gzName.c_str(),mode);
		if(f==NULL) ok=false; else itsopen=true;
	}
	~DumpGzip() { flush(); if(itsopen) gzclose(f); }
	
	void flush() { if(itsopen) gzflush(f,Z_SYNC_FLUSH); } 

	template<typename T>
	DumpGzip& operator<<(T);
	void as_text(const std::string& s)
	{
		if(ok && itsopen)
		{
			size_t size=static_cast<size_t>(s.size());
			size_t w=gzwrite(f,s.c_str(),size);
			if(w!=size)
				ok=false;
		}
	}	

	operator const void*() const { return ok ? this : 0; }
	private:
	bool ok;
	bool itsopen;
	gzFile f;
};

template<typename T>
DumpGzip& DumpGzip::operator<<(T t)
{
	if(ok && itsopen)
	{
		int w=gzwrite(f,&t,sizeof(t));
		if(w!=sizeof(t))
			ok=false;
	}
	return *this;
}
#endif
