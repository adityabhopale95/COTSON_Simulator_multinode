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

# $Id: regression.rb 6977 2009-12-09 16:18:35Z frb $

def compare(real,produced)
	s=$here.data(real)
	d=$here.data(produced)
  raise CotsonError.new(:Compare, :real=>s, :produced=>d) if !File.exist?(d)

	sc=IO.readlines(s)
	dc=IO.readlines(d)

	if File.exist?($here.data('filters')) then
		IO.read($here.data('filters')).each_line do |filter|
			f=Regexp.new(filter.chomp)
			sc=sc.select { |y| y !~ f }	
			dc=dc.select { |y| y !~ f }	
		end
	end

  raise CotsonError.new(:Compare, :real=>s, :produced=>d, :real_content=>sc, :produced_content=>dc) if sc.join != dc.join
end

def dst_name(src_name)
	a=src_name.
		gsub(/^(.+)_output/,'\1.out').
		gsub(/simpoint_profile/,'sp_log').
		gsub(/disasm/,'disasm_output').
		gsub(/disksim/,'disksim_output').
		gsub(/disktrace/,'disktrace_output')
  raise CotsonError.new(:Dst_Name, :source=>src_name) if src_name==a
	a
end
