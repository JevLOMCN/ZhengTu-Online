!/bin/bash
rm -f /tmp/*.log*
SuperServer/SuperServer &
sleep 5
#FLServer/FLServer &
#sleep 5
RecordServer/RecordServer &
sleep 5
BillServer/BillServer &
sleep 5
MiniServer/MiniServer &
sleep 5
SessionServer/SessionServer &
sleep 10
ScenesServer/ScenesServer &
#sleep 10
#ScenesServer/ScenesServer &
sleep 10
GatewayServer/GatewayServer &
