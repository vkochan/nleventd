#! /bin/sh

if [ "$NLMSG_IS_UP" = "TRUE" ]
then
    status="UP";
else
    status="DOWN";
fi

if [ "$NLMSG_IS_RUNNING" = "TRUE" ]
then
    status="$status, RUNNING"
fi

echo "Interface: $NLMSG_IFNAME"
echo "Status: $status"
echo "MAC: $NLMSG_ADDRESS"
echo "Broadcast: $NLMSG_BROADCAST"
echo "MTU: $NLMSG_MTU"
echo ""
