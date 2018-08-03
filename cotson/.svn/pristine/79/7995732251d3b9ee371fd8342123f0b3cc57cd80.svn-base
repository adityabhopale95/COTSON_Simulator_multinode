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
#ifndef STATES_H
#define STATES_H

typedef enum 
{
	CODE=0,         // identifies code translations
	MEMORY,         // identifies memory accesses
	EXCEPT,         // identifies exceptions
	HEARTBEAT,      // identifies heartbeat events
	INJECT_STATES   // amount of states
} InjectState;

typedef enum 
{
	FUNCTIONAL=0,   // no event generation
	SIMPLE_WARMING, // warming of caches + bp (long histeresis)
	FULL_WARMING,   // + warming of internal structs (short histeresis)
	SIMULATION,     // full & detailed simulation
	END,            // forces and end of the simulation
	SIM_STATES      // amount of states
} SimState; 

#endif
