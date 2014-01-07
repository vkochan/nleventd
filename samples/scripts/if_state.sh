#! /bin/sh

if [ "$NLMSG_IS_UP" = "TRUE" ]
then
    status="UP";
else
    status="DOWN";
fi

echo "Interface: $NLMSG_IFNAME"
echo "Status: $status"
echo "HW adress: $NLMSG_ADDRESS"
echo "MTU: $NLMSG_MTU"
