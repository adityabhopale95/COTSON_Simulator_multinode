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

# $Id: sandbox.rb 745 2016-11-25 18:35:57Z robgiorgi $

require 'location'
require 'lua_scripts'
require 'options'
require 'fileutils'

class Sandbox < Location
  private
  def create
    sandbox_src=$here.from_root('sandbox')
    debug2 "creating sandbox"
    debug2 "  from #{sandbox_src}"
    debug2 "  into #{@root}"

    raise CotsonError.new(:SandboxCreate, :sandbox => $here, :source => sandbox_src, :destination=>@root.to_str ) if !File.exist?(sandbox_src) || File.exist?(@root)
    
    FileUtils.mkdir_p File.dirname(@root)
    FileUtils.cp_r sandbox_src, @root, :preserve => true
  end   

  def install_abaeterno_in
    debug2 "installing abaeterno.in"
    File.open(data('abaeterno.in'),"w") do |f|
      f << "root='#{$here}'\n"
      f << "cotson_pid=#{$$}\n"
      f << "control_script_file='sandbox-'..cotson_pid..'-ctrl'\n"
      f << @lua_defs.to_str
    end
  end

  def install_simnow_pre
    debug2 "installing simnow.pre"
    File.open(data('simnow.pre'),"w") do |f|
      f << @lua_defs.simnow_pre
    end
  end

  def install_simnow_pre_cluster
    debug2 "installing simnow.pre (for cluster node)"
    File.open(data('simnow.pre'),"w") do |f|
      f << @lua_defs.simnow_pre_cluster
    end
  end

  def install_node_config
    if @opts[:node_config]!=""
        debug2 "installing user node configuration"
        uscript=@opts[:node_config]
        uscript1,uscript2=uscript.split(/\s/,2)
        debug2 "uscript1 #{uscript1}   uscript2 #{uscript2}"
        FileUtils.cp_r(uscript1,data('node_config'))
    end
  end

  def install_user_script
    debug2 "installing user script"
    uscript=@opts[:user_script]
    uscript1,uscript2=uscript.split(/\s/,2)
    debug2 "uscript1 #{uscript1}   uscript2 #{uscript2}"
    FileUtils.cp_r(uscript1, data('user_script'))
  end

  def install_run_sh
   if @opts[:run_script]!=""
    debug2 "installing run script"
    uscript=@opts[:run_script]
    uscript1,uscript2=uscript.split(/\s/,2)
    debug2 "uscript1 #{uscript1}   uscript2 #{uscript2}"
    FileUtils.cp_r(uscript1, data('run_script'))
    debug2 "installing run.sh"
    File.open(data('run.sh'),"w") do |f|
      f.puts "xget ../data/run_script run_script"
      f.puts "chmod +x run_script"
      f.puts "./run_script #{uscript2} 2>&1 | tee stdout.log"
      f.puts "xput stdout.log #{@root}/data/stdout.log"
      f.puts "touch l; xput l terminate"
      s_id=0
      a = @opts[:subscribe_result]
      if a.respond_to?(:lines) then
        @opts[:subscribe_result].lines.each do |s|
          f.puts "xput #{s.chomp} #{@root}/data/res_#{s_id}.#{File.basename(s)}" 
          s_id += 1
        end
      else
        @opts[:subscribe_result].each do |s|
          f.puts "xput #{s.chomp} #{@root}/data/res_#{s_id}.#{File.basename(s)}" 
          s_id += 1
        end
      end
    end
   end
  end

  def install_cluster_sh
    debug2 "installing cluster.sh"
    uscript=@opts[:user_script]
    uscript1,uscript2=uscript.split(/\s/,2)
    debug2 "uscript1 #{uscript1}   uscript2 #{uscript2}"
    uscript=@opts[:node_config]
    uscript3,uscript4=uscript.split(/\s/,2)
    debug2 "uscript3 #{uscript3}   uscript4 #{uscript4}"
    
    File.open(data('cluster.sh'),"w") do |f|
      f.puts "xget ../data/node_config node_config"
      f.puts "xget ../data/user_script user_script"
      f.puts "chmod +x node_config user_script"

      f.puts "sudo ./node_config #{@opts[:NODE]} #{@opts[:TOTAL]} #{uscript4} 2>&1 | tee node_config.log"
      f.puts "xput node_config.log #{@root}/data/node_config.log"

      f.puts "./user_script #{@opts[:NODE]} #{@opts[:TOTAL]} #{uscript2} 2>&1 | tee stdout.log"
      f.puts "xput stdout.log #{@root}/data/stdout.log"
      s_id=0
      a = @opts[:subscribe_result]
      if a.respond_to?(:lines) then
        @opts[:subscribe_result].lines.each do |s|
          f.puts "xput #{s.chomp} #{@root}/data/res_#{s_id}.#{File.basename(s)}" 
          s_id += 1
        end
      else
        @opts[:subscribe_result].each do |s|
          f.puts "xput #{s.chomp} #{@root}/data/res_#{s_id}.#{File.basename(s)}" 
          s_id += 1
        end
      end
      f.puts "touch l; xput l terminate"
    end
  end

  def install_simnow_dir
    simnow_dir=@opts[:simnow_dir]
    link=@opts[:simnow_dir_link]
    debug2 "installing simnow_dir"
    debug2 " from #{simnow_dir}"
    debug2 " using #{link ? 'link' : 'copy'}"
    %w{ simnow productfile linuxlibs analyzers libs Images icons }.each do |x|
      y=File.join simnow_dir, x
      if !File.exist?(y)
        raise CotsonError.new(:SandboxInstallSimNowDir, :simnow_location=>simnow_dir, :component=>x, :link =>link, :sandbox=>$here)
      end
      link ? FileUtils.ln_sf(y,share) : FileUtils.cp_r(y,share)
    end
  end

  def install_abaeterno_so
    f=File.expand_path(@opts[:abaeterno_so])
    aaso = share('abaeterno.so')
    link=@opts[:abaeterno_so_link]
    raise CotsonError.new(:SandboxInstallAbaeterno, :source=>f, :link=>link, :destination=>aaso, :sandbox=>"#{$here}") if !File.exist?(f)
    debug2 "installing abaeterno.so"
    debug2 " from #{f}"
    debug2 " using #{link ? 'link' : 'copy'}"
    link ? FileUtils.ln_sf(f,aaso) : FileUtils.cp_r(f, aaso)
  end

  def install_injector
    f=@opts[:injector]
    inj = share('injector')
    link=@opts[:injector_link]
    debug2 "installing aaInjector"
    debug2 " from #{f}"
    debug2 " using #{link ? 'link' : 'copy'}"
    link ? FileUtils.ln_sf(f,inj) : FileUtils.cp_r(f, inj)
  end

  def install_mediator
    f=File.expand_path(@opts[:mediator_file])
    med = share('mediator')
    link=@opts[:mediator_link]
    debug2 "installing mediator"
    debug2 " from #{f}"
    debug2 " using #{link ? 'link' : 'copy'}"
    link ? FileUtils.ln_sf(f,med) : FileUtils.cp_r(f, med)
  end

  def install_bsd
  end

  def install_hdd
  end

  def install_config
    debug2 "installing config"
    @opts.save(data('config'))
  end

  def install_db
    debug2 "installing database"
    File.open(data('database'),'w' ) { |x| YAML.dump(Hash.new, x ) }
  end
  
  def install_test_data
    files_to_copy = []
    [
      'filters',
      'abaeterno_output*',
      'injector_output*',
      'simnow_output',
      'disasm',
      'simpoint_profile',
      'disksim_*',
      'atlas10k.*',
      'statdefs',
      'disktrace_*',
    ].each do |x|
      files_to_copy+=@opts[:test].files(x)
    end
    FileUtils.cp files_to_copy.flatten, data
  end

  public
  def initialize(ld,opts,name=nil)
    @lua_defs=ld
    @opts=opts
    super(name ? name : Location.tempfile(opts[:runid]))
  end

  def install(*args)
    create
    args << :config << :db
    args.each { |x| self.send('install_'+x.id2name) }
  end

  def read_config
    debug2 "reading config"
    @opts=Options.new(nil)
    @opts.load_from_file(data('config'))
  end

  def run(*args)
    command_name=args.shift
    command=bin(command_name)
    raise CotsonError.new(:SandboxRun,:sandbox=>$here, :command=>command_name, :script_location=>command, :arguments => "#{args.join(' ')}") if !File.exist?(command)
    debug2 "running #{command} #{args.join(' ')} (and printing)"
    Execute.run_and_print("#{command} #{args.join(' ')}") 
    debug2 "running #{command} ended"
  end

  def run_output(*args)
    command_name=args.shift
    command=bin(command_name)
    raise CotsonError.new(:SandboxRunOutput,:sandbox => $here, :command=>command_name, :script_location=>command, :arguments => "#{args.join(' ')}") if !File.exist?(command)
    debug2 "running #{command} #{args.join(' ')}"
    a=Execute.run("#{command} #{args.join(' ')}")
    debug2 "running #{command} ended" 
    [a[:status],a[:output]]
  end

  def run_detached(*args)
    command_name=args.shift
    command=bin(command_name)
    raise CotsonError.new(:SandboxRunOutput,:sandbox => $here, :command=>command_name, :script_location=>command, :arguments => "#{args.join(' ')}") if !File.exist?(command)
    debug2 "running detached #{command} #{args.join(' ')}"
    a=Execute.run("#{command} #{args.join(' ')}", :mode => 'detach')
    debug2 "running detached #{command} ended" 
    [a[:status]]
  end

  def kill
    begin 
      i,o=run_output("kill")
      debug2 "sandbox kill: #{o}"
      return i==0

      rescue Exception
        return false
    end
  end

  def status
    begin 
      i,o=run_output("status")
      debug2 "sandbox status: #{o}"
      return i==0

      rescue Exception
        return false
    end
  end
  
  def start_go
    i,o=run_output("start_go")
    raise CotsonError.new(:SandboxStartGo,:command=>"start_go", :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
  end

  def set_mediator_value(y,z)
    i,o=run_output("set_mediator_value",y,z)
    raise CotsonError.new(:SandboxSetMediatorValue,:command=>"set_mediator_value", :key=>y, :value=>z, :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
  end

  def get_mediator_value(x)
    i,o=run_output("get_mediator_value",x)
    raise CotsonError.new(:SandboxGetMediatorValue,:command=>"get_mediator_value", :key=>x, :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
    return o
  end

  def get_vnc_webport
    i,o=run_output("get_vnc_webport")
    raise CotsonError.new(:SandboxGetMediatorValue,:command=>"get_vnc_webport", :key=>x, :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
    return o
  end

  def mediator
    i,o=run_output('mediator')
    raise CotsonError.new(:SandboxMediator,:command=>"mediator", :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
    return o
  end

  def clean(force=false)
    begin
      if force || @opts[:clean_sandbox]
        debug2 "about to clean sandbox #{to_str}"
        run('clean')
      else
        debug2 "sandbox left in #{to_str}"
      end
    rescue
    end
  end
  
  def copy_files(params)
    cmd=""
    cmd+="-d #{params[:destination]} " if params[:destination]
    cmd+="-t #{params[:type]} "        if params[:type]
    cmd+="-f '#{params[:filter]}' "    if params[:filter]
    cmd+="-p #{params[:prefix]} "      if params[:prefix]
    i,o=run_output("copy_files", cmd)
    raise CotsonError.new(:SandboxCopyFiles,:command=>"copy_files #{cmd}", :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
  end

  def list_files(params)
    cmd=""
    cmd += "-t #{params[:type]} "     if params[:type]
    cmd += "-f '#{params[:filter]}' " if params[:filter]
    i,o=run_output("list_files",cmd)
    raise CotsonError.new(:SandboxListFiles,:command=>"list_files #{cmd}", :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
  end

  def consoleX(params)
    cmd=""
    cmd += "-quiet" if params[:quiet] 
    i = run_detached("consoleX",cmd)
    return "#{i}"
  end
  
  def view(params)
    cmd=""
    cmd += "-quiet" if params[:quiet] 
    i = run_detached("view",cmd)
    return "#{i}"
  end

  def stop
    i,o=run_output("stop")
    raise CotsonError.new(:SandboxStop,:command=>"stop", :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
  end
  
  def start
    i,o=run_output("start")
    raise CotsonError.new(:SandboxStart,:command=>"start", :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
  end

  def ngo
    i,o=run_output("ngo")
    raise CotsonError.new(:SandboxNgo,:command=>"ngo", :status=>i, :output=>o.chomp, :location=>"#{@root}") if i != 0
  end
end
