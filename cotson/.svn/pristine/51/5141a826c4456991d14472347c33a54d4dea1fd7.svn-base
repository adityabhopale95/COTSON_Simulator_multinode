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

# $Id: error.rb 6977 2009-12-09 16:18:35Z frb $

## Fancy...
def colored_text1(bg, fg, st)
  esc="\033["

  "#{esc}#{fg};#{bg}m#{st}#{esc}0m"
end

## Boring...
def colored_text(bg, fg, st) 
   "#{st}"
end

class ErrorMessage
  def ErrorMessage.message(location, errorcode, exception, info)
    info = info ? info : {}
    if errorcode
      information(location,errorcode,exception,info) + solution(errorcode,info) + output(info) + extended_information(info)
    else
      information(location,errorcode,exception,info) + output(info) + extended_information(info) + original_exception(exception) + backtrace_info(exception)
    end
  end

  def ErrorMessage.information(location, errorcode, exception, info)
    location = location ? " at #{location}" : ""
    title="Error#{location}"
    message="No exception or description for this error"
    if errorcode && @@errors[errorcode] then
      title="Error #{errorcode}:"
      message= @@errors[errorcode][:message] ? format_message(@@errors[errorcode][:message], info) : "Error in database but no message found for this error."
    else
      if errorcode
        title="Error #{errorcode}#{location}:"
        message="Error identified as #{errorcode} but no description/solution found in database.\nException:#{exception}"
      else
        if exception then
          title = "Exception #{exception.class} ocurred#{location}" 
          message = "#{exception}" 
        end
      end
    end
    colored_text(CODE_BG, CODE_FG,"#{title}\n") + "#{message}\n"
  end

  def ErrorMessage.solution(errorcode, info)
    if errorcode && @@errors[errorcode]
      if @@errors[errorcode][:solution] then
        return colored_text(CODE_BG, CODE_FG,"Possible solution:") + "\n" + format_message(@@errors[errorcode][:solution], info) + "\n"
      end
    end
    ""
  end
  
  def ErrorMessage.output(info)
    if info[:output] then
      output = info.delete(:output)
      output=output.join("") if output.instance_of?(Array)
      output = "   " + output.strip.gsub(/\n/,"\n   ") 
      colored_text(CODE_BG, CODE_FG,"Command output:") + "\n#{output}\n"
    else
      ""
    end
  end
  
  def ErrorMessage.extended_information(info)
    return "" if info.size == 0
    e = colored_text(CODE_BG, CODE_FG,"Extended information:") + "\n"
    info.each_pair do |key, value|
      e += "#{key} = [#{value.to_s.chomp}]\n" if key != :exception
    end
    e
  end
  
  def ErrorMessage.backtrace_info(exception)
    if exception && exception.backtrace
      colored_text(CODE_BG, CODE_FG,"Backtrace:") + "\n#{exception.backtrace.join("\n")}"  
    else
      ""
    end
  end

  def ErrorMessage.original_exception(exception)
    if exception then
      colored_text(CODE_BG, CODE_FG,"Exception: #{exception.class}") + "\n#{exception}\n"
    else
      ""
    end
  end

  def ErrorMessage.format_message(message, info)
    res=message.clone
    info.each_pair do |key, value| 
      res.gsub!("{{#{key.id2name}}}",colored_text(HIGHLIGHT_BG,HIGHLIGHT_FG,value))
    end
