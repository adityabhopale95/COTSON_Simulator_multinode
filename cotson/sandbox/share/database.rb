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

# $Id: database.rb 6977 2009-12-09 16:18:35Z frb $

class Database < CConfig
	def initialize(file)
		super(file)
	end

	def store
		begin
			File.open(@file,'w' ) do |out|
				YAML.dump( @data, out )
			end
	  rescue Exception => e
      raise CotsonError.new(:DatabaseStore, :file=>@file, :error=>e)
		end
	end

	public
	def [](k)
		load
		@data[k]
	end

	def need(k)
		load
    raise CotsonError.new(:DatabaseNeed, :key=>k, :file=>@file) if !@data.has_key?(k)
		@data[k]
	end

	def []=(k,v)
		load
		if(v)	
			@data[k]=v
		else
			@data.delete(k)
		end
		store
	end

	def clear
		@data=Hash.new
		store
	end
end
