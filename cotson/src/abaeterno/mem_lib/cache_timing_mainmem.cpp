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

// $Id: cache_timing_l2.cpp 418 2012-11-21 15:09:57Z paolofrb $

#include "cache_timing_mainmem.h"
#include "error.h"
#include "cache.h"

namespace Memory {
namespace Timing {

static inline void reset_vector(std::vector<uint64_t>& vect)
{
    for (std::vector<uint64_t>::iterator it=vect.begin(); it != vect.end(); it++)
      (*it)=0;
}

MainMemory::MainMemory(const Parameters& p):
  _nchannels          (p.get<uint32_t>("nchannels",            "1")),
  _ndimms_per_channel (p.get<uint32_t>("ndimms_per_channel",   "1")),
  _ndevices_per_dimm  (p.get<uint32_t>("ndevices_per_dimm",    "1")),
  _nbanks_per_device  (p.get<uint32_t>("nbanks_per_device",    "1")),
  _page_sz            (p.get<uint32_t>("page_size",            "1")),
  _line_sz            (p.get<uint32_t>("line_size",            "1")),
  _cmd_clk_multiplier (p.get<uint32_t>("cmd_clk_multiplier",   "1")),
  _bank_clk_multiplier(p.get<uint32_t>("bank_clk_multiplier",  "1")),
  _channel_latest_access_cycle(_nchannels, 0),
  _device_latest_access_cycle(_nchannels*_ndevices_per_dimm, 0),
  _dimm_latest_access_cycle(_nchannels*_ndimms_per_channel*_ndevices_per_dimm, 0),
  _bank_latest_access_cycle(_nchannels*_ndimms_per_channel*_ndevices_per_dimm*_nbanks_per_device, 0),
  _total_latency      (0),
  _t_RC               (p.get<uint32_t>("t_RC", "1")),
  _t_RAS              (p.get<uint32_t>("t_RAS", "1")),
  _t_data_transfer    (p.get<uint32_t>("t_data_transfer", "1"))
{
	if (_line_sz > _page_sz)
        ERROR("Cache line size '",_line_sz,"' must be smaller than the page size '",_page_sz,"'");

	add("total_latency ", _total_latency);
	clear_metrics();

	_page_sz_log2            = ilog2(_page_sz);
	_line_sz_log2            = ilog2(_line_sz);
	_nchannels_log2          = ilog2(_nchannels);
	_ndimms_per_channel_log2 = ilog2(_ndimms_per_channel);
	_ndevices_per_dimm_log2  = ilog2(_ndevices_per_dimm);
	_nbanks_per_device_log2  = ilog2(_nbanks_per_device);
}

void MainMemory::reset()
{
  reset_vector( _channel_latest_access_cycle );
  reset_vector( _device_latest_access_cycle );
  reset_vector( _dimm_latest_access_cycle );
  reset_vector( _bank_latest_access_cycle );
}

uint32_t MainMemory::latency(uint64_t tstamp,const Trace& mt,const Access& ma)
{
  uint32_t latency = 0;

  // normalize clock to _cmd_clk_multiplier
  if (tstamp % _cmd_clk_multiplier != 0)
  {
    latency += _cmd_clk_multiplier - (tstamp%_cmd_clk_multiplier);
    tstamp  += latency;
  }

  // address interleaving
  // page  |  column in a page  |  bank  |  dimm  |  device  |  channel  |  cache line
  uint32_t channel_num = (ma.phys >> _line_sz_log2) % _nchannels;
  uint32_t device_num  = (ma.phys >> _line_sz_log2) %
                         (_ndevices_per_dimm*_nchannels);
  uint32_t dimm_num    = (ma.phys >> _line_sz_log2) %
                         (_ndimms_per_channel*_ndevices_per_dimm*_nchannels);
  uint32_t bank_num    = (ma.phys >> _line_sz_log2) %
                         (_nbanks_per_device*_ndimms_per_channel*_ndevices_per_dimm*_nchannels);

  // channel command path conflict
  if (_channel_latest_access_cycle[channel_num] + _cmd_clk_multiplier > tstamp)
  {
    latency += _channel_latest_access_cycle[channel_num] - tstamp + _cmd_clk_multiplier;
    tstamp   = _channel_latest_access_cycle[channel_num]          + _cmd_clk_multiplier;
  }
  _channel_latest_access_cycle[channel_num] = tstamp;

  // device level conflict (actually data transfer time)
  if (_device_latest_access_cycle[device_num] + _t_data_transfer > tstamp)
  {
    latency += _device_latest_access_cycle[device_num] - tstamp + _t_data_transfer;
    tstamp   = _device_latest_access_cycle[device_num]          + _t_data_transfer;
  }
  _device_latest_access_cycle[device_num] = tstamp;

  // dimm level conflict (actually tRRD)
  if (_dimm_latest_access_cycle[dimm_num] + _bank_clk_multiplier > tstamp)
  {
    latency += _dimm_latest_access_cycle[dimm_num] - tstamp + _bank_clk_multiplier;
    tstamp   = _dimm_latest_access_cycle[dimm_num]          + _bank_clk_multiplier;
  }
  _dimm_latest_access_cycle[dimm_num] = tstamp;

  // bank level conflict (actually tRC)
  if (_bank_latest_access_cycle[bank_num] + _t_RC > tstamp)
  {
    latency += _bank_latest_access_cycle[bank_num] - tstamp + _t_RC;
    tstamp   = _bank_latest_access_cycle[bank_num]          + _t_RC;
  }
  _bank_latest_access_cycle[bank_num] = tstamp;

  latency += _t_RAS;
  _total_latency += latency;
  return latency;
}

}
}
