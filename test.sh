#!/usr/bin/env sh

touch pp.can
rm *.can
mkfifo sender2recver.can

./sender sender2recver.can &
./recver sender2recver.can &

