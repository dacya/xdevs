#ifndef __proc_h_
#define __proc_h_
#include <cstdlib>
#include "lib/adevs/include/adevs.h"
#include "lib/adevs/include/adevs_digraph.h"
#include "Job.h"

/*
A processor requires a fixed period of time to service a job.
The processor can serve only one job at a time.  It the processor
is busy, it simply discards incoming jobs.
*/

class Processor: public adevs::Atomic<PortValue>
{
	private:	
		/// Model state variables
		double processing_time, sigma;
		Event job;
	public:
		/// Constructor.  The processing time is provided as an argument.
		Processor(double proc_time):
		adevs::Atomic<PortValue>(),
		processing_time(proc_time),
		sigma(DBL_MAX),
		job(nullptr)
		{
		}
		/// Internal transition function
		void delta_int()
		{
			// Done with the job, so set time of next event to infinity
			sigma = DBL_MAX;
			job = Event(nullptr);
		}
		/// External transition function
		void delta_ext(double e, const adevs::Bag<PortValue>& x)
		{
			// If we are waiting for a job
			if (job.get() == nullptr) 
			{
				job = (*(x.begin())).value;
				// Wait for the required processing time before outputting the
				// completed job
				sigma = processing_time;
			}
			// Otherwise, model just continues with time of next event unchanged
			else
			{
				sigma -= e;
			}
		}
		/// Confluent transition function.
		void delta_conf(const adevs::Bag<PortValue>& x)
		{
			// Discard the old job
			delta_int();
			// Process the incoming job
			delta_ext(0.0,x);
		}
		/// Output function.  
		void output_func(adevs::Bag<PortValue>& y)
		{
			// Produce a copy of the completed job on the out port
			PortValue pv(1,job);
			y.insert(pv);
		}
		/// Time advance function.
		double ta() { return sigma; }
		/**
		Garbage collection. No heap allocation in output_func, so
		do nothing.
		*/
		void gc_output(adevs::Bag<PortValue>& g){}
		/// Destructor
		~Processor()
		{
		}

};

#endif