#    while res =~ /\[\[([\w\s\.\/\'\_\=\{\}\@\-_]+)\]\]/ do  #RG -- reduce warnings
    while res =~ /\[\[([\w\s\.\/\'\=\{\}\@\-]+)\]\]/ do
      res.gsub!("[[#{$1}]]",colored_text(HIGHLIGHT_BG,HIGHLIGHT_FG,$1))
    end
    res
  end

  ErrorMessage::CODE_BG = 1
  ErrorMessage::CODE_FG = 31
  ErrorMessage::HIGHLIGHT_BG = 1
  ErrorMessage::HIGHLIGHT_FG = 33
  ErrorMessage::MSG_BG = 0
  ErrorMessage::MSG_FG = 34

  @@errors= { 
    :AbaeternoNotFound => {:message => "[[abaeterno.so]] library was not found", :solution => "Link/copy [[abaeterno.so]] to the current location or set at your lua configuration file the [[abaeterno_so]] variable to point to the [[abaeterno.so]] file. Example: [[abaeterno_so='/home/user/abaeterno.so']]" },
    :BSDNotFound => {:message => "[[use_bsd]] was not found on lua file or the referenced [[bsd]] file was not found", :solution => "Check your lua if there is the [[use_bsd]] function call inside of [[simnow.commands]]. Check if the [[bsd]] file pointed out by [[use_bsd]] exists and is well referenced.\nTo reproduce the error with lua execute '{{lua}} {{script}} {{luainput}}'." },
    :CompareWithoutProduced => {:message => "Could not compare reference result with produced result because the result file {{produced}} was not found"},
    :ComparisonMismatch => {:message => "The reference file {{real}} does not match with the test produced file {{produced}}, check with:\ndiff -w {{real}} {{produced}}\n"},
    :ConnectionRefused => {:message => "The connection with the host {{host}} port {{port}} was refused while attempting a HTTP {{type}}. It is very likely that the daemon service was not started for the host {{host}}:{{port}}.", :solution => "Start the daemon in the host {{host}} using the port {{port}} or point the execution to the correct host:port using your configuration file and setting the [[scheduler]] lua table."},
    :DaemonAlreadyOn => {:message => "The daemon is already running for the host {{host}}:{{port}} at the directory {{directory}}. The process pid is {{pid}}", :solution => "If you can not connect to the daemon at {{host}}:{{port}}, kill the process with pid {{pid}} if it is running, delete the file {{file}} and start the daemon again."},
    :DaemonAlreadyOff => {:message => "The daemon is not running for the host {{host}} at the directory {{directory}}."},
    :DisplayNotAvalilabe => {:message => "COTSon could not find a display available to start the VNC program. All displays between {{min_display}} and {{max_display}} were locked.\nThe VNC program being used was {{program}}"},
    :FileNotFound => {:message => "The file {{file}} was necessary to the execution and could not be found." },
    :HDDNotFound => {:message => "[[use_hdd]] was not found on lua file or the referenced [[hdd]] file was not found", :solution => "Check your lua if there is the [[use_hdd]] function call inside of [[simnow.commands]]. Check if the [[hdd]] file pointed out by [[use_hdd]] exists and is well referenced.\nTo reproduce the error with lua execute '{{lua}} {{script}} {{luainput}}'." },
    :HTTPCommunicationError => {:message => "A communication error happened while communicating to host {{host}}:{{port}}. The type of error was {{response_type}}.\nError message:\n{{body}}" },
    :InconsistentDaemonPID => {:message => "It was not possible to stop the daemon because the process with pid {{pid}} was not found. It is possible that the daemon is still running.", :solution => "Kill any [[daemon-start]] process if there is any running and delete the file {{file}}" },
    :LuaNotExecutable => {:message => "The lua interpreter {{file}} is not set as executable in the system.", :solution => "Set the file {{file}} as executable" },
    :LuaParsing => {:message => "Error parsing configuration file with lua (it is very likely a syntax error).\nCOTSon was attempting to get a value for the variable {{variable}}", :solution => "Check your lua file for syntax errors and check if the files passed as parameter to COTSon exist.\nTo reproduce the error with lua execute '{{lua}} {{script}} {{luainput}}'." },
    :LuaVariableNotDefined => {:message => "Could not obtain the value of variable {{variable}} using lua", :solution => "Set a value for the variable {{variable}} in your lua file" },
    :MediatorBadLink => {:message => "File {{mediator_original}} was set as [[mediator]] program but it does not exist", :solution => "Link/copy [[mediator]] to the current location or set at your lua configuration file the [[mediator_file]] variable to point to the [[mediator]] program. Example: [[mediator_file='/home/user/mediator']]\nIf [[mediator_file]] is properly set check if it points out to a valid [[mediator]] executable." },
    :MediatorFinished => {:message => "A call to [[mediator]] was done but the [[mediator]] process was already finished. The [[mediator]] parameters were not set correctly or the [[mediator]] program might have crashed.\nThe [[mediator]] execution output was:\n{{mediator_output}}", :solution => "Check if you declared correctly the [[mediator={...}]] table in the simulation configuration file.\nCheck the execution output and COTSon documentation to solve the problem." },
    :MediatorFinishedNoOutput => {:message => "A call to [[mediator]] was done but the [[mediator]] process was not running. The [[mediator]] program might have crashed or might not have been executed.", :solution => "Try to execute the mediator passing your lua file as a parameter and check its responsiveness to find out the problem" },
    :MediatorNotExecutable => {:message => "The [[mediator]] file was not executable.", :solution => "Assure that you have set the proper [[mediator]] to the execution and that the file has execution properties and you have execution rights." },
    :MediatorNotFound => {:message => "[[mediator]] program was not found", :solution => "Link/copy [[mediator]] to the current location or set at your lua configuration file the [[mediator_file]] variable to point to the [[mediator]] program. Example: [[mediator_file='/home/user/mediator']]\nIf [[mediator_file]] is properly set check if it points out to a valid [[mediator]] executable." },
    :MediatorNotRunning => {:message => "[[mediator]] program is not running. The assigned mediator might not be an executable, not exist or might have crashed."},
    :NoCriteria => {:message => "No selection criteria was passed to choose the regression tests to be executed", :solution => "Pass as argument a test number (ex: [[regression-test 001]]) or a filter criteria (ex: [[regression-test @all]])." },
    :NoHosts => {:message => "No hosts were set for the scheduler.", :solution => "Be sure to set the scheduler hosts with valid daemons." },
    :NoInitialArguments => {:message => "[[COTSon]] was executed without any lua command or file", :solution => "Pass your lua simulation configuration file or commands to COTSon" },
    :NoSchedulerPolicy => {:message => "No policy was set for the scheduler", :solution => "Make sure to set a valid policy for the scheduler. The existent policies are {{policy_list}}." },
    :NoTestSelected => {:message => "No regression test was selected for the criteria \'{{arguments}}\'", :solution => "Pass as argument a test number (ex: [[regression-test 001]]) or a filter criteria (ex: [[regression-test @all]])." },
    :NotInAbaeternoDir => {:message => "Attempt of running a regression test out of abaeterno directory", :solution => "Go to the abaeterno directory and run the regression test." },
    :SandboxInstallAbaeterno => {:message => "File {{source}} was set as [[abaeterno.so]] library but the file does not exist or could not be found", :solution => "Link/copy [[aabeterno.so]] to the current location or set at your lua configuration file the [[abaeterno_so]] variable to point to the [[abaeterno_so]] library. Example: [[abaeterno_so='../share/abaeterno.so']]\n" },
    :SchedulerPolicyNotFound => {:message => "No scheduler was found for the policy {{policy}}", :solution => "Set the scheduler policy to one of the availables: {{policy_list}}" },
    :SimnowCommandsNotDeclared => {:message => "[[simnow.commands]] function was not declared in COTSon simulation configuration lua file", :solution => "Check COTSon documentation and create the simnow.commands function in your lua configuration files with the necessary options.\nTo reproduce the error with lua execute '{{lua}} {{script}} {{luainput}}'." },
    :UserScriptNotFound => {:message => "User script referenced by [[execute]] function call was not found or the [[execute]] function is not being called", :solution => "Check if [[execute]] function is called inside of [[simnow.commands]] and if the referenced script (parameter) exists in the current directory.\nTo reproduce the error with lua execute '{{lua}} {{script}} {{luainput}}'." },
    :VNCTimeOut => {:message => "The startup of a node got frozen because it was not possible to initialize the VNC server using the display {{display}} for sandbox {{sandbox}}.\nThe command used was:'{{command}}'", :solution => "It is very likely that you have reached the maximum amount of VNC servers that can be run simultaneously.\nTo check the problem cause run the command '{{command}}'.\nYou can solve this problem by checking if the running VNC servers are necessary and killing the ones that are not or by increasing the amount of VNC servers that can be run simultaneously.\nThe VNC program being used by COTSon is {{program}}" },
	:CtrlC => {:message => "Got Ctrl-C", :solution => "None (you pressed ctrl-c!)" },
    }
end


class CotsonError < StandardError
  def initialize(errorlocation, info=nil)
    @info = {}
    @location = errorlocation
    push_info(info) if info
    cause = @info[:exception]
    push_info(cause.info) if cause.instance_of?(CotsonError)
  end
  
  def info
    @info
  end
  
  def inspect_error
    if @location.to_s =~ /Sandbox/ && @info[:status] && @info[:status] !=0 then
      if @info[:output] =~ /MediatorNotRunning/ then
        #Check if it was a problem with mediator file or a crash during execution
        mediator_file = @info[:location]+"/share/mediator"
        @info[:mediator_original] = File.readlink(mediator_file) if File.symlink?(mediator_file)
        return :MediatorNotFound if !File.exist?(mediator_file)
        return :MediatorBadLink if File.symlink?(mediator_file) && !File.exist?(@info[:mediator_original])
        return :MediatorNotExecutable if !File.executable?(mediator_file)
        medlog=@info[:location]+"/data/mediator.log"
        if File.exist?(medlog)
          @info[:mediator_output] = File.readlines(medlog).map {|l| l.rstrip} 
          return :MediatorFinished
        end
        return :MediatorFinishedNoOutput
      end
    end

    case @location
      when :Compare
        return :CompareWithoutProduced if !File.exist?(@info[:produced])
        if @info[:real_content].join != @info[:produced_content] .join then
          #can even do a diff here
          @info.delete(:real_content)
          @info.delete(:produced_content)
          return :ComparisonMismatch 
        end
      
      when :CotsonScript
        return :NoInitialArguments if @info.size == 0
        
      when :DaemonCall
        return :ConnectionRefused if @info[:exception].instance_of? Errno::ECONNREFUSED
        if @info[:exception].instance_of? CotsonError
          return @info[:exception].inspect_error 
        end
        
      when :DaemonStart
        return :DaemonAlreadyOn

      when :DaemonStop
        return :DaemonAlreadyOff
        
      when :DaemonProcessResponse
        return :PassThroughError if @info[:message]
        return :HTTPCommunicationError if @info[:response_type] 
        
      when :GetMediatorValue
        return :MediatorNotRunning if @info[:running] != nil && @info[:running] == false
        
      when :LuaDefinitionsCheck
        return :FileNotFound if !File.exist?(@info[:file])
        return :LuaNotExecutable if !File.executable?(@info[:file]) && @info[:name]=="lua"
        
      when :OptionsLoad
        case @info[:status].to_i
          when 255 
            #Not known the variable means that variable was not defined
            case @info[:variable]        
              when :mediator_file
                return :MediatorNotFound
              when :abaeterno_so
                return :AbaeternoNotFound
              when :bsd
                return :BSDNotFound
              when :hdd
                return :HDDNotFound
              when :user_script
                return :UserScriptNotFound
              else
                return :LuaVariableNotDefined
            end
          when 1
            #Error parsing lua
            return :SimnowCommandsNotDeclared if @info[:output] =~ /attempt to call field 'commands'/ 
            return :LuaParsing
        end  
      
      when :RegressionTest
        return :NoCriteria if @info[:arguments].strip.empty?
        return :NoTestSelected if @info[:tests].empty?
        
      when :RegressionTestAllInADir
        return :NotInAbaeternoDir
      
      when :SandboxInstallAbaeterno
        return :AbaeternoNotFound
      
      when :SchedulerCreate
        return :NoSchedulerPolicy if @info[:policy] == nil || @info[:policy]==""
        return :SchedulerPolicyNotFound
      
      when :SchedulerInitialize
        return :NoHosts if @info[:hosts].size == 0

      when :StartVNC
        return :DisplayNotAvalilabe if @info[:display] == nil
        return :VNCTimeOut if @info[:wait] >= @info[:timeout]

      when :CtrlC
        return :CtrlC
        
    end
    return nil
  end
  
  def to_s
    #Inspect and format error
    @error = inspect_error
    if @error == :PassThroughError then
      #Error comes already formated
      return @info[:message]
    end
    #Return a formated error message    
    ErrorMessage.message(@location, @error, @info[:exception], @info) #
  end

  def push_info(info)
    @info = @info.merge(info)
  end
end

def error_message(exception, values=nil)
  if values then
    begin
      if values[:med_sandbox] then
        values[:med_sandbox].copy_files(:type => :output, 
            :filter => '*.log',
            :prefix => "err.med.",
            :destination => Dir.pwd)
        values.delete(:med_sandbox)
      end
      if values[:node_sandbox] then
        i=1
        values[:node_sandbox].each do |x|
          x.copy_files(:type => :output, 
            :filter => '*.log',
            :prefix => "err.node.#{i}.",
            :destination => Dir.pwd)
          i+=1
        end
        values.delete(:node_sandbox)
      end
#    rescue Exception => e  #RG -- reduce warnings
    rescue Exception
      values[:additional_info]= 'Could not copy sandboxes logfiles.'
    end
  end

  if exception == nil then
    return ErrorMessage.message(nil, nil, nil, values)
  end
  
  case exception
    when CotsonError
      return exception.to_s 
    #Treating exceptions directly: these are the exceptions that made all their way
    #to the end of the program without being catched
    when Errno::ESRCH
      return ErrorMessage.message(nil, :InconsistentDaemonPID, exception, values)
  end
  ErrorMessage.message(nil, nil, exception, values)
end
