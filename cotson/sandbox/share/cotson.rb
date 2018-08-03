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

# $Id: cotson.rb 6977 2009-12-09 16:18:35Z frb $

require 'location'
require 'error'
require 'cconfig'  #RG  -- 'config' renamed 'CConfig' for CotsonConfig
require 'database'
begin
  CF=CConfig.new($here.data('config'))
  DB=Database.new($here.data('database'))
rescue Exception => e
  #Error ocurred before function finish is defined
  puts e
  exit(-1)
end

def debug(*args)
	puts args.join(' ') if CF[:debug]
end

def debug_nnl(*args)
	print args.join(' ') if CF[:debug]
	$stdout.flush
end

def debug2(*args)
	puts args.join(' ') if CF[:debug]
end

def finish(e, values=nil)
  puts error_message(e, values)

  if CF[:debug]
		puts "backtrace:"
		puts e.backtrace 
		puts
  end
  exit(-1)
end

require 'simnow'
require 'regression'
require 'kill'
require 'send'

require 'socket'

