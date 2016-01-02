# !/bin/bash
#
# Migration to qdc
# This script migrates all users if run as root, 
# otherwise just migrates the current user.
#
# Copyright (C) 2015-2016 Javier Llorente <javier@opensuse.org>
#

USRDIR=/home

CONFIGDIR=".config/qdc"
OLDCONFIGFILE="Diccionario castellano de la RAE.conf"
NEWCONFIGFILE="qdc.conf"

OLDDATADIR=".local/share/data/qraeqRAE"
NEWDATADIR=".local/share/data/qdc"


function migrate() {
  if [ "$USR" = "" ]; then
    echo "No user found! Exiting..."
    exit 1;
  fi
  if [ -d "$USRDIR/$USR/$CONFIGDIR" ]; then
    echo User: $USR
    if [ -f "$USRDIR/$USR/$CONFIGDIR/$OLDCONFIGFILE" ]; then
      mv "$USRDIR/$USR/$CONFIGDIR/$OLDCONFIGFILE" "$USRDIR/$USR/$CONFIGDIR/$NEWCONFIGFILE"
      echo Config file migrated
    fi
  fi 
  if [ -d "$USRDIR/$USR/$OLDDATADIR" ]; then
    mv "$USRDIR/$USR/$OLDDATADIR" "$USRDIR/$USR/$NEWDATADIR"
    echo Data directory migrated
  fi
}

function migrate_allusers() {
for USR in $(ls $USRDIR)
do
  migrate
done
}

echo User directory: $USRDIR

if [ "$EUID" -ne 0 ]; then
  echo "Migrating the current user..."
  USR=$USER
  migrate
else
  echo "Migrating all users..."
  migrate_allusers
fi

