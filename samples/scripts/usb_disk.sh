#!/bin/sh

if [ "$ACTION" = "add" ]; then
    echo "Added USB storage device /dev/$DEVNAME"
else
    echo "Removed USB storage device /dev/$DEVNAME"
fi

echo ""
