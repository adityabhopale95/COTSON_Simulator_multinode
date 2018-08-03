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

# $Id: control_script.rb 723 2015-06-03 23:13:54Z paolofrb $

class ControlScript
  def initialize
    @default_args = {"consoleX" => {:quiet=>true} }
    @commands = ["list", "help", "status", "consoleX", "kill", "clean", "view", "stop", "start_go"]
    @mediator = nil
    @nodes = []
  end
  
  def add(sandbox, mediator=false)
    if mediator then
      @mediator = sandbox if sandbox.kind_of?(Location) || sandbox.kind_of?(DaemonServiceCall)
      return
    end
    if sandbox.kind_of?(Location) || sandbox.kind_of?(DaemonServiceCall) then
      @nodes << sandbox
      return
    end
    #TODO: Add a cotson error instead
    raise "Not a sandbox"
  end
  
  def script_sandbox(sandbox, mediator=false)
    if sandbox.kind_of?(Location)
      return "cs.add(Sandbox.new(nil,nil,'#{sandbox}'), #{mediator})" 
    end
    sandbox.to_s =~ /(.*)<(\d+)>$/
    return "cs.add(DaemonServiceCall.new('#{$1}',#{$2}), #{mediator})"
  end
  
  def save(filename)
    @file=File.expand_path(filename)
    
    File.open(@file,"w") do |f|
      f.puts "#!/usr/bin/ruby -w"
      f.puts "# (C) Copyright 2009 Hewlett-Packard Development Company, L.P."
      f.puts "# Experiment control script generated at #{Time.now}"
      f.puts
      f.puts "$: << \"#{File.expand_path(File.dirname(__FILE__) + '/../share')}\""
      f.puts "require 'cotson'"
      f.puts "require 'control_script'"
      f.puts "require 'daemon_service'"
      f.puts
      f.puts "begin" 
      f.puts "  cs = ControlScript.new"
      f.puts "  #{script_sandbox(@mediator, true)}" 
      @nodes.each { |n| f.puts("  #{script_sandbox(n)}") }
      f.puts "  cs.command(ARGV)"
      f.puts "rescue Exception => e"
      f.puts "  finish e"
      f.puts "end"
    end
    File.chmod(0755, @file)
  end
  
  def destroy
    File.delete(@file) if File.exist?(@file)
  end
  
  
  def command(args)
    if args.size == 0 then
      self.list 
      return
    end
    cmd = args.delete_at(0)
    if @commands.find { |command| command == cmd } 
      self.send cmd, args
      return
    end
    puts "Error: '#{cmd}' is not a valid command"
    help
  end
  
#  private
  def list_sandbox_commands
    path = File.dirname(__FILE__) + "/../sandbox/bin"
#    puts path
    d=[]
    Dir.glob(path + "/*").each do |f|
      d<< "#{File.basename(f)}"
    end
    d.sort
  end
  
  def method_missing(method, *args)
    help
    return
  end
  
  def remote_execute(sandbox, method, args)
    if @default_args[method.to_s] then
      args=[] if ! args 
      args = @default_args[method.to_s] + args
    end
#    puts "Executing #{method} with #{args.join(", ")}"
    sandbox.send method, args 
  end
  
  def selected(parms)
    sel = (parms == nil || parms.size ==0) ? "all" : parms[0].downcase
    return [@mediator] + @nodes if sel=='all' && @mediator
    return @nodes if sel=='all' 
    return @nodes if sel == 'nodes'
    return [@mediator] if sel == 'm'
    return [@nodes[sel.to_i]]
  end
  
  def sandbox_desc(sandbox)
    return "mediator" if sandbox == @mediator
    "node #{@nodes.index(sandbox)}"
  end
  
  def sandbox_mediator?(sandbox)
    return true if sandbox == @mediator
    false
  end
  
  def sandbox_remote?(sandbox)
    sandbox.to_s =~ /.*<\d+>$/
  end

  #COMMANDS THAT WERE ENABLED
  def help(params=nil)
    puts "Usage: #{File.basename($0)} [command] [nodeID|'nodes'|'all']"
    puts "                            'all' is default"
    puts "Commands: #{@commands.sort.join(", ")}"
  end
  
  def list(params=nil)
    puts "ID - Description - Status - Sandbox"
    puts "m - mediator - #{@mediator.status ? "running" : "not running"} - #{@mediator}" if @mediator
    @nodes.each_index do |i|
      puts "#{i} - Node #{i} - #{@nodes[i].status ? "running" : "not running"} - #{@nodes[i]}"
    end
  end
  
  def status(parms)
    selected(parms).each do |sb|
      res = sb.status
      puts "#{sandbox_desc(sb)} - #{res ? "running" : "not running"}"
    end
  end
  
  def consoleX(parms)
    selected(parms).each do |sb|
      if !sandbox_mediator?(sb)
        puts "#{sandbox_desc(sb)}"
        if sandbox_remote?(sb)
          res = sb.get_vnc_webport
          if res =~ /rxvt_display_http=(\d+)/
            port = $1.to_i - 5800
            sb.to_s =~ /(.*):/
            server = $1
            Execute.run_detached("vncviewer #{server}:#{port}")
          end
        else
          sb.consoleX(Hash.new)
        end
      end
    end
  end
  
  def kill(parms)
    selected(parms).each do |sb|
      begin
        res = sb.kill
#      rescue Exception => e   # RG -- reduce warnings
      rescue Exception
      end
    
      puts "--- #{sandbox_desc(sb)} ---"
      puts res
      puts "--------------"
    end
  end
  
  def clean(parms)
    selected(parms).each do |sb|
      if sandbox_remote?(sb)
        sb.clean
      else
        sb.clean(true) 
      end
      puts "#{sandbox_desc(sb)}"
    end
  end
  
  def view(parms)
    selected(parms).each do |sb|
      if !sandbox_mediator?(sb)
        puts "#{sandbox_desc(sb)}"
        if sandbox_remote?(sb)
          res = sb.get_vnc_webport
          if res =~ /display_http=(\d+)/
            port = $1.to_i - 5800
            sb.to_s =~ /(.*):/
            server = $1
            Execute.run_detached("vncviewer #{server}:#{port}")
          end
        else
          sb.view(Hash.new)
        end
      end
    end
  end
  
  def stop(parms)
    selected(parms).each do |sb|
      res = sb.stop
    
      puts "--- #{sandbox_desc(sb)} ---"
      puts res
      puts "--------------"
    end
  end

  def start(parms)
    selected(parms).each do |sb|
      res = sb.start
    
      puts "--- #{sandbox_desc(sb)} ---"
      puts res
      puts "--------------"
    end
  end
  
  def start_go(parms)
    selected(parms).each do |sb|
      res = sb.start_go
    
      puts "--- #{sandbox_desc(sb)} ---"
      puts res
      puts "--------------"
    end
  end
end
