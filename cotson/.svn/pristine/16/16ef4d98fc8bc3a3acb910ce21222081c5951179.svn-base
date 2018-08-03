#!/bin/bash
#  Copyright (c) 2016-2018 Roberto Giorgi - University of Siena

#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:

#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.

#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#
#  $Id$


thr="5" # 5% of tolerance
EXAMPLES="\
	mem_tracer mem_tracer2 trace_stats  \
	one_cpu_simple smarts dynamic multiple_cpu_interval \
	twonodes twohet \
	tracer \
"
okcount="0"
[ -z "$1" ] || { EXAMPLES="$1"; MYEXM="_$1"; }
refcount="0"
for e in $EXAMPLES; do refcount=`expr $refcount + 1`; done

#--------------------------------------------
function intolerance() {
   local f="$1"
   local str=`echo "$ref" $f|awk '{print $1}'`
   local vref=`echo "$ref" $f|awk '{print $2}'`
   line=`egrep "^$str" $f`
   local val1=`echo "$line"| awk '{print $2}'`
   echo "vref=$vref  -- val1=$val1"
   retv=`echo "define abs(x) {if (x<0) {return -x}; return x;}  define tol(vref,v,thr) { return(abs(v-vref)/vref*100<thr); } scale=5; tol($vref,$val1,$thr);"|bc -l`
   if [ "$retv" = "1" ]; then line="$ref"; fi
}

#
function check_outcome() {
exm="${1}"
echo "----------------------------------------------------"
echo "Example: '$exm' ..."
case $1 in
   mem_tracer)
      ref="    361821 r 0xFFFFFFFF81019541 0xFFFFFFFF81019541 [Trace:[cpu0(icache R)(l2cache R)(bus R)]]"
      f=`ls -tr /tmp/cotson_${exm}_*|tail -1`
      line=`zless $f|tail -1`
   ;;
   mem_tracer2)
      ref=" 163333317 r 0x0000000000000000 0x0000000000000000 [0]"
      f=`ls -tr /tmp/cotson_${exm}_*|tail -1`
      line=`zless $f|tail -1`
   ;;
   trace_stats)
      ref="0e4c5000 00000000010194a8 (02) e6 ed                    OUT 0xed, AL"
      f=`ls -tr /tmp/cotson_${exm}_*|tail -1`
      line=`zless $f|tail -1`
   ;;
   one_cpu_simple)
      ref="cpu0.timer.cycles                                           12001109"
      f=node.1.${exm}.log
      intolerance $f
   ;;
   smarts)
      ref="cpu0.timer.cycles                                           5770467"
      f=node.1.${exm}.log
      intolerance $f
   ;;
   dynamic)
      ref="cpu0.timer.cycles                                           3787410"
      f=node.1.${exm}.log
      intolerance $f
   ;;
   multiple_cpu_interval)
      ref="cpu0.timer.cycles                                           1332920"
      f=node.1.${exm}.log
      intolerance $f
   ;;
   tracer)
      exm="cotson_${exm}"
#      ref="cpu0.timer.cycles                                           2910841"
      ref="cpu0.timer.ipc                                              0.946147"
      f=${1}/node.1.${exm}.log
      intolerance $f
#      ref="called with 1234 4 32751632"
#      f=tracer/node.1.stdout.log
#      line=`tail -1 $f|sed 's/^[^ ]\+ \(.*\)0x.*/\1/'`
   ;;
   twonodes)
      exm="${exm}1"
      ref="time - from fa:cd:1:0:0 to fa:cd:1:0:0 len 98"
      f=`ls -tr /tmp/cotson_${exm}_*gz|tail -1`
      line=`../tools/traceplayer $f 0 1|tail -2|head -1|sed 's/time [0-9]\+ /time - /'|sed 's/^\[.*\] //'|sed 's/1:0:0:[12]/1:0:0/g'`
   ;;
   twohet)
      ref="time - from fa:cd:1:0:0:1 to fa:cd:1:0:0:2 len 98"
      f=`ls -tr /tmp/cotson_${exm}_*gz|tail -1`
      line=`../tools/traceplayer $f 0 1|tail -2|head -1|sed 's/time [0-9]\+ /time - /'|sed 's/^\[.*\] //'`
   ;;
   *)
      echo "ERROR: Unknown example '$exm'."
      exit 1
   ;;
esac
echo "ref='$ref'"
echo "line='$line'"
[ "$line" = "$ref" ] && { echo "$exm: SUCCESS"; okcount=`expr $okcount + 1`; } || echo "$exm: FAIL" 
}
#--------------------------------------------

# Do a non-interactive run of all main examples
NOINT=1 make run$MYEXM -e


# Check the outcome of those main examples
for app in $EXAMPLES; do
   check_outcome $app
done
[ "$okcount" = "$refcount" ] && success="1" || success="0"

# Cleanup
[ -z "$MYEXM" -a "$success" = "1" ] && make clean

# Printout summary
if [ "$success" = "1" ]; then
   echo "REGRESSION_SUCCESS ($okcount/$refcount)"
else
   echo "Regression_FAIL ($okcount/$refcount)"
fi
