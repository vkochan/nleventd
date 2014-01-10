#! /bin/sh

if [ "$NL_IS_UP" = "TRUE" ]
then
    status="UP";
else
    status="DOWN";
fi

if [ "$NL_IS_RUNNING" = "TRUE" ]
then
    status="$status, RUNNING"
fi

echo "Interface: $NL_IFNAME"
echo "Status: $status"
echo "MAC: $NL_ADDRESS"
echo "Broadcast: $NL_BROADCAST"
echo "MTU: $NL_MTU"
echo ""
