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

# $Id: config.rb 6977 2009-12-09 16:18:35Z frb $

require 'yaml'
 require 'regression_test'

class CConfig
	def initialize(file)
		@file=file
		@data=Hash.new
		load
	end

	def load
		begin
			@data = YAML::load_file(@file)
	  rescue Exception => e
      raise CotsonError.new(:ConfigLoad, :file=>@file, :exception=>e)
		end
	end

	public
	def [](k)
		raise CotsonError.new(:ConfigGetValue, :file=>@file, :key=>k) if !@data.has_key?(k)
		@data[k]
	end

	def force(vals={})
		vals.each { |x,y| @data[x.to_sym]=y }
	end
end
