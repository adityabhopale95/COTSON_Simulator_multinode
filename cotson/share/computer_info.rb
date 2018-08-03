#!/usr/bin/ruby -w
# (C) Copyright 2009 Hewlett-Packard Development Company, L.P.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
class Convert
  @@to_unit = { :B => 1, :KB => 1024, :MB => 1048576, :GB => 1073741824 }
  def Convert.value_to_unit(value, unit)
    value_from_to(value,unit,:B)
  end
  def Convert.value_from_to(value, from, to)
    value * @@to_unit[from.to_sym] / @@to_unit[to.to_sym]
  end
end

class ComputerInfo
  def ComputerInfo.info
    %x{uname -a}.strip
  end
  def ComputerInfo.release
    %x{uname -r}.strip
  end
  def ComputerInfo.processors
    p=`cat /proc/cpuinfo | grep processor | wc -l`
    p.strip.to_i
  end
  def ComputerInfo.memory(info=nil)
    mem_info={}    
    %x{cat /proc/meminfo}.each_line do |l| 
      l =~ /(.*):\s+(\d+)\s(.*)/
      if $3 != nil && $3 != ''
        mem_info[$1.to_sym]= Convert.value_to_unit($2.to_i, $3.upcase)
      else
        mem_info[$1.to_sym]= $2.to_i 
      end
    end
    if info
      mem_info[info]
    else
      mem_info
    end
  end
  def ComputerInfo.ps(process=nil)
    p=[]
    arg=process ? "-C #{process} hwwo" : "hwwaxo"
    arg+=@@ps_fields.join(",")
    %x{ps #{arg}}.each_line do |l|
      fields=l.strip.split(/\s+/,@@ps_fields.size) #use limit
      r={}
      r[:info] = l.strip
      (0..@@ps_fields.size-1).each do |i|
        r[@@ps_fields[i]]=fields[i]
      end
      p << r
    end
    p
  end
  #command must be the last
  @@ps_fields=[:egroup,:etime,:euser,:nlwp,:pcpu,:pgid,:pid,:pmem,:ppid,:size,:sz,:tid,:time,:command]
end
