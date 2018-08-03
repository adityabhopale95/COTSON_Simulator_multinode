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

// $Id: mediator_server.cpp 144 2010-11-08 20:12:34Z paolofrb $

#include "mediator_server.h"
#include "liboptions.h"
#include "log.h"

#include <unistd.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace boost;

mutex io_mutex;

MediatorServer::MediatorServer(uint16_t ctrl_port,
                               uint16_t data_port,
                               const char* slirp_network,
                               const char* mcast_ip,
                               uint16_t mcast_port,
                               uint8_t mcast_ttl,
                               const char *vde_file,
                               uint32_t mac_base,
                               double qmin, double qmax, 
                               double qup, double qdown,
                               bool force_queue,
                               const std::vector<uint32_t>& slirp_redir_node,
                               const std::vector<uint16_t>& slirp_redir_port1,
                               const std::vector<uint16_t>& slirp_redir_port2,
                               const char* tracefile,
                               int verbose)

        :   switch_(&data_processor_,&scheduler_,mac_base,
                    mcast_port,mcast_ttl,mcast_ip,
                    qmin,qmax,qup,qdown,
                    force_queue,tracefile,
                    verbose),
            data_reactor_(),
            ctrl_reactor_(),
            control_processor_(&switch_,ctrl_port, data_port, verbose),
            data_processor_(&switch_,verbose),
            control_acceptor_(&ctrl_reactor_, &control_processor_),
            data_acceptor_(&data_reactor_, &data_processor_),
            vde_acceptor_(&data_reactor_, &data_processor_),
            slirp_acceptor_(SlirpAcceptor::get()),
            scheduler_(&switch_, verbose),
            ctrl_port_(ctrl_port),
            data_port_(data_port),
            slirp_network_(slirp_network),
            slirp_redir_node_(slirp_redir_node),
            slirp_redir_port1_(slirp_redir_port1),
            slirp_redir_port2_(slirp_redir_port2),
            vde_file_(vde_file),
            verbose_(verbose)
{}

void MediatorServer::register_timing_model(TimingModel* timing)
{
    // Register a timing model
    switch_.register_timing_model(timing);
}

int MediatorServer::init()
{
    Stats::get().reset(); // Reset stats

    // Register the data socket acceptor, control socket acceptor and the VDE socket acceptor
    int pid = ::getpid();

    // Open a passive connection for control
    if (control_acceptor_.open_acceptor(ctrl_port_) == -1)
        return -1;

    // Open a passive connection for data
    if (data_acceptor_.open_acceptor(data_port_) == -1)
        return -1;

    // Set the data handler
    data_processor_.set_data_handler(&data_acceptor_);

    // Check for VDE and setup properly
    if (vde_file_) {
        // Open a passive connection for vde
        switch_.vde_on(false);
        if (vde_acceptor_.open_acceptor(vde_file_,pid) != -1) {
            // Set the vde handler
            data_processor_.set_vde_handler(&vde_acceptor_);
            switch_.vde_on(true);
        }
    }

    // Check for SLIRP and setup properly
    if (slirp_network_) {
        // Open a passive connection for slirp
        switch_.slirp_on(false);
        slirp_acceptor_.init(slirp_network_,&data_reactor_,&data_processor_);
        if (slirp_acceptor_.open_acceptor(pid) != -1) {
            // Set the slirp handler
            data_processor_.set_slirp_handler(&slirp_acceptor_);
            switch_.slirp_on(true);
        }
        for (unsigned i = 0; i < slirp_redir_node_.size(); ++i) {
            slirp_acceptor_.redir(
                slirp_redir_node_[i],
                slirp_redir_port1_[i],
                slirp_redir_port2_[i]);
        }
    }

    return 0;
}

void MediatorServer::Heartbeat()
{
    int hbms=Option::get<int>("sync_heartbeat"); 
    if (hbms<=0)
        return;
    int hbs = hbms/1000;
    int hbns = (hbms%1000)*1000000;
    timespec ts={1,0};
    while(true) {
        ts.tv_sec=hbs;
        ts.tv_nsec=hbns;
        ::nanosleep(&ts,0);
        if (switch_.sync_started()) {
            LOG1("(SYNC) HEARTBEAT", switch_.GT(), switch_.nextGT());
            switch_.timeout();
		}
    }
}

int MediatorServer::run()
{
    // Enter in the reactors never ending loop
    thread ct(bind(&Reactor::handle_events,ctrl_reactor_));
    thread dt(bind(&Reactor::handle_events,data_reactor_));
    thread ht(bind(&MediatorServer::Heartbeat,this));
    ct.join();
    dt.join();
    ht.join();
    return 0;
}

/* vi: set sw=4 ts=4: */
