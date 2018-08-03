#!/usr/bin/ruby 
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

# $Id: cotson_process.rb 6977 2009-12-09 16:18:35Z frb $

$: << File.expand_path(File.dirname(__FILE__) + '/../share')

class ExperimentStdOut
  def initialize(experiment)
    @experiment = experiment
  end
  def write(s)
    if s.chomp != ''
      @experiment.log(s.gsub(/\e[^m]+m/,"").chomp)
    end
  end
  def flush
    
  end
end

begin
  require 'cotson'
  require 'daemon_service'
  require 'scheduler'

  experiment = Experiment.find(ARGV[0])

  $stdout = ExperimentStdOut.new(experiment)
  
  puts "Starting execution"
  
  node_sandbox=[]
  med_sandbox=nil
  output=[]
  
  #Create a folder and save the lua file
  experiment_dir = "./db/experiments/#{experiment.id}"
  if File.exist?(experiment_dir)
    #TODO: Put a cotson error around it here
    experiment.log("Experiment dir '#{experiment_dir}' already exists")
  else
    FileUtils.mkdir(experiment_dir)
  end
  #TODO: Put the experiment dir at the table?
  Dir.chdir(experiment_dir)
  #Save lua file
  File.open("simulation.input","w") do |f|
    f.print experiment.simulation.lua.gsub("\r","")
  end
  #Save commands file
  File.open("simulation.command","w") do |f| 
    f.print experiment.simulation.command.gsub("\r","")
  end
  #Save scheduler with active and online machines
  #Also putting the graphic heartbeat if required
  #TODO: Change that to something either separate or one file as a complementary
  File.open("scheduler.input","w") do |f|
    f.puts("scheduler = {")
    f.puts("\tpolicy='load-balance',")
    f.print("\thosts={")
    hosts = Machine.find(:all, :conditions => {:active => true}).select {|m| m.online}
    hosts = hosts.collect {|m| "'#{m.host}:#{m.port}'" }
    f.print hosts.join(", ")
    f.puts "}"
    f.puts "}"
    if experiment.experiment_data != nil
      f.puts "-- SQL heartbeat to enable graphics"
      f.puts "options.heartbeat= {"
      f.puts "\ttype=\"pgsql\","
      f.puts "\tdbconn=\"host=hpl-bro05.esp.hp.com dbname=cotson user=cotson password=cotson\","
      f.puts "\texperiment_id=\"#{experiment.experiment_data_id}\" "
      f.puts "}"
    end
  end
  
  parms = ["simulation.input","scheduler.input",'user_script="simulation.command"']

  lua_defs=LuaDefinitions.new(:command_line => ["../../../etc/local-defaults"] + parms)
#    opts=Options.new(lua_defs)
#    if opts.one_node? || !opts.scheduler? then
#      #local execution
#      parms << "../../../local-defaults" 
#    else
#      #daemon execution
#      parms << "../../../daemon-defaults"
#    end
#    
#    lua_defs=LuaDefinitions.new(:command_line => parms)
#    opts=Options.new(lua_defs)
 
    #saving the result file
    #TODO: Take this out once it is not that representative - just works for local
  File.open("experiment.input", "w") {|f| f.puts lua_defs.to_str }
  
  opts=Options.new(lua_defs)
  if(opts.one_node?)
    #TODO: Check what is expected from a web one_node simulation
    opts.load :one_node_script, :data_dir, :display,
      :bsd, :bsd_link,
      :hdd, :hdd_link,
      :vncviewer, :xterm, :screen, :Xvnc, :Xvnc_args, :Xvncwm,
      :simnow_dir, :simnow_dir_link,
      :abaeterno_so, :abaeterno_so_link,
      :clean_sandbox, :screen,
      :copy_files, :copy_files_prefix

    sb=Sandbox.new(lua_defs,opts)
    node_sandbox.push sb
    sb.install :abaeterno_in, :simnow_pre, :simnow_dir,
      :abaeterno_so, :bsd, :hdd
    sb.run(opts[:one_node_script])
  else
    opts.load :cluster_nodes, 
      :mediator_file, :mediator_link,
      :clean_sandbox,
      :subscribe_result,
      :copy_files, :copy_files_prefix

    if opts.scheduler?
      parms = ["../../../etc/web-defaults"] + parms
#        parms[parms.size - 1] = '"' + parms[parms.size - 1] + '"'
      
      scheduler=SchedulerFactory.create_scheduler(lua_defs)

      debug "using scheduler #{scheduler.get_policy}"
      debug "using hosts #{scheduler.hosts.join(",")}"

      debug "creating mediator" 
      hostname=scheduler.get_host(:mediator=>true)
      med_sandbox = DaemonServiceCall.new(hostname)
