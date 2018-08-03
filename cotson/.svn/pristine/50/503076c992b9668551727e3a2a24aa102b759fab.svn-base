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
#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

/* For future reference (RG150625):
#if defined(__GNUC__) && (__GNUC__*100+__GNUC__ >= 408) //GCC >=4.8
    #undef HashMap
    #undef HashSet
    #include <unordered_map>
    #include <unordered_set>
    #define HashMap std::unordered_map
    #define HashSet std::unordered_set
#else
*/
    #if defined(__GNUC__) && (__GNUC__*100+__GNUC__ >= 402) //GCC >= 4.2
        #define __HAVE_TR1
        #include <tr1/unordered_map>
        #include <tr1/unordered_set>
        #define HashMap std::tr1::unordered_map
        #define HashSet std::tr1::unordered_set
    #else
        #include <map>
        #include <set>
        #define HashMap std::map
        #define HashSet std::set
    #endif
/*
#endif // GCC >= 4.8
*/

#endif // HASHMAP_H_INCLUDED
