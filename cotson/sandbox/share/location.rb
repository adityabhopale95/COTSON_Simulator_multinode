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

# $Id: location.rb 6977 2009-12-09 16:18:35Z frb $

require 'tempfile'

class Location
  def initialize(root)
    @root=File.expand_path(root)
  end

  def from_root(*args)
    args.pop if args[-1]==nil
    File.expand_path(File.join(@root, *args))
  end

  %w{bin data etc share daemon}.each do |x|
    module_eval <<-"end;"
      def #{x}(*args) from_root('#{x}', *args) end
    end;
  end
  
  def to_s
    to_str
  end

  def to_str
    @root
  end

  def reset(new_root)
    @root,r=new_root,@root
    r
  end

  def Location.tempfile(name)
    tf=Tempfile.new(name)
    n=tf.path
    tf.close!
    Location.new(n)
  end
end

$here=Location.new(File.join(File.dirname($0), '..'))
