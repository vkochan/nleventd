#! /bin/sh

if [ "$IS_UP" = "TRUE" ]
then
    status="UP";
else
    status="DOWN";
fi

if [ "$IS_RUNNING" = "TRUE" ]
then
    status="$status, RUNNING"
fi

echo "Interface: $IF"
echo "Status: $status"
echo "MAC: $ADDRESS"
echo "Broadcast: $BROADCAST"
echo "MTU: $MTU"
echo ""
