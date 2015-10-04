# !/bin/bash
#
# This script sets the qRAE version and release date
#
# Copyright (C) 2015 Javier Llorente <javier@opensuse.org>
#

SPEC="../qrae.spec"
SRCPRO="../src/src.pro"
CONFIGXML="../installer/config/config.xml"
PACKAGEXML="../installer/packages/com.javierllorente/meta/package.xml"
DATE=`date +%Y-%m-%d`

if [ -z "$1" ]; then
  echo "Usage: setversion.sh <version>"
  exit 1;
fi

# qrae.spec
sed -i -e "/^ *Version:/ s/ [^ ]*$/ $1/" $SPEC

# src.pro
sed -i -e "/^ *VERSION =/ s/ [^ ]*$/ $1/" $SRCPRO

# config.xml
sed -i -e "s%\(<Version>\).*\(</Version>\)%\1$1\2%" $CONFIGXML

# package.xml
sed -i -e "s%\(<Version>\).*\(-1</Version>\)%\1$1\2%" $PACKAGEXML
sed -i -e "s%\(<ReleaseDate>\).*\(</ReleaseDate>\)%\1$DATE\2%" $PACKAGEXML