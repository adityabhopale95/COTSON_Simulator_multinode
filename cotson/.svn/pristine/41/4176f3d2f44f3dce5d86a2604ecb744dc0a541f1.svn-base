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
#ifndef UINT64_T_HASH
#define UINT64_T_HASH

#ifndef __x86_64__

namespace boost
{
	std::size_t hash_value(uint64_t v);

	template<> struct hash<uint64_t> : public std::unary_function<uint64_t,std::size_t> 
	{ 
		std::size_t operator()(uint64_t v) const { return boost::hash_value(v); }
	}; 

	template<> struct hash<const uint64_t> : public std::unary_function<const uint64_t,std::size_t> 
	{ 
		std::size_t operator()(const uint64_t v) const  {  return boost::hash_value(v);  } 
	};

}

#endif
#endif
