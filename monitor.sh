#!/bin/sh

set -x

#sudo dmesg -c

#sudo rmmod iwlwifi iwlmvm mac80211 cfg80211 compat

#sleep 1s

#sudo modprobe iwlwifi amsdu_size=3

#sleep 3s

#sudo modprobe iwlmvm

#sleep 3s

#iw dev

#iw phy

#sleep 3s

sudo iw phy phy0 interface add mon0 type monitor

sleep 3s

sudo iw dev wlp4s0 del
#sudo ifconfig wlp4s0 down

sleep 3s

sudo ifconfig mon0 up

sleep 3s

iwconfig mon0

sleep 3s

sudo tcpdump -i mon0
