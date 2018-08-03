// -*- C++ -*-
// (C) Copyright 2006-2009 Hewlett-Packard Development Company, L.P.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

// $Id$
#include <zlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include "read_gzip.h"
#include "net_typedefs.h"

#define MAXNODES 200
enum pmode { LOG=1, NSTRACE=2, HIST=3, TIME=4, SVG=5, GP=6 };
using namespace std;

uint64_t packets[MAXNODES][MAXNODES];
uint64_t bytes[MAXNODES][MAXNODES];
int svglines = 0;

static void usage(const char * s)
{
    cerr << "Usage: " << s << " tracefile [packets_to_skip] [mode[:params]]" << endl;
    cerr << "    mode 1                           Readable log" << endl;
    cerr << "    mode 2                           NS-style trace" << endl;
    cerr << "    mode 3                           Generate node-node traffic histogram (as pgm image)" << endl;
    cerr << "    mode 4:tdelta[:tmin:tmax]        Generate time traffic data (global)" << endl;
    cerr << "    mode 5[:tmin:tmax]               Generate svg packet graph" << endl;
    cerr << "    mode 6[:tmin:tmax:minsz:maxsz]   Generate gnuplot vector data" << endl;
    exit(1);
}


#define YSCALE 40000
#define TLAT 10
#define NODEH 2000
void line(uint64_t t, int src, int dst)
{
    double x1 = t/1000.0;
    double x2 = (t+TLAT)/1000.0;
    double y1 = (src * YSCALE)/1000.0;
    double y2 = (dst * YSCALE)/1000.0;
    cout << "<line";
    cout << " x1=\"" << x1 << "\"";
    cout << " y1=\"" << y1 << "\"";
    cout << " x2=\"" << x2 << "\"";
    cout << " y2=\"" << y2 << "\"";
    cout << " style=\"stroke:#000000;stroke-width:1\" ";
    cout << " />" << endl;
    ++svglines;
}

// void rect (uint64_t w, uint64_t h, uint64_t x, uint64_t y)
void rect (uint64_t t, uint64_t n)
{
    double w = t/1000.0;
    double h = NODEH/1000.0;
    double x = 0;
    double y = (n * YSCALE)/1000.0;
    cout << "<rect style=\"fill:#0000ff\"";
    cout << " width=\"" << w << "\"";
    cout << " height=\"" << h << "\"";
    cout << " x=\"" << x << "\"";
    cout << " y=\"" << y << "\"";
    cout << " />" << endl;
}

