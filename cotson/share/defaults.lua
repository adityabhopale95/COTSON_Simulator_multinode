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

-- $Id: defaults.lua 176 2011-03-02 12:44:12Z paolofrb $

function is_absolute(x) return x:sub(1,1) == '/' end

function exists_file(x)
	f=io.open(x,"r")
	if f then
		f:close()
		return true
	else
		return false
	end
end

function error(m)
		if m then
			print("error: "..m.."\n")
		end
		os.exit(-1)
end

check_for_existance=true
function do_not_check_for_existance()
	check_for_existance=false
end

function check_exists(x,error)
	if check_for_existance and not exists_file(x) then
		print(error)
		os.exit(-1)
	end
end

function location(x)
    if x==nil then return nil end
	-- file is absolute
	if is_absolute(x) then return x end
	
	-- relative to exec_dir
	if exec_dir~=nil then
		relative=exec_dir..'/'..x
		if exists_file(relative) then
			return relative
		end
	end
		
	--	or relative to data_dir
	relative=data_dir..'/'..x
	if exists_file(relative) then
		return relative
	end
	
	return x
end

function to_s(x)
	if type(x)=='string' or type(x)=='number' then
		return x
	end
	if type(x)=='boolean' then
		if x then
			return "true"
		else
			return "false"
		end
	end
	error("unknown type '"..type(x).."'")
end

function puts(x)
	io.write(to_s(x))
end
	
-- main api (helper versions)

function send_keyboard_helper(x,finish)
	if finish==nil or finish==true then
		x=x..'\r'
	end
	local function chars(s,n)
		if n < s:len() then
			sp_start,sp_end=s:find("(\\{[^}]+})")
			if sp_start == n+1 then
				n=n+(sp_end-sp_start)+1
				return n,s:sub(sp_start,sp_end)
			else
				n=n+1
				return n,s:sub(n,n)
			end
		end
	end
	local letters={ ['a']='1E 9E', ['b']='30 B0', ['c']='2E AE', ['d']='20 A0',
		['e']='12 92', ['f']='21 A1', ['g']='22 A2', ['h']='23 A3',
		['i']='17 97', ['j']='24 A4', ['k']='25 A5', ['l']='26 A6',
		['m']='32 B2', ['n']='31 B1', ['o']='18 98', ['p']='19 99',
		['q']='10 9D', ['r']='13 93', ['s']='1F 9F', ['t']='14 94',
		['u']='16 96', ['v']='2F AF', ['w']='11 91', ['x']='2D AD',
		['y']='15 95', ['z']='2C AC', ['1']='02 82', ['2']='03 83',
		['3']='04 84', ['4']='05 85', ['5']='06 86', ['6']='07 87',
		['7']='08 88', ['8']='09 89', ['9']='0A 8A', ['0']='0B 8B',
		['A']='2A 1E 9E AA', ['B']='2A 30 B0 AA', ['C']='2A 2E AE AA',
		['D']='2A 20 A0 AA', ['E']='2A 12 92 AA', ['F']='2A 21 A1 AA',
		['G']='2A 22 A2 AA', ['H']='2A 23 A3 AA', ['I']='2A 17 97 AA',
		['J']='2A 24 A4 AA', ['K']='2A 25 A5 AA', ['L']='2A 26 A6 AA',
		['M']='2A 32 B2 AA', ['N']='2A 31 B1 AA', ['O']='2A 18 98 AA',
		['P']='2A 19 99 AA', ['Q']='2A 10 9D AA', ['R']='2A 13 93 AA',
		['S']='2A 1F 9F AA', ['T']='2A 14 94 AA', ['U']='2A 16 96 AA',
		['V']='2A 2F AF AA', ['W']='2A 11 91 AA', ['X']='2A 2D AD AA',
		['Y']='2A 15 95 AA', ['Z']='2A 2C AC AA', ['!']='2A 02 82 AA',
		['@']='2A 03 83 AA', ['#']='2A 04 84 AA', ['$']='2A 05 85 AA',
		['%']='2A 06 86 AA', ['^']='2A 07 87 AA', ['&']='2A 08 88 AA',
		['*']='2A 09 89 AA', ['(']='2A 0A 8A AA', [')']='2A 0B 8B AA',
		['`']='29 A9', ['-']='0C 8C', ['=']='0D 8D', ['\\']='2B AB',
		['[']='1A 9A', [']']='1B 9B', [';']='27 A7', ['\'']='28 A8',
		[',']='33 B3', ['.']='34 B4', ['/']='35 B5', [' ']='39 B9',
		['~']='2A 29 A9 AA', ['_']='2A 0C 8C AA', ['+']='2A 0D 8D AA',
		['|']='2A 2B AB AA', ['{']='2A 1A 9A AA', ['}']='2A 1B 9B AA',
		[':']='2A 27 A7 AA', ['"']='2A 28 A8 AA', ['<']='2A 33 B3 AA',
		['>']='2A 34 B4 AA', ['?']='2A 35 B5 AA', ['\r']='1C 9C',
		['\n']='1C 9C',  ['\{ESC}']='01 81', ['\{F1}']='3B BB',
		['\{F2}']='3C BC', ['\{F3}']='3D BD', ['\{F4}']='3E BE',
		['\{F5}']='3F BF', ['\{F6}']='40 C0', ['\{F7}']='41 C1',
		['\{F8}']='42 C2', ['\{F9}']='43 C3', ['\{F10}']='44 C4',
		['\{tab}']='0F 8F', ['\{ctl-M}']='1D', ['\{ctl-B}']='9D',
		['\{alt-M}']='38', ['\{alt-B}']='B8', ['\{del}']='E0 53 E0 D3',
		['\{up}']='E0 48 E0 C8', ['\{down}']='E0 50 E0 D0',
		['\{left}']='E0 4B E0 CB', ['\{right}']='E0 4D E0 CD',
		['\{bsp}']='0E 8E',	}

	for i,v in chars,x,0 do
		print('keyboard.key ' ..letters[v])
	end
