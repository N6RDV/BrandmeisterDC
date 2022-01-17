# BrandmeisterDC
The prupose of this project is to let owners of the Pi-Star duplex MMDVM hotspots to disconnect from dynamic groups on either time slot whenever they need, regardless of presence of downlink traffic from BM on that time slot.

## Why bother?
Brandmeister (aka BM) used to drop dynamic groups on both time slots when someone called TG 4000 on either of two time slots.
This worked well for the woners of the personal duplex hotspots, allowing them to disconnect from the active talk group amid downlink traffic coming from BM on one time slot by calling TG 4000 on another time slot. It was an immediate advantage of duplex hotspor over simplex hotspot.

As of January 2022 this isn't the case any more. 
Owners and users of a full scale repeaters connected to BM weren't happy about such behaviour.
Users were often disconnected from dynamic talk group on one time slot when somene were calling TG 4000 on another timeslot.
From the repeater users perspective such behavior considered a bug and not surprisingly - it was fixed by BM team.

After the fix call of TG 4000 can only affect time slot on which call is sent to BM. This effectively downgrades duplex MMDVM hotspot to the convenience of a two simplex MMDVM hotspot working on two different time slots simultaneously.

The only remaining real benefit of a duplex MMDVM hotspot is ability to use it on two time slots independently. However, in most cases personal MMDVM hotspots are used by a single person. Thus having second time slot available isn't very useful.

At a same time duplex MMDVM hotspot isn't a match to a pair of simplex hotspots, since pair of simplex MMDVM hotspots can be used in two different DMR nets, or even in two different digital modes, whereas duplex can work only in a single mode at a time.

The main purpose of this project is to make dupex MMDVM hotspots worthfull for personal use again.

## How it works?
When MMDVM Host processes an incoming RF activity, it appends a new lines that look like this
```
Downlink Activate received from N6RDV
DMR Slot 1, received RF voice header from N6RDV to TG 4000
DMR Slot 1, received RF end of voice transmission from N6RDV to TG 4000, 0.4 seconds, BER: 0.1%, RSSI: -47/-47/-47 dBm
```
BranmeisterDC opens current MMDVM Host log file and waits for a line indicating an RF call to apear.

If the call is directed to TG 4000, BranmeisterDC drops all dynamic groups and interrupts QSO on a time slot on which call has been made by alling Branmeister API via HTTPS.
See the article [Using the BrandMeister API by Vladimir, AC2F](https://coloradodigital.net/2019/12/01/using-the-brandmeister-api/) for more details.

## Instructions
* [Setup](SETUP.md)
* [Usage](USAGE.md)
