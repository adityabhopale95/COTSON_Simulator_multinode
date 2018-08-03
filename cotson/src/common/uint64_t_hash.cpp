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

// $Id: uint64_t_hash.cpp 11 2010-01-08 13:20:58Z paolofrb $

#include "uint64_t_hash.h"

#ifndef __x86_64__
namespace boost
{
	std::size_t hash_value(uint64_t v)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed,static_cast<uint32_t>(v>>32));
		boost::hash_combine(seed,static_cast<uint32_t>(v));
		return seed;
	}
}
#endif
