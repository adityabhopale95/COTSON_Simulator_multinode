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

// $Id: kill.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef KILL_H
#define KILL_H

#include <stdlib.h>
#include <execinfo.h>
#include <iostream>

template<typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>& kill(std::basic_ostream<_CharT, _Traits>& o)
{
	o << std::endl;

	void* bt[25];
	int ns = backtrace(bt,25);
	if (ns > 1) { // 0 is "kill" itself
	    char **symbols = backtrace_symbols(bt,ns);
	    o << "Called from:" << std::endl;
        for (int i=1;i<ns;++i)
	        o << "    [" << i-1 << "] " << symbols[i] << std::endl;
	    free(symbols);
	}
	o.flush();
	exit(-1);
	return o;
}

#endif
