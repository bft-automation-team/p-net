#!/bin/sh

cd /home/automation-dell/profinet
sudo /home/automation-dell/profinet/build/pn_dev -vvvvv -i enp7s0 -a /home/automation-dell/profinet/plexus_outputs_as_profinet_inputs.txt -b /home/automation-dell/profinet/pnet_heartbeat.txt
