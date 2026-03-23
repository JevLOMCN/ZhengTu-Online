#!/bin/bash
rm -f /tmp/*.log*
./SuperServer &
sleep 5
./FLServer &
sleep 5
./RecordServer &
sleep 5
./BillServer &
sleep 5
./MiniServer &
sleep 8
./SessionServer &
sleep 15
./ScenesServer &
#sleep 10
#./ScenesServer &
sleep 15
./GatewayServer &
