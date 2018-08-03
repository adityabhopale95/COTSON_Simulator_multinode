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

# $Id: execute.rb 6977 2009-12-09 16:18:35Z frb $

require 'tmpdir'
require 'fileutils'

class Hash
  def add_default(key,value)
    if !self.has_key?(key) then
      self[key]=value
    end
  end
end

class Execute
  def Execute.run(command,options={})
    options.add_default :mode, 'simple'
    options.add_default :merge_stderr, true

    options.add_default :print_output, true
    options.add_default :save_output, false

    mode=options[:mode]

    case mode
      when 'simple'
        command="(#{command}) 2>&1 " if options[:merge_stderr]
        output=%x{#{command}}
        { :output => output, :status => $?.exitstatus, :pid => $?.pid }
      when 'pipe'
        command="(#{command}) 2>&1 " if options[:merge_stderr]
        po=options[:print_output]
        so=options[:save_output]
        if so then
          if !options.has_key?(:save_file) then
            raise CotsonError.new(:ExecuteRun, :option=>"save_file", :command=>command, :mode => mode, :print_output => po, :save_output=>so)     
          end
          save=File.open(options[:save_file],"w")
        end
        IO.popen(command).each_line do |line|
          if po then
            puts(line)
            $stdout.flush
          end
          if so then
            save.puts(line)
            save.flush
          end
        end
        save.close if so
        {}
      when 'interactive'
        Process.wait(Process.fork { exec(command)})
        { :status => $?.exitstatus }
      when 'background'
        command+=" > #{options[:save_file]} " if options[:save_file]
        command +=" 2>&1 " if options[:merge_stderr] && options[:save_file]
#RG (should work also with RUBY_VERSION < 1.8  (TO BE TESTED)
#          if "#{RUBY_VERSION}" &lt; "1.9.0" then
#             pid=%x{#{command} & echo \$!}.to_a[-1].to_i
#          else
             pid=%x{#{command} & echo \$!}.lines.to_a[-1].to_i
#          end
        { :pid => pid }
      when 'detach'
        pid=Process.fork { exec(command) }
        Process.detach(pid)
        { :pid => pid }
      when 'screen'
        sd=options[:screen_dir]
        if sd then
          FileUtils.mkdir_p(sd)
          FileUtils.chmod 0700, sd
          ENV['SCREENDIR']=sd
        end
        if !options.has_key?(:screen_exe) then
          raise CotsonError.new(:ExecuteRun, :option=>"screen_exe", :screen_dir=>sd, :mode => mode)
        end
        if !options.has_key?(:screen_conf) then
          raise CotsonError.new(:ExecuteRun, :option=>"screen_conf", :screen_dir=>sd, :mode => mode)
        end
        command="#{options[:screen_exe]} -L -c #{options[:screen_conf]} "+
          " -dmS sessionname #{command}"
        output=%x{#{command}}
        status=$?.exitstatus
#RG-4
#        so=%x{#{options[:screen_exe]} -ls}.
#          split.
#          find { |x| x =~ /^[0-9]+\.sessionname$/ }.
#          map { |x| x.gsub(/^([0-9]+)\.sessionname$/,'\\1') }
#RG+8 patch: sometimes we fond something that is not an Array but it is a string (therefore no method 'map')
        so1=%x{#{options[:screen_exe]} -ls}.
          split.
          find { |x| x =~ /^[0-9]+\.sessionname$/ }
        if so1.kind_of?(Array) then
          so=so1.map { |x| x.gsub(/^([0-9]+)\.sessionname$/,'\\1') }
        else
          so=[ so1.gsub(/^([0-9]+)\.sessionname$/,'\\1') ]
        end
        { :output => output, :status => status, :pid => so[-1]}
    else
      raise CotsonError.new(:ExecuteRun, :mode=>mode)
    end
  end

  def Execute.run_and_print(command)
    run(command, :mode => 'pipe')
  end

  def Execute.run_and_save(command,save_file)
    run(command, :mode => 'pipe', :print_output => false,
      :save_output => true, :save_file => save_file)
  end

  def Execute.run_interactive(command)
    run(command, :mode => 'interactive')
  end

  def Execute.run_background(command,save_file=nil)
    run(command, :mode => 'background', :save_file => save_file)
  end

  def Execute.run_detached(command)
    run(command, :mode => 'detach')
  end

  def Execute.run_in_display(command,display)
    display_str=VNC.display_for_ENV(display)
    ENV['DISPLAY']=display_str
    a=run_detached(command)
    a[:display]=display
    a[:display_str]=display_str
    a
  end

  def Execute.runs?(pid)
    return false if ! pid
    %x{ps -p #{pid}}   # RG -- reduce warnings
    $?.exitstatus == 0
  end

  def Execute.runs_any?(*args)
    args.any? { |x| Execute.runs? x }
  end

  def Execute.kill(*args)
    args.each do |pids|
      if pids then
        pid=pids.to_i
        Process.kill("TERM", pid ) if Execute.runs?(pid)
        Process.kill("KILL", pid ) if Execute.runs?(pid)
      end
    end
  end
end
