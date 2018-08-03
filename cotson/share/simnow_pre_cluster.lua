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

-- $Id: simnow_pre_cluster.lua 176 2011-03-02 12:44:12Z paolofrb $

dofile(arg[0]:gsub("(.+)/.+","%1/defaults.lua"))

use_bsd=use_bsd_helper
use_hdd=use_hdd_helper
set_journal=set_journal_helper
set_diskjournal=set_diskjournal_helper
set_quantum=set_quantum_helper
send_keyboard=send_keyboard_helper
set_network=set_network_helper
set_serial=set_serial_helper
dumpregistry=dumpregistry_helper
analyzer=analyzer_helper

function execute(n)
	send_keyboard("xget ../data/cluster.sh b ; sh ./b")	
end

if type(simnow)~='table' or type(simnow.commands)~='function' then
	error("missing simnow.commands function")
end
simnow.commands()

if type(simnow)=='table' and type(simnow.xcommands)=='function' then
    simnow.xcommands()
end
