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

# $Id: kill.rb 6977 2009-12-09 16:18:35Z frb $

require 'execute'
require 'vnc'

class Kill
	def Kill.vnc
		a=DB['vnc_pid']
        puts "kill vnc pid #{a}"
		Execute.kill(DB['vnc_pid'])
		Kill.display(DB['vnc_pid'],DB['display'])
		DB['vnc_pid']=nil
		DB['vnc_pid']=nil
		DB['display']=nil
		a
	end

	def Kill.screen
		a=DB['screen_pid']
		Execute.kill(DB['screen_pid'])
		FileUtils.rm_f($here.share("screen_dir/#{a}.sessionname")) if a
		DB['screen_pid']=nil
		a
	end

	def Kill.rxvt
		a=DB['rxvt_pid']
		Execute.kill(DB['rxvt_pid'])
		Kill.display(DB['rxvt_vnc_pid'],DB['rxvt_display'])
		DB['rxvt_pid']=nil
		DB['rxvt_vnc_pid']=nil
		DB['rxvt_display']=nil
		a # need to know if I have killed anything
	end

	def Kill.display(pid,display)
		Execute.kill(pid)
		if display && pid then
			FileUtils.rm_f([File.join(Dir.tmpdir,".X#{display}-lock"),
				File.join(Dir.tmpdir,".X11-unix","X#{display}")])
		end
	end

	def Kill.mediator
		a=DB['mediator_pid']
		Execute.kill(a)
		a
	end
end
