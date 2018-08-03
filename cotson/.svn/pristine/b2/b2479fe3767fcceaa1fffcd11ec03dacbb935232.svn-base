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
#include "twolev.h"
#include "logger.h"

using namespace std;

twolevT::twolevT(uint32_t l1, uint32_t l2, uint32_t hl, bool uxor) :
	hlength(hl), 
	use_xor(uxor),
	lookup_bhr(l1,1), 
	update_bhr(l1,1), 
	pht(l2,1)
{
	lookup_bhr.init(0);
	update_bhr.init(0);
	pht.init(0);

	add("lookup",lookup);
	add("update",update);
	add("misses",pred_miss);
	add("reset",reset);
	add_ratio("miss_rate","misses","lookup");
	clear_metrics();
}

void twolevT::ResetHistory()
{
	reset++;
	lookup_bhr=update_bhr;
}
