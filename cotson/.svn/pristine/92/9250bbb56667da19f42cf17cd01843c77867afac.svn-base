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
#include "abaeterno_config.h"
#include "version.h"
#include "version_info.h" 

#ifndef SVN_REVISION
#define SVN_REVISION "unknown"
#endif
#ifndef GXX_VERSION
#define GXX_VERSION "unknown"
#endif
#ifndef BUILDHOST
#define BUILDHOST "unknown"
#endif
#ifndef BUILDDATE
#define BUILDDATE "unknown"
#endif
#ifndef BUILDUSER
#define BUILDUSER "unknown"
#endif

using namespace std;
using namespace boost;

void printVersion(ostream& log,format& fmt)
{
	log << "Version Information:" << endl << endl;
	log << fmt % "AbAeterno Revision:" % SVN_REVISION << endl;
	log << fmt % "G++ Version:" % GXX_VERSION << endl;
	log << fmt % "Build user:" % BUILDUSER << endl;
	log << fmt % "Build host:" % BUILDHOST << endl;
	log << fmt % "Build date:" % BUILDDATE << endl;
	log << endl;
}

