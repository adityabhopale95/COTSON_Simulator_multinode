// (C) Copyright 2006 Hewlett-Packard Development Company, L.P.
// HP Confidential
// $Id$

#include "aggregator.h"
#include "liboptions.h"

class SumAggregator : public Aggregator
{
public:
	SumAggregator(Parameters&);
	~SumAggregator();

protected:
	void do_first_beat();
	void do_beat();
	void do_end();
private:
	metric_storage storage;
};

registerClass<Aggregator,SumAggregator> sum_aggregator_c("sum");

SumAggregator::SumAggregator(Parameters&) {}
SumAggregator::~SumAggregator() {}

void SumAggregator::do_first_beat()
{
	storage=met->checkpoint();
}

void SumAggregator::do_beat()
{
	storage+=met->checkpoint();
}

void SumAggregator::do_end()
{
	met->restore(storage);
}
