
options = {
	fastforward="100M",
	time_feedback = true,
	heartbeat = { type="sqlite", dbfile=DBFILE, experiment_id=1, experiment_description="test run"},
    max_nanos="200M",
    sampler={ type="dynamic", functional="500k", warming="100k", simulation="100k", 
              maxfunctional=1000, sensitivity="500",
	          variable={"cpu.*.other_exceptions", "cpu.*.iocount"}},
}

one_node_script="run_interactive"
-- display=os.getenv("DISPLAY")

simnow.commands=function()
	use_bsd('1p.bsd')
	use_hdd('karmic64.img')
	set_journal()
	send_keyboard('gcc -S -o - -O3 -c -w /home/user/test.i')
end

function build()
	i=0
	while i < disks() do
		disk=get_disk(i)
		disk:timer{ name='disk'..i, type="simple_disk" }
		i=i+1
	end

	i=0
	while i < nics() do
		nic=get_nic(i)
		nic:timer{ name='nic'..i, type="simple_nic" }
		i=i+1
	end

	mem=Memory{ name="main", latency=150  } 

    l2=Cache{ name="l2cache", size="512kB", 
		line_size=16, latency=20, num_sets=4, next=mem, 
		write_policy="WB", write_allocate="true" }
    t2=TLB{ name="l2tlb", page_size="4kB", 
		entries=512, latency=80, num_sets=4, next=mem, 
		write_policy="WB", write_allocate="true" }

	i=0
	while i < cpus() do
	    cpu=get_cpu(i)
	    -- cpu:timer{ name='cpu'..i, type="timer_dep" }
	    cpu:timer{ name='cpu'..i, type="timer1" }
        ic=Cache{ name="icache"..i, size="16kB", line_size=16, 
		    latency=0, num_sets=2, next=l2, 
		    write_policy="WT", write_allocate="false" }
        dc=Cache{ name="dcache"..i, size="16kB", line_size=16, 
		    latency=0, num_sets=2, next=l2, 
		    write_policy="WT", write_allocate="false" }
        it=TLB{ name="itlb"..i, page_size="4kB", entries=40, 
		    latency=0, num_sets=40, next=t2,	
		    write_policy="WT", write_allocate="false" }
        dt=TLB{ name="dtlb"..i, page_size="4kB", entries=40, 
		    latency=0, num_sets=40, next=t2, 
		    write_policy="WT", write_allocate="false" }
    
	    cpu:instruction_cache(ic)
	    cpu:data_cache(dc)
	    cpu:instruction_tlb(it)
	    cpu:data_tlb(dt)
		i=i+1
    end
end
-- vim:ft=lua
