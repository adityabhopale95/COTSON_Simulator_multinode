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

def abaeterno_test(test,argv)
	my_args =  []
	my_args << test.files('abaeterno_input').first
	my_args << "options.heartbeat.logfile='../data/abaeterno.out'"
	my_args << "options.trace_stats_disasm_file='../data/disasm_output'"
	my_args += argv

	lua_defs=LuaDefinitions.new :command_line => my_args
	opts=Options.new(lua_defs,:test => test,:runid => "test"+test.tid+"-")
	opts.load :data_dir, :display,
		:simnow_dir, :simnow_dir_link,
		:abaeterno_so, :abaeterno_so_link,
		:clean_sandbox, :screen

	sb=Sandbox.new(lua_defs,opts)
	sb.install :abaeterno_in, :simnow_pre, :simnow_dir,
		:abaeterno_so, :test_data

	i,o=sb.run_output('regression')
	debug2("regression output begin\n#{o}\nregression output end")
	return false,"incorrect return value",sb.to_str if i!=0

	i,o=sb.run_output('check_regression')
	debug2("check_regression output begin\n#{o}\ncheck_regression output end")
	return false,o.chomp,sb.to_str if i!=0

	sb.clean
	return true,"",""
end

def injector_test(test,argv)
	my_args =  []
	my_args << test.files('abaeterno_input').first
	my_args << "options.heartbeat.type='file_last'"
	my_args << "options.heartbeat.logfile='../data/abaeterno.out'"
	my_args += argv

	lua_defs=LuaDefinitions.new :command_line => my_args
	opts=Options.new(lua_defs,:test => test,:runid => "test"+test.tid+"-")
	opts.load :data_dir, :clean_sandbox,
		:injector, :injector_link

	sb=Sandbox.new(lua_defs,opts)
	sb.install :abaeterno_in, :test_data,
		:injector

	i,o=sb.run_output('injector')
	debug2("injector output begin\n#{o}\ninjector output end")
	return false,"incorrect return value",sb.to_str if i!=0

	i,o=sb.run_output('check_regression')
	debug2("check_regression output begin\n#{o}\ncheck_regression output end")
	return false,o.chomp,sb.to_str if i!=0

	sb.clean
	return true,"",""
end
