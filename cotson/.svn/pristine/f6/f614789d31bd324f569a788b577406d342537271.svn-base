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

# $Id$

$: << File.expand_path(File.dirname(__FILE__) + '/../sandbox/share')

require 'sandbox'
require 'lua_definitions'
require 'error'
require 'options'
require 'regression_test'
require 'regression'
require "socket"

$debug=0

def debug(*args)
	puts args.join(' ') if $debug >= 1
end

def debug_nnl(*args)
	print args.join(' ') if $debug >= 1
	$stdout.flush
end

def debug2(*args)
	puts args.join(' ') if $debug >= 2
end

def finish(e, values=nil)
  puts error_message e, values

	if $debug >= 2 then
		puts "backtrace:" 
		puts e.backtrace
		puts
	end
	exit(-1)
end

def this_host()
	Socket.gethostname.gsub(/[^A-Za-z0-9.]/,'')
end
