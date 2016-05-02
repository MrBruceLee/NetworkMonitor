#!/bin/bash

ping -c 1 -i 0.1 www.facebook.com
traceroute -q 1 www.facebook.com
