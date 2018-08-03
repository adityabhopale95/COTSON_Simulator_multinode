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

require 'location'
require 'stringio'

class LuaDefinitions
	private
	def fix_with_exec_dir(file)
		defined?(@exec_dir) ? File.join(@exec_dir,file) : file
	end

	def from_conf(name)
		jamfile_location=fix_with_exec_dir('Jamfile')
		if File.exist?(jamfile_location) then
			IO.foreach(jamfile_location) do |line|
				if line =~ /^#{name} *= * (.+) * ; *$/
					return $1	
				end
			end
		end
		makeconf_location=fix_with_exec_dir('../Make.conf')
		if File.exist?(makeconf_location) then
			IO.foreach(makeconf_location) do |line|
				if line =~ /^#{name} *= * (.+)$/
					return $1	
				end
			end
		end
		nil
	end

	def read_file(x)
		if File.exist?(x) then
			st=StringIO.new
			File.open(x).each_line { |y| st << y }
			st.string
		else
			z=fix_with_exec_dir(x)
			if File.exist?(z) then
				st=StringIO.new
				File.open(z).each_line { |y| st << y }
				st.string
			else
				nil
			end
		end
	end

	protected
	def default_data_dir
		dd=nil
		if a=ENV['DATA_DIR'] then
			dd="data_dir='#{File.expand_path(a)}'\n"
		end
		if a=from_conf('DATA_DIR') then
			dd="data_dir='#{File.expand_path(a)}'\n"
		end
		dd
	end

	def default_injector
		injector_location=fix_with_exec_dir('aaInjector')
		File.exist?(injector_location) ?
			"injector='#{File.expand_path(injector_location)}'\n" :
			nil
	end

	def default_mediator
		mediator_location=fix_with_exec_dir('mediator')
		File.exist?(mediator_location) ?
			"mediator_file='#{File.expand_path(mediator_location)}'\n":
			nil
	end

	def default_simnow
		sim=nil
		if a=ENV['SIMNOW_DIR'] then
			sim="simnow_dir='#{File.expand_path(a)}'\n"
		end
		if a=from_conf('SIMNOW_DIR') then
			sim="simnow_dir='#{File.expand_path(a)}'\n"
		end
		sim
	end

	def default_abaeterno
		abaeterno_location=fix_with_exec_dir('abaeterno.so')
		File.exist?(abaeterno_location) ?
			"abaeterno_so='#{File.expand_path(abaeterno_location)}'\n" :
			nil
	end

	def dynamic_defaults
		st=StringIO.new
		%w{data_dir injector mediator simnow abaeterno}.each do |x|
				st << self.send('default_'+x)
		end
		st.string
	end

	def arguments(args)
		st=StringIO.new
		args.each do |x|
			y=read_file(x)
			st << (y ? y : "#{x}\n")
		end
		st.string
	end

	def script_defaults
		st=StringIO.new
		st << read_file($here.etc(File.basename($0)))
		st.string
	end

	public
	def initialize(load={})
		@exec_dir=load[:exec_dir] if load[:exec_dir]
		@instance_defaults=''
		@command_line_arguments=''
		@script_defaults=''
		@dynamic_defaults=''
		if load[:command_line].kind_of? Array then
			@command_line_arguments=arguments(load[:command_line])
		else
			if load[:command_line]!=false then
				@command_line_arguments=arguments(ARGV)
			end
		end
		@script_defaults=script_defaults unless load[:script_defaults]==false
		@dynamic_defaults=dynamic_defaults unless load[:dynamic_defaults]==false
	end

	def to_str
		@script_defaults +
		@dynamic_defaults +
		@instance_defaults +
		@command_line_arguments
	end

	def instance_defaults(values={})
		a=""
		values.each { |x,y| a="#{a}#{x}=#{y}\n" }
		@instance_defaults=a
	end
end
