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

// $Id: units.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "units.h"

template<>
units<std::string>::units(const char* cs) : n(cs) {}

template<>
std::ostream& operator<< (std::ostream& o,const units<std::string>& d)
{
	return o << d.n;
}

template<>
std::istream& operator>> (std::istream& i,units<std::string>& d)
{
	return i >> d.n;
}


