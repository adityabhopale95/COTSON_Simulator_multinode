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

# $Id$

require 'daemon_service'

class SchedulerFactory
  @@schedulers = []

  def SchedulerFactory.add(s)  
    @@schedulers.push(s)
  end

  def SchedulerFactory.create_scheduler(lua_defs)
    opts=Options.new(lua_defs)
    opts.load :scheduler_policy
    scheduler_policy = opts[:scheduler_policy]
    @@schedulers.each do |s| 
      if s.get_policy == scheduler_policy then
        return s::new(lua_defs)        
      end
    end
    raise CotsonError.new(:SchedulerCreate, :policy=>scheduler_policy, :policy_list=>scheduler_list.join(", "))
  end
  
  def SchedulerFactory.scheduler_list
    list=[]
    @@schedulers.each {|s| list << s.get_policy }
    list
  end
end

class Scheduler
  @hosts = []
  def Scheduler.inherited(s)
    SchedulerFactory.add(s)
  end
  
  def initialize(lua_defs)
    opts=Options.new(lua_defs)
    opts.load :scheduler_hosts
    @hosts = opts[:scheduler_hosts].split(/\s+/)
    raise CotsonError.new(:SchedulerInitialize, :hosts=>@hosts) if @hosts.size == 0
  end
  
  def Scheduler.get_policy
    return ""
  end
  
  def get_policy
    self.class.get_policy
  end
  
  def get_host(parms=nil)
    raise CotsonError.new(:SchedulerGetHost, :scheduler=>self.class, :hosts=>hosts)
  end
  
  def hosts
    @hosts
  end
end

class RoundRobin < Scheduler
  def initialize(lua_defs)
    super lua_defs
    @next = 0
  end
  
  def RoundRobin.get_policy
    return "round-robin"
  end
  
  def get_host(parms=nil)
    host=@hosts[@next]  
    @next= @next+1 >= @hosts.size ? 0 : @next+1
    host
  end
end

class LoadBalance < Scheduler
  def initialize(lua_defs)
    @servers={}
    @scheduled={}
    super lua_defs
    @hosts.each do |h|
      @servers[h] = DaemonServiceCall.new(h)
      @scheduled[h]=0
    end
  end
  
  def LoadBalance.get_policy
    return "load-balance"
  end
  
  def get_host(parms=nil)
    host=nil
    max_free=nil
    @servers.each_pair do |h,s|
      load = s.get_load
      if !max_free || load['Processors'] - load['Simulations'] - @scheduled[h] > max_free then
        host = h
        max_free = load['Processors'] - load['Simulations'] - @scheduled[h]
      end
    end
    #TODO: Will there be a limitation when no processors are available?
    #raise CotsonError.new(:LoadBalanceGetHost, :server_info=>@servers, :host=>'') if ! host
    @scheduled[host]+=1  if !parms || !parms[:mediator] 
    host
  end
end

