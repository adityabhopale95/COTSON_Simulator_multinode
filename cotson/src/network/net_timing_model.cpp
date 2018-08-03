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

// $Id: net_timing_model.cpp 11 2010-01-08 13:20:58Z paolofrb $

#include "net_timing_model.h"
#include "kill.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
namespace po = boost::program_options;

TimingModel::TimingModel():verbose_(0) {}
TimingModel::~TimingModel() {}
void TimingModel::verbose(int v) { verbose_ = v; }

