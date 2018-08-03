#TODO: Put these requires in a better way
#require "#{RAILS_ROOT}/sandbox/share/error"
#require "#{RAILS_ROOT}/share/daemon_service"
require "error"
require "daemon_service"

class Machine < ActiveRecord::Base
  def online
    begin
      dc=DaemonServiceCall.new("#{self.host}:#{self.port}")     
      dc.ping=="Ok"   
    rescue 
      return false
    end
  end
  def info(infotag)
    #Possible tags: :Info :Release :MemTotal :MemFree :Processors :Simulations
    return nil if ! self.online
    data = {}
    begin
      dc=DaemonServiceCall.new("#{self.host}:#{self.port}")     
      data=dc.get_load
    rescue 
      puts "#{e}"
      return nil
    end
    puts data
    puts "RESPONSE"
    puts data[infotag]
    data["#{infotag}"]
  end
end
