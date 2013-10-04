#!/bin/sh

make && \
./lisod 9999 4443 /tmp/lisod.log /tmp/lisod.lock /var/www flaskr/flaskr.py ./hongchad.key ./hongchad.crt
