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

# $Id: options.rb 6977 2009-12-09 16:18:35Z frb $

require 'yaml'

class Options
  def initialize(ld,vals={})
    @lua_defs=ld
    @opts=Hash.new
    vals.each { |x,y| @opts[x]=y }
  end

  def one_node?
    begin 
      self.load :cluster_nodes
    rescue 
      return true
    else
      return false
    end
  end

  def control_script?
    begin 
      self.load :control_script_file
    rescue 
      return false
    else
      return true
    end
  end

  def scheduler?
    begin 
      self.load :scheduler
      @opts[:scheduler]
    end
  end

  def load(*args)
    args << :debug << :tmpdir
    args << :runid if ! @opts.has_key?(:runid)
    args.each do |x|
      begin
        val=@lua_defs.send x
        rescue Exception => e # CotsonError => e 
        raise CotsonError.new(:OptionsLoad, :variable=>x, :exception=>e)
      end
      if val=="true" then val=true end
      if val=="false" then val=false end
      @opts[x.to_sym]=val
    end

    $debug=@opts[:debug]
    $debug=1 if $debug.class == TrueClass
    $debug=0 if $debug.class == FalseClass
    $debug=$debug.to_i if $debug.class == String
  end

  def [](k)
    raise CotsonError.new(:OptionGetValue, :key=>k) if !@opts.has_key?(k)
    @opts[k]
  end

  def save(file)
    File.open(file,'w' ) { |x| YAML.dump(@opts, x ) }
  end

  def load_from_file(file)
    begin
      @opts = YAML::load_file(file)
    rescue Exception => e
      raise CotsonError.new(:OptionsLoadFromFile, :file=>file, :exception=>e) 
    end
  end
end
