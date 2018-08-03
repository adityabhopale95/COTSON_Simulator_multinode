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

# $Id: filecopy.rb 6977 2009-12-09 16:18:35Z frb $

require 'tempfile'

class Location
	def list_files(opts={:type => :all, :filter => '*'})
		opts[:type] = :all  if opts[:type] == nil
		opts[:filter] = '*' if opts[:filter] == nil

		list=[]
		FileUtils.cd("#{@root}/data") do
			list=Dir[opts[:filter]]
		end

		case opts[:type] 
		when :input
			list_input & list
		when :output
			list - list_input
		when :subscribed
			(list - list_input).select { |f| f=~/^res_[0-9]+/ }
		else # :all
			list
		end
	end
  
	def copy_files(opts={:destination => '', :prefix => '', 
		:type => :all, :filter => '*'})
		dest=opts[:destination]
		prefix=opts[:prefix]
    raise CotsonError.new(:LocationCopyFiles, :destination=>dest, :prefix=>prefix) if !File.exist?(dest)

		files=list_files(opts)

		debug2 "copying files [#{files.join(" ")}] to #{dest}"

		files.each { |f| FileUtils.cp "#{@root}/data/#{f}", "#{dest}/#{prefix}#{f}" }
		return files
	end
  
  def mark_input
    input_files=Dir.entries("#{@root}/data").select {|f| f=~/^[^.]/ }
    File.open(data('.input_files'),"w") do |f|
      f << input_files.join("\n")
    end
  end
  
	private
	def list_input
		inf=data('.input_files')
		File.exist?(inf) ? 
			IO.readlines(inf).map { |x| x.chomp } : 
			[]
	end
end
