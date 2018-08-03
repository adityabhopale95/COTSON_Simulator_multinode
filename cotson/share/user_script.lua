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

-- $Id: user_script.lua 745 2016-11-25 18:35:57Z robgiorgi $

dofile(arg[0]:gsub("(.+)/.+","%1/defaults.lua"))

function mysplit(inputstr, sep)
        if sep == nil then
                sep = "%s"
        end
        local t={} ; i=0
        for str in string.gmatch(inputstr,"([^"..sep.."]+)") do
                t[i] = str
                i = i + 1
        end
        return t
end

executed=false
function execute(x)
	local tmpstrv={}
	tmpstrv=mysplit(x)
	l=location(tmpstrv[0])
	check_exists(l,"user script '"..tmpstrv[0].."' does not exist")
        if (tmpstrv[1] ~= nil) then
        	local tmps2=table.concat(tmpstrv, " ", 1);
		puts(l.." "..tmps2)
	else
		puts(l)
	end
	executed=true
end

if type(simnow)~='table' or type(simnow.commands)~='function' then
	error("no simnow.commands function")
end

if user_script==nil then
	simnow.commands()
	if executed==false then
		error("no execute call inside simnow.commands")
	end
else
	execute(user_script)
end

