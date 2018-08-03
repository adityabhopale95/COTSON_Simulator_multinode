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

-- $Id: user_script.lua 15 2010-01-08 14:59:25Z paolofrb $

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

run_script=''
function execute(x)
        local tmpstrv={}
        tmpstrv=mysplit(x)
        l=location(tmpstrv[0])
        check_exists(l,"run script '"..tmpstrv[0].."' does not exist")
        if (tmpstrv[1] ~= nil) then
                local tmps2=table.concat(tmpstrv, " ", 1);
                puts(l.." "..tmps2)
        else
                puts(l)
        end
end

if type(simnow)~='table' or type(simnow.commands)~='function' then
	error("no simnow.commands function")
end

if run_script=='' then
	simnow.commands()
else
	execute(run_script)
end

