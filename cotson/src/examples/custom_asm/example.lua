
abaeterno_so="custom_asm.so"
wd=os.getenv("PWD")
tmpdir=wd
runid="asm"
-- clean_sandbox=false

options = {
	--max_nanos='3G',
	exit_trigger='terminate',
	sampler={type="simple", quantum="100M"},
	heartbeat={ type="file_last", logfile=runid..".log" }, 
	custom_asm=true
}

one_node_script="run_interactive"
display=os.getenv("DISPLAY")
copy_files_prefix=runid.."."

simnow.commands=function()
	use_bsd('1p-karmic64.bsd')
	use_hdd('karmic64.img')
	set_journal()
	execute('run.sh')
end

function build()
	i=0
	while i < disks() do
		disk=get_disk(i)
		disk:timer{ name='disk'..i, type="simple_disk", }
		i=i+1
	end

	i=0
	while i < nics() do
		nic=get_nic(i)
		nic:timer{ name='nic'..i, type="simple_nic", }
		i=i+1
	end

	i=0
	while i < cpus() do
		cpu=get_cpu(i)
		cpu:timer{ name='cpu'..i, type="timer0" }
		i=i+1
	end
end

-- vim:ft=lua
