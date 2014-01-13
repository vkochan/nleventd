#! /bin/sh

if [ "$NL_EVENT" = "NEWADDR" ]
then
    ADDR_OP="ADD"
    DIRECT="to"
else
    ADDR_OP="DEL"
    DIRECT="from"
fi

echo "Address $ADDR_OP: <$NL_ADDRESS/$NL_PREFIXLEN, $NL_SCOPE> $DIRECT $NL_IF interface"
echo ""