#        med_sandbox.create_mediator("#{ARGV.join(' ')}", Dir.pwd)
      med_sandbox.create_mediator("#{parms.join(' ')}", Dir.pwd)
      
      cluster_nodes=opts[:cluster_nodes].to_i
      
      #STORE HOST
      host = experiment.experiment_machines.new
      host.mediator = true
      host.machine = 0
      host.server = hostname
      host.sandbox_id = med_sandbox.id
      host.save
      
      debug "creating #{cluster_nodes} nodes" 
      node_sandbox=[]
      (1..cluster_nodes).each do |node| 
        hostname = scheduler.get_host
        x = DaemonServiceCall.new(hostname)#scheduler.get_host)
#          x.create_node(node, "#{ARGV.join(' ')}", Dir.pwd)
        puts "Creating node #{node} at #{hostname} with #{parms.join(' ')} at #{Dir.pwd}"
        x.create_node(node, "#{parms.join(' ')}", Dir.pwd)
        node_sandbox.push x

        #Save host
        host = experiment.experiment_machines.new
        host.mediator = false
        host.machine = node
        host.server = hostname
        host.sandbox_id = x.id
        host.save

      end
    else
      debug "using local host"

      cluster_nodes=opts[:cluster_nodes].to_i

      med_sandbox=Sandbox.new(lua_defs,opts)
      med_sandbox.install :mediator, :abaeterno_in

      (1..cluster_nodes).each do |node|

        lua_defs.instance_defaults :NODE => node, :TOTAL => cluster_nodes
        nopts=Options.new(lua_defs, :NODE => node, :TOTAL => cluster_nodes)
        nopts.load :data_dir, :display,
          :bsd, :bsd_link,
          :hdd, :hdd_link,
          :vncviewer, :xterm, :screen, :Xvnc, :Xvnc_args, :Xvncwm, 
          :simnow_dir, :simnow_dir_link,
          :abaeterno_so, :abaeterno_so_link,
          :user_script, :clean_sandbox,
          :subscribe_result

        nsb=Sandbox.new(lua_defs,nopts)
        nsb.install :abaeterno_in, :simnow_pre_cluster, :simnow_dir,
          :abaeterno_so, :bsd, :hdd, :user_script, :cluster_sh

        node_sandbox.push nsb
      end

    end

    debug "starting mediator" 
    output << med_sandbox.mediator

    debug "getting mediator configuration"
    vals={}
    names=%w{host control_port multicast_ip multicast_port quantum_max}
    names.each do |x|
      vals[x]=med_sandbox.get_mediator_value(x).chomp
    end

    debug "setting mediator configuration into nodes" 
    node_sandbox.each do |x|
      vals.each do |y,z|
        x.set_mediator_value(y,z) 
      end
    end

    node_sandbox.each do |x|
      debug "starting node" 
      output << x.start_go
    end

    if opts.scheduler?
      node_sandbox.each do |x|
        puts "ping"
        puts x.ping
        debug "getting vnc port for node #{x.id}" 
        vnc_port=x.get_vnc_webport
        puts "vnc port"
        puts vnc_port.strip
        if vnc_port =~ /.*display_http=(\d+).*/
          vnc_port = Integer($1)
        end
        puts "after: vncport"
        puts vnc_port
        
        if vnc_port 
          puts "Setting port #{$1}"
          host = experiment.experiment_machines.find(:first, :conditions => {:sandbox_id => x.id})
          puts "HOST NOT FOUND" if host == nil
          if host
            host.vnc_port = $1
            host.save
          end
        end
      end
    end


    debug_nnl("waiting ")
    running=true
    while running do
      node_sandbox.each do |x|
        status=x.status
#          debug_nnl(".")
        if status==false
          running=false
          break
        end
      end
      sleep(1)
    end
    debug ""

    node_sandbox.each do |x|
      debug "stopping node" 
      x.kill  
    end

    debug "stopping mediator" 
    med_sandbox.kill 

    extra_prefix=opts[:copy_files_prefix]
    debug "copying mediator output to #{Dir.pwd} " 
    med_sandbox.copy_files(:type => :output, 
        :filter => opts[:copy_files],
        :prefix => "#{extra_prefix}med.",
        :destination => Dir.pwd)
  end
  i=1
  node_sandbox.each do |x|
    debug "copying node #{i} output to #{Dir.pwd} " 
    x.copy_files(:type => :output, 
      :filter => opts[:copy_files],
      :prefix => "#{extra_prefix}node.#{i}.",
      :destination => Dir.pwd)
    i+=1
  end
rescue Exception => e
  finish e, :med_sandbox => med_sandbox, :node_sandbox => node_sandbox, :output => output  
ensure 
  puts("Cleaning sandboxes")
  node_sandbox.each { |x| x.clean }
  med_sandbox.clean if med_sandbox
  puts("Finishing execution")
  experiment.ended_at = Time.now
  experiment.save
  puts "The end"
end
