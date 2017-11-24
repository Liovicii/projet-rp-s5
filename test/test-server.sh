#!/bin/sh

#############################
# Tests du programme server #
#############################

. ./ftest.sh

#############################
# usage du programme        #
#############################
# nombre d'arguments
$V ./server                       2> $TMP >&2 || tu && fail "server: pas d'arg"
$V ./server arg                   2> $TMP >&2 || tu && fail "server: un seul arg"
$V ./server arg1 arg2 arg3        2> $TMP >&2 || tu && fail "server: trop d'args"
# numero de ports valide
$V ./server ::1 -1                2> $TMP >&2 || tu && fail "server: numero de port invalide"
$V ./server ::1 65537             2> $TMP >&2 || tu && fail "server: numero de port invalide"
# adresse IPv6 valide
$V ./server ::: 9000              2> $TMP >&2 || tu && fail "server: adresse IPv6 invalide"
$V ./server 2000::1::1 9000       2> $TMP >&2 || tu && fail "server: adresse IPv6 invalide"
$V ./server 0:0:0:0:0 9000        2> $TMP >&2 || tu && fail "server: adresse IPv6 invalide"

##############################
# configuration du serveur   #
##############################



rm -f $TMP

echo "Tests du programme server reussis !"

exit 0
