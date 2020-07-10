/*
 * Poisson Traffic Generator (for NS2 2.26)
 *
 * To install:
 *    1. copy this file to tools/poisson.c
 *    2. add poisson.o in the Makefile
 *    3. make
 *
 * To use this generator: (example)
 *    set udp(0) [new Agent/UDP]
 *    $ns attach-agent $n1 $udp(0)
 *    set null(0) [new Agent/Null]
 *    $ns attach-agent $n2 $null(0)
 *    set poisson(0) [new Application/Traffic/Poisson]
 *    $poisson(0) set packetSize_ 1500
 *    $poisson(0) set rate_ 1Mb
 *    $poisson(0) attach-agent $udp(0)
 *    $ns connect $udp(0) $null(0)
 *    $ns at 0.3 "$poisson(0) start"
 * 
 *
 * Ling-Jyh Chen (cclljj@cs.ucla.edu), Feb. 22, 2005
 */
#include <stdlib.h>
#include <math.h>
 
#include "random.h"
#include "trafgen.h"
#include "ranvar.h"
class Poisson_Traffic : public TrafficGenerator {
 public:
        Poisson_Traffic();
        virtual void timeout();
        virtual double next_interval(int&);
        int command(int argc, const char*const* argv);
 protected:
        void init();
        double lambda;
        double rate_;     /* send rate during on time (bps) */
};
static class PoissonTrafficClass : public TclClass {
 public:
        PoissonTrafficClass() : TclClass("Application/Traffic/Poisson") {}
        TclObject* create(int, const char*const*) {
                return (new Poisson_Traffic());
        }
} class_poisson_traffic;
int Poisson_Traffic::command(int argc, const char*const* argv){
        return Application::command(argc,argv);
}
Poisson_Traffic::Poisson_Traffic() 
{
        bind_bw("rate_", &rate_);
        bind("packetSize_", &size_);
}
void Poisson_Traffic::init()
{
        if (agent_)
                agent_->set_pkttype(PT_EXP);
        lambda = (double)rate_ / (double)(size_ << 3);
        nextPkttime_ = next_interval(size_);
}
double Poisson_Traffic::next_interval(int& size)
{
double U;
        U = drand48();
        do {
                U = drand48();
        } while (U<=0 || U>1);
        return ( 0 - (log(U)/lambda) );
}
void Poisson_Traffic::timeout()
{
        if (! running_)
                return;
        agent_->sendmsg(size_);
        nextPkttime_ = next_interval(size_);
        timer_.resched(nextPkttime_);
}
