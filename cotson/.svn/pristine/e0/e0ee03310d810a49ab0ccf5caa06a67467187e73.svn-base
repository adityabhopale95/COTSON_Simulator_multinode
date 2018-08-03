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

#include "cache_timing_l2.h"
#include "error.h"
#include "cache.h"

namespace Memory {
namespace Timing {

static inline uint32_t _log2(uint32_t num) 
{ 
    if (num == 0)
 	    throw std::runtime_error("Cannot compute log2(0)");
    return Memory::ilog2(num); 
}

static inline void reset_vector(std::vector<uint64_t>& vect)
{
    for (std::vector<uint64_t>::iterator it=vect.begin(); it != vect.end(); it++)
      (*it)=0;
}

L2::L2(const Parameters & p)
 : _nbanks           (p.get<uint32_t>("nbanks", "1")),
  _nsubbanks_per_bank(p.get<uint32_t>("nsubbanks_per_bank", "1")),
  _page_sz           (p.get<uint32_t>("page_size", "1")),
  _line_sz           (p.get<uint32_t>("line_size", "1")),
  _clock_multiplier  (p.get<uint32_t>("clock_multiplier", "1")),
  _t_RC              (p.get<uint32_t>("t_RC", "1")),
  _t_RAS             (p.get<uint32_t>("t_RAS", "1")),
  _t_additional_hit_latency(p.get<uint32_t>("t_additional_hit_latency", "1")),
  _latest_service_time(0),
  _n_bank_conflict   (_nbanks),
  _n_subbank_access  (_nbanks*_nsubbanks_per_bank),
  _bank_latest_access_cycle       (_nbanks,                     0),
  _subbank_latest_access_cycle    (_nbanks*_nsubbanks_per_bank, 0)
{
  if (_line_sz > _page_sz)
    ERROR("Cache line size '",_line_sz,"' must be smaller than the page size '",_page_sz,"'");

  /*
  for (uint32_t i = 0; i < _nbanks*_nsubbanks_per_bank; i++)
  {
    add(".n_subbank_access_"+boost::lexical_cast<std::string>(i)+" ", _n_subbank_access[i]);
  }
  for (uint32_t i = 0; i < _nbanks; i++)
  {
    add(".n_bank_cnflt_"    +boost::lexical_cast<std::string>(i)+" ", _n_bank_conflict[i]);
  } 
  */ // Jung Ho : interesting in general, but not now. 
  
  add("total_contention_cycle ", _total_contention_cycle);
  add("total_latency ", _total_latency);
  clear_metrics();

  _page_sz_log2 = _log2(_page_sz);
  _line_sz_log2 = _log2(_line_sz);
  _nbanks_log2  = _log2(_nbanks);
  _nsubbanks_per_bank_log2 = _log2(_nsubbanks_per_bank);
}

void L2::reset()
{
  //_n_bank_conflict.clear();
  //_n_subbank_access.clear(),
  reset_vector( _bank_latest_access_cycle );
  reset_vector( _subbank_latest_access_cycle );

  _latest_service_time = 0;
}

uint32_t L2::latency(uint64_t tstamp, const Trace& mt, const Access& ma)
{
  uint32_t latency = 0;

  // normalize clock
  if (tstamp % _clock_multiplier != 0)
  {
    latency += _clock_multiplier - (tstamp%_clock_multiplier);
    tstamp  += latency;
  }

  // TODO: how to map an address to a bank is important and must be
  // parameterized.
  uint32_t bank_num = (ma.phys >> _line_sz_log2) % _nbanks;


  // bank conflict
  if (_bank_latest_access_cycle[bank_num] + _clock_multiplier > tstamp)
  {
    _n_bank_conflict[bank_num]++;
    _total_contention_cycle += _bank_latest_access_cycle[bank_num] - tstamp + _clock_multiplier;
    latency                 += _bank_latest_access_cycle[bank_num] - tstamp + _clock_multiplier;
    tstamp                   = _bank_latest_access_cycle[bank_num]          + _clock_multiplier;
  }
  _bank_latest_access_cycle[bank_num] = tstamp;

  uint32_t subbank_num = (bank_num << _nsubbanks_per_bank_log2) +
                         ((ma.phys >> (_page_sz_log2+_nbanks_log2)) % _nsubbanks_per_bank);

  if (_subbank_latest_access_cycle[subbank_num] + _t_RC > tstamp)
  {
    _n_subbank_access[subbank_num]++;
    _total_contention_cycle += _subbank_latest_access_cycle[subbank_num] + _t_RC - tstamp;
    latency                 += _subbank_latest_access_cycle[subbank_num] + _t_RC - tstamp;
    tstamp                   = _subbank_latest_access_cycle[subbank_num] + _t_RC;
  }
  _subbank_latest_access_cycle[subbank_num] = tstamp;
  latency += _t_RAS + _t_additional_hit_latency;

  _total_latency += latency;
  return latency;
}

}
}

