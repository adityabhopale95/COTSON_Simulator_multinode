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
#ifndef EXCEPT_ON_FILE_H
#define EXCEPT_ON_FILE_H

#include "selector.h"

namespace ExceptOnFile
{
	struct ExceptOnFile : public Selector
	{
		std::string filename;
		ExceptOnFile(const std::string& name) : filename(name) {}
		virtual void Do(const char* file, int line) const
		{
			std::ifstream f(filename.c_str());
			if(f)
				throw std::runtime_error("exception because I found file "+filename);
		}
	};
	
	static ExceptOnFile Make(const char*n)
	{
		return ExceptOnFile(n);
	}
}

#define EXCEPT_ON_FILE SELECTOR(ExceptOnFile)

#endif
