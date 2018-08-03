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

#ifndef TIMING_L2_H
#define TIMING_L2_H

#include "abaeterno_config.h"
#include "libmetric.h"
#include "liboptions.h"
#include "memory_interface.h"
#include "cpu_timer.h"

namespace Memory {
namespace Timing {

class L2 : public metric
{
 public:
  L2(const Parameters & p);
  void reset();
  uint32_t latency(uint64_t tstamp,const Trace& mt,const Access& ma);
   
 private:
  const uint32_t _nbanks;
  const uint32_t _nsubbanks_per_bank;
  const uint32_t _page_sz;  // page size in byte
  const uint32_t _line_sz;  // cache line size in byte

  // timing parameters
  const uint32_t _clock_multiplier;
  const uint32_t _t_RC;
  const uint32_t _t_RAS;
  const uint32_t _t_additional_hit_latency;

  uint64_t _latest_service_time;
  uint32_t _page_sz_log2;
  uint32_t _line_sz_log2;
  uint32_t _nbanks_log2;
  uint32_t _nsubbanks_per_bank_log2;
  uint64_t _total_contention_cycle;
  uint64_t _total_latency;

  std::vector<uint64_t> _n_bank_conflict;
  std::vector<uint64_t> _n_subbank_access;
  std::vector<uint64_t> _bank_latest_access_cycle;
  std::vector<uint64_t> _subbank_latest_access_cycle;
};

}
}
#endif
