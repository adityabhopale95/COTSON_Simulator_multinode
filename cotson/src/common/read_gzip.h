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

// $Id: read_gzip.h 726 2015-08-04 10:27:52Z robgiorgi $
#ifndef READ_GZIP_H
#define READ_GZIP_H

#include <zlib.h>
#include <string>

class ReadGzip 
{
	public: 
	ReadGzip(std::string gzName, size_t bufsz=0): ok(true),data(true)
	{
		f=gzopen(gzName.c_str(),"rb");
		if(f==NULL)
			ok=false;
// The following is not supported in e.g. ZLIB_VERNUM 0x1234 (i.e. Ubuntu 11.10,12.04)
// We restrict the use of gzbuffer only to ZLIB_VERNUM >= 0x1280
#if (ZLIB_VERNUM >= 0x1280)
		if (bufsz)
	        gzbuffer(f,bufsz);
#endif
	}
	~ReadGzip() { if(ok) gzclose(f); }
	template<typename T>
	ReadGzip& operator>>(T&);

	operator const void*() const { return ok&&data ? this : 0; }
// The following seems a reasonable bug fix. However, it's messing up regression tests
// on Ubuntu 11.10,12.04 (at least). So we revert to the older version (as in revisions < 721)
// in order to continue supporting 11.10 and 12.04 (at least).
#if (ZLIB_VERNUM >= 0x1280)
	bool eof() const { return !data || gzeof(f); }
#else
        bool eof() const { return !data; }
#endif

	std::string as_text(size_t n)
	{
		if(ok)
		{
			char buf[n];
			size_t w=gzread(f,buf,n);
			if(w!=n) 
				ok=false;
		    return std::string(buf);
		}
		return std::string();
	}

	private:
	bool ok,data;
	gzFile f;
};

template<typename T>
ReadGzip& ReadGzip::operator>>(T& t)
{
	if(ok&&data)
	{
		int w=gzread(f,&t,sizeof(t));
// The following seems a reasonable bug fix. However, it's messing up regression tests
// on Ubuntu 11.10,12.04 (at least). So we revert to the older version (as in revisions < 721)
// in order to continue supporting 11.10 and 12.04 (at least).
#if (ZLIB_VERNUM >= 0x1280)
		if ((w<=0) || gzeof(f))
#else
                if(w==0 && gzeof(f))
#endif
			data=false;
		else if(w!=sizeof(t))
			ok=false;
	}
	return *this;
}
#endif
