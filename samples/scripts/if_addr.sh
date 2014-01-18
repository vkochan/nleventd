#! /bin/sh

if [ "$EVENT" = "NEWADDR" ]
then
    ADDR_OP="ADD"
    DIRECT="to"
else
    ADDR_OP="DEL"
    DIRECT="from"
fi

echo "Address $ADDR_OP: <$ADDRESS/$PREFIXLEN, $SCOPE> $DIRECT $IF interface"
echo ""
