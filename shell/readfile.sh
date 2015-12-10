#!/bin/bash

exec 3>>log.txt

echo appended newline >&3
