#!/bin/sh

sudo kill -9 `ps -e | grep lisod | xargs | cut -d' ' -f1`