end

function set_quantum_helper(n)
	print("setmpquantum "..n)
	print("setsyncquantum "..n+1)
end

function use_bsd_helper(x)
	l=location(x)
	check_exists(l,"bsd file '"..x.."' does not exist")
	print('open '..l)
end

function use_hdd_helper(x,num,dt)
	l=location(x)
	check_exists(l,"hdd file '"..x.."' does not exist")
	if num==nil then num=0 end
	if dt==nil then dt='master' end
	print('ide:'..num..'.image '..dt..' '..l)
end

function set_journal_helper(num,dt)
	if num==nil then num=0 end
	if dt==nil then dt='master' end
	print('ide:'..num..'.journal '..dt..' on')
end

function set_diskjournal_helper(x,num,dt)
	if num==nil then num=0 end
	if dt==nil then dt='master' end
	print('ide:'..num..'.journal '..dt..' off')
	print('ide:'..num..'.journaladd '..dt..' '..x)
end

function set_serial_helper(x)
    print("serial:0.setcommport "..x)
    print("serial:1.setcommport "..x)
end

function dumpregistry_helper()
	print('dumpregistry')
end

function analyzer_helper(cmd,anl,cpu)
	xcpu="" if cpu~=nil then xcpu="cpu:"..cpu.."." end
	if     cmd=="load"    then print(xcpu.."loadanalyzer "..anl)
	elseif cmd=="unload"  then print(xcpu.."unloadanalyzer "..anl)
	elseif cmd=="enable"  then print(xcpu.."enableanalyzer "..anl.." 1")
	elseif cmd=="disable" then print(xcpu.."enableanalyzer "..anl.." 0")
	elseif cmd=="show"    then print(xcpu.."showanalyzers ")
	else
	   error("ANALYZER ERROR:\
Usage: analyzer(load,    'analyzer_name analyzer_params', [,cpu])\
   or  analyzer(unload,  'analyzer_num'  [,cpu])\
   or  analyzer(enable,  'analyzer_num'  [,cpu])\
   or  analyzer(disable, 'analyzer_num'  [,cpu])\
   or  analyzer(show)\
       ");
	end
end

function set_network_helper(tx,rx,thr)
	low=NODE%256
	high=(NODE-low)/256

	if rx==nil then rx=1 end
	if tx==nil then tx=1 end
	if thr==nil then thr=100 end

	print("e1000.linkconnect down")
	print("cmos.setbyte 254 "..low)
	print("cmos.setbyte 255 "..high)
	print("e1000.setmacaddress FA:CD:01:00:00:"..string.format("%02X",NODE))
	print("e1000.setmediatorhost MEDIATOR")
	print("e1000.forcetunevalues on")
	print("e1000.tune rxdelay "..rx)
	print("e1000.tune txdelay "..tx)
	print("e1000.tune intthrtl "..thr)
	print("e1000.linkconnect auto")
end

-- main api (void versions)

function use_bsd(x) end
function use_hdd(x,num,dt) end
function set_journal(num,dt) end
function set_diskjournal(x,num,dt) end
function set_quantum(n) end
function set_network(tx,rx,thr) end
function send_keyboard() end
function execute(n) end
function set_serial(x) end
function dumpregistry() end
function analyzer(cmd,anl,cpu) end

function subscribe_result(x) 
	if result_subscriptions==nil then result_subscriptions='' end
	result_subscriptions = result_subscriptions..x.."\n"
end

-- this gets executed in each script that calls this

simnow={} -- simplifying defaults
options={}

for key,value in ipairs(arg) do
	if exists_file(value) then
		dofile(value)
	else
		assert(loadstring(value))()
	end
end

for x,y in pairs({ 'bsd', 'hdd', 'keyboard', 'net', 'quantum'}) do
	if simnow[y] then
		error("simnow."..y.." use is not allowed anymore")
	end
end

