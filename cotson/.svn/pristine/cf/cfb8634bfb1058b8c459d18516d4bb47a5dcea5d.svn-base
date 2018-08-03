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

// #define _DEBUG_THIS_

#include <abaeterno_config.h>
#include "liboptions.h"
#include "predictor.h"

using namespace std;

namespace {
option o3("tpredictor.a0",      "1.0", "AR param 0");
option o4("tpredictor.a1",      "0.0", "AR param 1");
option o5("tpredictor.m0",      "0.0", "MA param 0");
option o6("tpredictor.m1",      "0.0", "MA param 1");
}

double Predictor::predict(double x)
{
    observations.push_back(x);
    double px = observations.arma22(predictions, a0,a1,m0,m1);
    predictions.push_back(px);
    return px;
}

Predictor::Predictor(uint64_t id):
    observations(4,1.0),
    predictions(4,1.0),
    a0(Option::get<double>("tpredictor.a0",1.0)),
    a1(Option::get<double>("tpredictor.a1",0.0)),
    m0(Option::get<double>("tpredictor.m0",0.0)),
    m1(Option::get<double>("tpredictor.m1",0.0)),
	devid(id)
{ }

