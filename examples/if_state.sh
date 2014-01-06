#! /bin/sh

echo "From $0: device: $NLMSG_IFNAME"

if [ "$NLMSG_IFF_UP" == "TRUE" ];
then
    echo "status: UP";
else
    echo "status: DOWN";
fi

echo "adress: $NLMSG_ADDRESS"
