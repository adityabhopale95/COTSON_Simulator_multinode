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

// $Id: predictor.h 11 2010-01-08 13:20:58Z paolofrb $

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "circbuf.h"
#include "series.h"

class Predictor 
{
public: 
    Predictor(uint64_t);
    ~Predictor() { }
    double predict(double);

private:
    Series<double> observations; 
    Series<double> predictions;

    // ARMA model
    const double a0, a1, m0, m1;
	const uint64_t devid;
};

#endif