int main(int argc, char *argv[]) 
{
    int skip = 0;
    int maxnode = 0;
    pmode mode = LOG;
    if (argc < 2)
        usage(argv[0]);

    ReadGzip trace(argv[1]);
    if (!trace) {
        cerr << "Error: cannot open tracefile '" << argv[1] << "'" << endl;
        exit(2);
    }
    if (argc >= 3)
         skip = ::atoi(argv[2]);

    uint64_t tmin = 0;
    uint64_t tmax = (uint64_t)-1;
    uint64_t minsz = 0;
    uint64_t maxsz = (uint64_t)-1;
    uint64_t tlast = 0;
    uint64_t tdelta = 10;
    if (argc == 4) {
        const char *q = ::strtok(argv[3],":"); // qmin
        if (!q) usage(argv[0]); // mandatory
        mode = (pmode)::strtol(q,0,0);  

        if (mode==TIME) {
            q = ::strtok(0,":"); // tdelta
            if (!q) usage(argv[0]); // mandatory
            tdelta = ::strtoull(q,0,0); 
            if (q) {
                q = ::strtok(0,":"); // tmin (opt)
                if (q) tmin = ::strtoull(q,0,0); 
            }
            if (q) {
                q = ::strtok(0,":"); // tmax (opt)
                if (q) tmax = ::strtoull(q,0,0); 
            }
        }
        if (mode==SVG || mode==GP) {
            q = ::strtok(0,":"); // tmin (opt)
            if (q) tmin = ::strtoull(q,0,0); 
            if (q) {
                q = ::strtok(0,":"); // tmax (opt)
                if (q) tmax = ::strtoull(q,0,0); 
            }
            if (q) {
                q = ::strtok(0,":"); // minsz (opt)
                if (q) minsz = ::strtoull(q,0,0); 
            }
            if (q) {
                q = ::strtok(0,":"); // maxsz (opt)
                if (q) maxsz = ::strtoull(q,0,0); 
            }
        }
    }

    int n = 0;
    if (skip && mode==LOG)
        cout << "Skipping " << skip << " packets" << endl;

    if (mode==SVG) {
        cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << endl;
        cout << "<svg>" << endl;
        cout << "<g>" << endl;
    }

    uint64_t tcur = 0;
    uint64_t tpackets = 0;
    uint64_t tbytes = 0;
    while (trace) {

        if (trace.eof()) break; //avoid to call eof if trace is 0
        // IMPORTANT: keep in sync with Switch.cpp
        uint64_t tstamp;
        MacAddress src;
        MacAddress dst;
        uint32_t len;
        uint32_t lat;
        double q;

        trace >> tstamp;
        if (trace.eof()) break; //avoid to print garbage
        trace >> src;
        trace >> dst;
        trace >> len;
        trace >> q;
        trace >> lat;

        if (n > skip) {
            switch(mode) {
                case LOG:
                    cout << "[" << n << "]"
                         << " time " << tstamp
                         << " from " << src.str()
                         << " to " << dst.str()
                         << " len " << len
                         << endl;
                    break;

                case NSTRACE: {
                    int s0 = src.byte(5);
                    int d0 = dst.byte(5);
                    if (s0 < 255 && d0 < 255) {
                        cout << "r " << (double)tstamp/1000.0 << " "
                             << s0 << " " << d0 << " "
                             << "tcp "
                             << len << " "
                             << "------- 1 "
                             << s0 << ".0 " << d0 << ".0 "
                             << "0 "
                             << n
                             << endl;
                    }
                    break;
                }

                case HIST: {
                    int s0 = src.byte(5);
                    int d0 = dst.byte(5);
                    if (s0 < MAXNODES && d0 < MAXNODES) {
                        if (s0 > maxnode) maxnode = s0;
                        if (d0 > maxnode) maxnode = d0;
                        packets[s0][d0]++;
                        bytes[s0][d0]+=len;
                    }
                    if ((n % 1000000) == 0)
                        cerr << n/1000000 << " " << flush;
                    break;
                }

                case TIME: {
                   if (!tcur) tcur = tstamp; // Initialize

                   if (tcur <= tstamp && tstamp < tcur+tdelta) {
                       tpackets++;
                       tbytes += len;
                   }
                   else {
                      if (tpackets && tmin < tcur && tcur < tmax)
                          cout << tcur << " " 
                               << tpackets << " " 
                               << tbytes << endl;
                      tcur = tstamp;
                      tpackets = 1;
                      tbytes = len;
                   }
                   break;
                }

                case SVG: {
                    if (tmin <= tstamp && tstamp < tmax) {
                        int s = src.byte(5);
                        int d = dst.byte(5);
                        if (s > 0 && s < 255 && d > 0 && d < 255) {
                            if (s > maxnode) maxnode = s;
                            if (d > maxnode) maxnode = d;
                            line(tstamp-tmin,s,d);
                        }
                        if (tstamp > tlast) tlast = tstamp;
                    }
                    break;
                }

                case GP: {
                    if (tmin <= tstamp && tstamp < tmax && minsz <= len && len <= maxsz) {
                        int s = src.byte(5);
                        int d = dst.byte(5);
                        if (s > 0 && s < 255 && d > 0 && d < 255) {
                            if (s > maxnode) maxnode = s;
                            if (d > maxnode) maxnode = d;
                            // x y xd yd
                            cout << tstamp << " " << s << " " << 10 << " " << d-s << endl;
                        }
                    }
                    break;
                }

                default:
                    usage(argv[0]);
                    break;
            }
        }
        n++;
    }

    if (mode==HIST) {
        uint64_t maxp = 0;
        uint64_t maxb = 0;
        for (int i = 0; i < maxnode; ++i) 
            for (int j = 0; j < maxnode; ++j) {
                uint64_t p = packets[i][j];
                uint64_t b = bytes[i][j];
                if (p > maxp) maxp = p;
                if (b > maxb) maxb = b;
            }

        cout << "P2" << endl;
        cout << "# packets" << endl;
        cout << maxnode << " " << maxnode << endl;
        cout << maxp << endl;
        for (int i = 0; i < maxnode; ++i) {
            for (int j = 0; j < maxnode; ++j) {
                cout << " " << packets[i][j];
                if (((j+1)%10) == 0) cout << endl;
            }
            cout << endl;
        }

        cout << "P2" << endl;
        cout << "# bytes" << endl;
        cout << maxnode << " " << maxnode << endl;
        cout << maxb/1024 << endl;
        for (int i = 0; i < maxnode; ++i) {
            for (int j = 0; j < maxnode; ++j) {
                cout << " " << bytes[i][j]/1024;
                if (((j+1)%10) == 0) cout << endl;
            }
            cout << endl;
        }
    }

    if (mode==SVG) {
        for (int i = 1; i <= maxnode; ++i)
           rect(tlast-tmin,i);
        cout << "<!-- " << svglines << " packets -->" << endl;
        cerr << "packets: " << svglines << endl;
        cout << "</g>" << endl;
        cout << "</svg>" << endl;
    }
    if (mode==GP) {
        ofstream s("nodes.gp");
        s << "set key off\n";
        s << "unset ytics\n";
        s << "unset border\n";
        s << "unset bars\n";
        s << "plot ";
        for (int i = 1; i <= maxnode; ++i)
           s << i << " with dots, ";
        s << " 'nodes.dat' with vectors\n";
        s << " pause -1\n";
        s.close();
    }
    return 0;
}

