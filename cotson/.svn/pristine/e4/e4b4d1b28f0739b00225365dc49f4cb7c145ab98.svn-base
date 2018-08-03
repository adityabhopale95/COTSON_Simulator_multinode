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

# $Id: simnow.rb 6977 2009-12-09 16:18:35Z frb $

require 'execute'
require 'vnc'
require 'filecopy'
#RG+1
require 'fileutils'

class Simnow
	def Simnow.interactive
		if CF[:interactive] then
			debug2 ' interactively'
			""
		else
			debug2 ' non-interactively'
			" -n -c -d"
		end
	end

	def Simnow.functional
		if !CF[:functional] then
			debug2 ' with timing'
			debug2 ' using abaeterno.in'
			ai=$here.data('abaeterno.in')
			aei=$here.data('abaeterno.extra.in')
			if File.exist?(aei)
				debug2 ' using abaeterno.extra.in'
				ai="'#{ai} #{aei}'"
			end
			" --Monitor --ALd 8 --ADir . --AArg "+ai
		else
			debug2 ' without timing (functional)'
			""
		end
	end

	def Simnow.input_file
		debug2 ' generating simnow.in from simnow.pre'
		simnow_in=$here.data('simnow.in')
		simnow_pre=$here.data('simnow.pre')	
		File.open(simnow_in,"w") do |f|
			File.open(simnow_pre).each_line do |x|
				if x =~ /MEDIATOR/ then
					debug2 '  setting mediator'
					med_host=DB.need('host')
					med_port=DB.need('control_port')
					x.gsub!(/MEDIATOR/,"#{med_host}:#{med_port}")
				end
				f << x
			end
			if CF[:run] then
				f << "go\n"
				debug2 '  setting go'
			end
		end
		simnow_in
	end

	def Simnow.extra_input_file
		File.open($here.data('abaeterno.extra.in'),"w") do |f|
			f.puts "options.heartbeat.logfile='../data/timer.log'"
			f.puts "options.cmdfile='../data/command_port'"
			f.puts "options.exit_trigger='terminate'"
      f.puts "options['network.mediator_nodeid']='#{CF[:NODE]}'"
			%w{host control_port multicast_ip 
				multicast_port quantum_max}.each do |x|
				f.puts "options['network.#{x}']='#{DB.need(x)}'"
			end
		end
	end

  def Simnow.one_node_extra_input_file
    File.open($here.data('abaeterno.extra.in'),"w") do |f|
      f.puts "if options.heartbeat.logfile==nil then"
      f.puts "   options.heartbeat.logfile='../data/timer.log'"
      f.puts "else"
      f.puts "   options.heartbeat.logfile='../data/'..options.heartbeat.logfile"
      f.puts "end"
    end
  end

  def Simnow.run
		debug2 'preparing simnow execution'
		simnow_command="./simnow -e "+Simnow.input_file
		simnow_command+=Simnow.interactive
		simnow_command+=Simnow.functional
		simnow_command+=" -i #{CF[:data_dir]}"

		FileUtils.cd($here.share) do
			display_strategy=CF[:display]
			case display_strategy
				when "auto"
					vnc_pid,display=VNC.start_vnc
					force_display=true
					debug2 " starting automatic display #{display}"
				when "none"
					force_display=false
					debug2 " using default display (if needed)"
				else
					force_display=true	
					display=display_strategy
					debug2 " using given display #{display}"
			end

			if force_display then
				display_str=VNC.display_for_ENV(display)
				ENV['DISPLAY']=display_str
			end

			print_output=CF[:print_output]
			save_output=CF[:save_output]
			
			if CF[:use_screen] then
				debug2 " running inside screen"
				mode='screen'
			else
				mode='pipe'
				debug2 " running directly (printing=#{print_output}, saving=#{save_output})"
			end
			$here.mark_input
			a=Execute.run(simnow_command,
				:mode => mode,
				:screen_dir => $here.share('screen_dir'),
				:screen_conf => $here.data('screen.rc'),
				:screen_exe => CF[:screen],
				:print_output => print_output,
				:save_output => save_output,
				:save_file => $here.data("simnow.out"))
			if force_display then
				a[:vnc_pid]=vnc_pid if vnc_pid
				a[:display]=display
				a[:display_str]=display_str
				a[:display_http]=VNC.display_for_http(display)
			end
			return a
		end
  end

	def Simnow.gdb
		debug2 'preparing gdb-simnow execution'
		simnow_args="-e "+Simnow.input_file
		simnow_args+=Simnow.interactive
		simnow_args+=Simnow.functional

		debug2 ' generating gdb.in'
		gdb_in=$here.data('gdb.in')
		File.open(gdb_in,"w") do |f|
			f.puts "define segv-on";
			f.puts "handle SIGSEGV pass stop print";
			f.puts "end";
			f.puts "define segv-off";
			f.puts "handle SIGSEGV pass nostop noprint";
			f.puts "end";
			f.puts "b main";
			f.puts "segv-off";
			f.puts "run #{simnow_args}";
		end

		$here.mark_input
		debug2 ' running inside gdb'
		FileUtils.cd($here.share) do
			Execute.run_interactive("gdb ./simnow -x "+gdb_in)
		end
	end

	def Simnow.injector
		debug2 'preparing injector execution'
		ENV['DATA_DIR']=CF[:data_dir];
		injector_command=" ./injector ../data/abaeterno.in"

		print_output=CF[:print_output]
		save_output=CF[:save_output]

		$here.mark_input
		debug2 " running injector (printing=#{print_output}, saving=#{save_output})"
		FileUtils.cd($here.share) do
			Execute.run(injector_command,
				:mode => 'pipe',
				:print_output => print_output,
				:save_output => save_output,
				:save_file => $here.data("injector.out"))
		end
	end

	def Simnow.mediator
		debug2 'preparing mediator execution'
		mediator_command=" ./mediator #{$here.data("abaeterno.in")} \"mediator.autoscan_ports=true\" \"mediator.save_config_file='../data/mediator_config'\" \"mediator.heartbeat.logfile='../data/timer.log'\""
		save_output = CF[:save_output]
		merge_stderr = CF[:merge_stderr]

		$here.mark_input
		debug2 " running mediator (merging=#{merge_stderr}, saving=#{save_output})"
		FileUtils.cd($here.share) do
			return Execute.run(mediator_command,
				:mode => 'background',
				:save_output => save_output,
				:merge_stderr => merge_stderr,
				:save_file => $here.data("mediator.log"))
		end
	end
end
