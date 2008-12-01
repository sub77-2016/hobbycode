#!/bin/bash
find . -iname "*.h" -exec perl -ni -e 'chomp; print "$_\n"' {} \;

