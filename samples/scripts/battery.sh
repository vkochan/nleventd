#!/bin/sh

if [ "$POWER_SUPPLY_ONLINE" = "1" ]; then
    echo "$POWER_SUPPLY_NAME is connected"
else
    echo "$POWER_SUPPLY_NAME is disconnected"
fi

echo ""
