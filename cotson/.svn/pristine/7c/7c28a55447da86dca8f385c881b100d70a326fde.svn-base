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

# $Id: vnc.rb 6977 2009-12-09 16:18:35Z frb $

class VNC
  TIMEOUT = 30
  MIN_DISPLAY = 50
  MAX_DISPLAY = 1024
  def VNC.start_vnc
    display=(MIN_DISPLAY..MAX_DISPLAY).detect do |d|
      !File.exist?(File.join(Dir.tmpdir,".X#{d}-lock")) &&
      !File.exist?(File.join(Dir.tmpdir,".X11-unix","X#{d}"))
    end
    
    raise CotsonNew(:StartVNC, :sandbox=>$here, :display=>display, :wait=>0, :timeout=>VNC::TIMEOUT, :program=>CF[:Xvnc], :min_display=>MIN_DISPLAY, :max_display=>MAX_DISPLAY) if !display
    # we execute the server in the background    
    cmd = "#{CF[:Xvnc]} :#{display} #{CF[:Xvnc_args]}\'#{CF[:runid]}-#{CF[:NODE]}\'"
    a=Execute.run_background(cmd,$here.data('vnc.log'))
    vnc_pid=a[:pid]
    # we wait for the server to appear
    beg=Time.new 
    while(true) do
      break if File.exist? File.join(Dir.tmpdir,".X11-unix","X#{display}")
      Kernel.select(nil,nil,nil,0.1)
      now=Time.new
      raise CotsonError.new(:StartVNC, :sandbox=>$here, :command => cmd, :display=>display, :vnc_pid=>vnc_pid, :wait=>now-beg, :timeout=>TIMEOUT, :program=>CF[:Xvnc]) if now - beg > VNC::TIMEOUT #Time out if over 30 seconds
    end
    # Fire up the window manager in the background (will die with vnc)
    cmd2 = "#{CF[:Xvncwm]}:#{display}"
    Execute.run_background(cmd2,$here.data('vncwm.log'))
    [vnc_pid, display]
  end

  def VNC.display_for_ENV(display)
    display_str=display.to_s
    if display_str !~ /:/ then
      display_str=":#{display_str}"
    end
    if display_str !~ /\.[0-9]$/ then
      display_str="#{display_str}.0"
    end
    display_str
  end

  def VNC.display_for_http(display)
    display_str=display.to_s.sub(/.*:/,'')
    display_str.to_i+5800
  end

  def VNC.display_for_vncviewer(display)
    display=":#{display}" if display !~ /:/
    display.gsub(/\.[0-9]$/,'')
  end
end

