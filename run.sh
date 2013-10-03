#!/bin/sh

make && \
./lisod 9999 4443 test/logfile test/lockfile /var/www flaskr/flaskr.py ./hongchad.key ./hongchad.crt
