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

// $Id: opcodes.cpp 95 2010-07-14 17:08:21Z paolofrb $
#include "abaeterno_config.h"

#include "opcodes.h"
#include "logger.h"

Opcodes::Opcodes() : 
	data(hash_size),
	pul(new boost::pool<>(Opcodes::pool_size)) 
{}

Opcodes::~Opcodes()
{
	clear();
	delete pul;
}

void Opcodes::clear()
{
	data.clear();
	delete pul; // wipes out old pool
	pul=new boost::pool<>(Opcodes::pool_size);
}

