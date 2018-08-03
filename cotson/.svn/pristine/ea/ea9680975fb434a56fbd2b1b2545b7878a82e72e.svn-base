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

require 'execute'
require 'location'

class LuaDefinitions
  def LuaDefinitions.check(name='lua')
    file=$here.share(name)
    raise CotsonError.new(:LuaDefinitionsCheck, :name=>name, :file=>file) if !File.exist?(file) 
    if name == 'lua' then  
      #raise "lua not executable" if !File.executable?(file)
      raise CotsonError.new(:LuaDefinitionsCheck, :name=>name, :file=>file) if !File.executable?(file)
    end
  end

  def LuaDefinitions.run(script,*args)
    LuaDefinitions.check
    LuaDefinitions.check script
    tmpf=Tempfile.new("cotson-lua")
    tmpf << "root='#{$here}'\ncotson_pid=#{$$}\n"
    tmpf << "control_script_file='sandbox-'..cotson_pid..'-ctrl'\n"
    args.each { |x| tmpf << x.to_str }
    tmpf.close
    a=Execute.run("#{$here.share('lua')} #{$here.share(script)} #{tmpf.path}")
    if a[:status] != 0 then
      debugtmp="/tmp/lua#{$$}.in"
      FileUtils.cp tmpf.path, debugtmp
      raise CotsonError.new(:LuaDefinitionsRun, :lua=>$here.share('lua'), 
                          :script=>$here.share(script), :luainput=>debugtmp,
              :status=>"#{a[:status]}", :output=>a[:output])
    end
    tmpf.delete
    a[:output]
  end

  def method_missing(method)
    LuaDefinitions.run(method.id2name+".lua",self)
  end

  def display
    LuaDefinitions.run('display.lua',self)
  end

  def debug
    LuaDefinitions.run('debug.lua',self)
  end
end
