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

class RegressionTest
	include Comparable
	attr_reader :tid

	def initialize(filename)
		@tid=File.basename(filename)
		@path=File.dirname(filename)
		@tags=Dir[filename+"/tag.*"].map { |x| x.gsub(/.*tag\./,'') }.sort
	end

	def to_s
		"'#{@tid}' { #{@tags.join(' ')} }"
	end

	def <=>(other)
		tid <=> other.tid
	end

	def ===(tag)
		tag == 'all' || @tags.include?(tag)
	end

	def files(name)
		Dir[File.join(@path,@tid)+"/"+name].map { |x| File.expand_path(x) }.sort
	end

	def RegressionTest.all_in_a_dir(dir)
		if !File.exist?(dir)
      raise CotsonError.new(:RegressionTestAllInADir, :dir=>dir)
		end
		tests=[]
		Dir["test.regression/*"].each do |y|
			next if !File.directory?(y)
			tests.push RegressionTest.new(y)
		end
		tests.sort
	end

	def RegressionTest.find_all(args)
		all=RegressionTest.all_in_a_dir('test.regression')
		tests=[]
		rest=[]
		args.each do |x|
			if (tag=x.gsub(/^\@/,''))!=x then
				tests+=all.find_all { |y| y === tag }
			else
				e=all.find { |y| y.tid == x }
				if e
					tests.push e
				else
					rest.push x
				end
			end
		end
		[tests.uniq.sort,rest]
	end
end
