// (C) Copyright 2015 Hewlett-Packard Development Company, L.P.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

#include <zlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <getopt.h>
#include <boost/format.hpp>
#include "read_gzip.h"

using namespace std;
using namespace boost;

namespace memtrace { // anonymous
    int debug = 0;
    uint64_t t_start = 0;
    uint64_t t_end = numeric_limits<uint64_t>::max();
    const char* gfile = 0;
    const int MAXCPU = 128;
    const int LINESZ = 64;
    const int EPOCHSZ = 40000000; // max events per epoch
    uint64_t tdelta = 1000;
    uint64_t ts0 = 0; // smallest timestamp
	uint64_t evn_tot = 0;
    ofstream gout;

    struct event {
        // IMPORTANT: keep in sync with timer_cpu/memory_tracer.cpp
        uint64_t ts;
        uint64_t addr;
        uint16_t cr3;
        int cpu;
        bool w;
        event():ts(0),addr(0),cr3(0),cpu(0),w(false) {}
        bool is_epoch_marker() { return cpu==0 && addr==0 && cr3==0; }
    };

    struct mstat {
        uint64_t nr;
        uint64_t nw;
        inline void dump(const char* s, int n) {
            if (nr > 0)
                cout << boost::format("==== %s %5d R %10d W %10d\n") % s % n % nr % nw;
        }
        inline void update(const event& e) { 
		    if (e.w) nw++; else nr++; 
		}
    };

    inline ReadGzip& operator>> (ReadGzip& gz, event& t)
    {
        uint8_t tmp;
        gz >> t.ts; 
        gz >> t.addr;
        gz >> t.cr3;
        gz >> tmp;
        t.cpu = tmp>>1;
        t.w = tmp&0x1;
        return gz;
    }

    inline ostream& operator<< (ostream& s, const event& e) 
    {
        const char* rw = e.w ? "W" : "R";
        s << boost::format("%10llu %s 0x%016X %8d [%d]") % e.ts % rw % e.addr % e.cr3 % e.cpu;
        return s;
    }

    inline double MBps (uint64_t b, uint64_t ns) 
    {
        return ((double)b * (1000000000/(1024*1024))) / (double)ns; 
    }

    mstat cstats[MAXCPU];
    event *epoch0, *epoch1;

} // namespace memtrace

using namespace memtrace;

static void usage(const char * s)
{
    cerr << "Usage: " << s << " tracefile.gz [flags]" << endl;
    cerr << "    -d         increase debug level" << endl;
    cerr << "    -t0 t      start time" << endl;
    cerr << "    -t1 t      end time" << endl;
    cerr << "    -g file    write time/bw graph file" << endl;
    cerr << "    -td delta  delta time (for time graphs)" << endl;
    exit(1);
}

static int evcmp(const void *p1, const void *p2)
{
    const event *e1 = static_cast<const event*>(p1);
    const event *e2 = static_cast<const event*>(p2);
    return (e1->ts == e2->ts) ? e1->cpu - e2->cpu : e1->ts - e2->ts;
}

static void report(const event* epoch, int evn, const char *s)
{
    cout << endl << "=== " << s << " " << epoch[0].ts << " " << evn_tot << endl;
    uint64_t trd = 0;
    uint64_t twr = 0;
    for (int i=0; i<MAXCPU; ++i) {
        trd += cstats[i].nr;
        twr += cstats[i].nw;
        cstats[i].dump("CPU",i);
    }
    double nsec = (double)(epoch[evn-1].ts-ts0); // total time in ns
    double rbw = (double)(trd * LINESZ * 1024)/nsec;
    double wbw = (double)(twr * LINESZ * 1024)/nsec;
    cout << "Time (msec)         " << nsec*1e-6 << endl;
    cout << "GB read             " << trd * LINESZ/(1024*1024) << endl;
    cout << "GB written          " << twr * LINESZ/(1024*1024) << endl;
    cout << "Read bw avg (MB/s)  " << rbw << endl;
    cout << "Write bw avg (MB/s) " << wbw << endl;
}

static void bwgraph(const event& e)
{
    static uint64_t tdcur = 0;
    static uint64_t rlines = 0;
    static uint64_t wlines = 0;

    uint64_t td = e.ts/tdelta;
    if (td <= tdcur) {
        if (e.w)
            wlines++;
        else
            rlines++;
    }
    else {
        double rbw = MBps(rlines*LINESZ,tdelta); // MB/s
        double wbw = MBps(wlines*LINESZ,tdelta); // MB/s
        double ms = (double)tdcur*tdelta*1e-6;
        gout << ms << " " << rbw << " " << wbw << endl;
        tdcur = td;
        rlines = wlines = 0; 
    }
}

static void process_epoch(event* epoch, int evn, bool last)
{
	evn_tot += evn;
    qsort(epoch,evn,sizeof(event),evcmp); // sort by timestamp
    if (!ts0) ts0 = epoch[0].ts; // initial timestamp

    for (int i = 0; i < evn; ++i) {
        const event& e = epoch[i];
        cstats[e.cpu].update(e);
        if (debug>=2) 
            cout << e << endl;
        if (gfile)
            bwgraph(e);
    }
    if (last)
        report(epoch, evn,"FINAL");
    else if(debug>=1)
        report(epoch, evn,"EPOCH");
}

static void parse_trace(ReadGzip& trace, event *epoch)
{
    int nt = 0;
    int evx = 0;
    while (!trace.eof()) {
        trace >> epoch[evx++];
        if (evx >= EPOCHSZ) {
            cerr << "ERROR: increase EPOCHSZ (evx " << evx << ")" << endl;
            exit(1);
        }
        if (epoch[evx-1].is_epoch_marker() && evx > 1) {
            process_epoch(epoch,evx-1,false);
            evx=0;
        }
        if (nt++ % (1024*1024) == 0) cout << nt/(1024*1024) << " " << flush;
    }
    if (evx>1)
        process_epoch(epoch,evx-1,true);
}

int main(int argc, char *argv[]) 
{
    if (argc < 2)
        usage(argv[0]);
    ReadGzip trace(argv[1], 1<<20); // 1MB buffer
    for(int i = 2; i<argc; ++i) {
         if (! ::strcmp(argv[i],"-d")) {
             debug++;
         }
         else if (! ::strcmp(argv[i],"-t0")) {
             t_start = ::atoll(argv[++i]);
             cout << "T_start " << t_start << endl;
         }
         else if (! ::strcmp(argv[i],"-t1")) {
             t_end = ::atoll(argv[++i]);
             cout << "T_end " << t_end << endl;
         }
         else if (! ::strcmp(argv[i],"-td")) {
             tdelta = ::atoll(argv[++i]);
             cout << "Tdelta " << tdelta << endl;
         }
         else if (! ::strcmp(argv[i],"-g")) {
             gfile = argv[++i];
             gout.open(gfile);
             cout << "graph file " << gfile << endl;
         }
         else usage(argv[0]);
    }

    if (!trace) {
        cerr << "Error: cannot open tracefile '" << argv[1] << "'" << endl;
        exit(2);
    }

	event* epoch0 = new event[EPOCHSZ];
	parse_trace(trace,epoch0);
	delete[] epoch0;

    if (gfile)
        gout.close();
    return 0;
}

