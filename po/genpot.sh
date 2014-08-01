#!/bin/sh

XGETTEXT_ARGS="--keyword=tr --keyword=tr:1,2 --keyword=dtr:2 --keyword=dtr:2,3" XGETTEXT=$1 srcdir=$2 $3 --gettext-package $4 --pot

