#$: << File.expand_path(File.dirname(__FILE__) + '/../../share')
require 'daemon_service'

class ExperimentMachine < ActiveRecord::Base
  belongs_to :experiment
  
  def console_server
    if self.console_port == nil
      #start an X for the console
      ds = DaemonServiceCall.new(self.server, self.sandbox_id)
      puts "a"
      ds.consoleX

      start=Time.new 
      while(Time.new - start < 5) do
        puts ds.get_vnc_webport
        if ds.get_vnc_webport =~ /.*rxvt_display_http=(\d+).*/ then
          self.console_port = $1
          self.save
          break
        end
        Kernel.select(nil,nil,nil,0.1)
      end
    end

    return nil if self.console_port == nil
    self.server.gsub(/:.*/,":#{self.console_port}")
  end
  
  def screen_server
    self.server.gsub(/:.*/,":#{self.vnc_port}")
  end
end
