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

# $Id: send.rb 6977 2009-12-09 16:18:35Z frb $

require 'socket'

class Send
	def Send.command(message)
		begin
			s = TCPSocket.new("127.0.0.1", IO.read($here.data('command_port')).to_i)
			s.print(message)
			r=s.recv( 16384 )
			s.close
			r
#		rescue Exception => f   # RG -- reduce warnings
		rescue Exception
			nil
		end
	end
end
