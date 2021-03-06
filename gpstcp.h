/*
    This file is part of Kismet

    Kismet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Kismet is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Kismet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __GPSTCP_H__
#define __GPSTCP_H__

#include "config.h"

#include "kis_gps.h"
#include "timetracker.h"
#include "buffer_handler.h"
#include "ringbuf2.h"
#include "globalregistry.h"
#include "tcpclient2.h"
#include "serialclient2.h"
#include "pollabletracker.h"
#include "gpsnmea.h"

// Raw NMEA over TCP, as served from many phone apps

class GPSTCP : public GPSNMEA {
public:
    GPSTCP(GlobalRegistry *in_globalreg, SharedGpsBuilder in_builder);
    virtual ~GPSTCP();

    // BufferInterface API - buffer available implemented in gpsnmea
    virtual void BufferError(std::string error);

    virtual bool open_gps(std::string in_opts);

    virtual bool get_location_valid();

    virtual bool get_device_connected();

protected:
    time_t error_reconnect_timer;

    std::shared_ptr<PollableTracker> pollabletracker;
   
    std::shared_ptr<TcpClientV2> tcpclient;

    std::string host;
    unsigned int port;

    // Have we ever seen data from the device?
    bool ever_seen_gps;

    // Last time we calculated the heading, don't do it more than once every 
    // few seconds or we get nasty noise
    time_t last_heading_time;

    // Decaying reconnection algorithm
    int reconnect_tid;
    int num_reconnects;
    static int time_event_reconnect(TIMEEVENT_PARMS);
};

class GPSTCPBuilder : public KisGpsBuilder {
public:
    GPSTCPBuilder(GlobalRegistry *in_globalreg) : 
        KisGpsBuilder(in_globalreg, 0) { 
        initialize();
    }

    virtual void initialize() {
        set_int_gps_class("tcp");
        set_int_gps_class_description("Raw NMEA over TCP (typically from a smartphone app)");
        set_int_gps_priority(-1000);
        set_int_default_name("tcp");
        set_int_singleton(false);
    }

    virtual SharedGps build_gps(SharedGpsBuilder in_builder) {
        return SharedGps(new GPSTCP(globalreg, in_builder));
    }
};


#endif

