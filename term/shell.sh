#!/bin/bash

`gnome-terminal -x ./broker1`

`gnome-terminal -x ./publish abc`
`gnome-terminal -x ./publish qwe`
`gnome-terminal -x ./subscribe abc`
`gnome-terminal -x ./subscribe qwe`

sleep 3
`gnome-terminal -x ./publish xyz`
sleep 2
`gnome-terminal -x ./subscribe xyz`
