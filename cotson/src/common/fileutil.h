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
#ifndef FILEUTIL_H
#define FILEUTIL_H
#include <sys/stat.h>
namespace File {
inline static bool exists(const char *f)
{
    struct stat statbuf; // stat() returns 0 on success
	return !stat(f,&statbuf);
}
inline static unsigned long long length(const char *f)
{
    struct stat statbuf;
    stat(f, &statbuf);
    return(statbuf.st_size); 
}
inline static bool exists(const std::string& f) { return exists(f.c_str()); }
inline static unsigned long long length(const std::string& f) { return length(f.c_str()); }
}

#endif