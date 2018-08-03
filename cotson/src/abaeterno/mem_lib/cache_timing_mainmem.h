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

// $Id: cache_timing_l2.h 11 2010-01-08 13:20:58Z paolofrb $

#ifndef TIMING_MAINMEM_H
#define TIMING_MAINMEM_H

#include "abaeterno_config.h"
#include "libmetric.h"
#include "liboptions.h"
#include "memory_interface.h"
#include "cpu_timer.h"

namespace Memory {
namespace Timing {

class MainMemory : public metric
{
 public:
  MainMemory(const Parameters & p);
  void reset();
  uint32_t latency(uint64_t tstamp,const Trace& mt,const Access& ma);
   
 private:
  
  const uint32_t _nchannels;
  const uint32_t _ndimms_per_channel;
  const uint32_t _ndevices_per_dimm;
  const uint32_t _nbanks_per_device;
  const uint32_t _page_sz;  // page size in byte
  const uint32_t _line_sz;  // cache line size in byte

  const uint32_t _cmd_clk_multiplier;
  const uint32_t _bank_clk_multiplier;

  std::vector<uint64_t> _channel_latest_access_cycle;
  std::vector<uint64_t> _device_latest_access_cycle;
  std::vector<uint64_t> _dimm_latest_access_cycle;
  std::vector<uint64_t> _bank_latest_access_cycle;

  uint64_t _total_latency;
  uint32_t _nchannels_log2;
  uint32_t _ndimms_per_channel_log2;
  uint32_t _ndevices_per_dimm_log2;
  uint32_t _nbanks_per_device_log2;
  uint32_t _page_sz_log2;
  uint32_t _line_sz_log2;


  const uint32_t _t_RC;
  const uint32_t _t_RAS;
  const uint32_t _t_data_transfer;
};


}
}
#endif

