#!/usr/bin/env lua
-- (C) Copyright 2009 Hewlett-Packard Development Company, L.P.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
-- THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
-- OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
-- ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
-- OTHER DEALINGS IN THE SOFTWARE.
--

-- $Id: scheduler_hosts.lua 15 2010-01-08 14:59:25Z paolofrb $

dofile(arg[0]:gsub("(.+)/.+","%1/defaults.lua"))

hosts=scheduler.hosts
if hosts~=nil then
	if type(hosts)=='string' then
		print(hosts)
	end
	if type(hosts)=='table' then
		for key,value in ipairs(hosts) do
			print(value)
		end
	end
end
