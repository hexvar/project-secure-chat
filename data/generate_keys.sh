#!/bin/sh

SIZE=4096
CIPHER="-aes256"
PASSOUT="-passout pass:password"
PASSIN="-passin pass:password"

genkey()
{
    openssl genrsa $2  $3 -out $1.pem $SIZE
    openssl rsa $4 -in $1.pem -pubout > $1.pub
}

mkdir -p keys
genkey "server"

genkey "client_viralex" $CIPHER "$PASSOUT" "$PASSIN"
genkey "client_paradox" $CIPHER "$PASSOUT" "$PASSIN"
genkey "client_alec"    $CIPHER "$PASSOUT" "$PASSIN"

genkey "client_gufo"    $CIPHER "$PASSOUT" "$PASSIN"
genkey "client_daniele" $CIPHER "$PASSOUT" "$PASSIN"

PASSOUT="-passout pass:badwolf"
PASSIN="-passin pass:badwolf"
genkey "client_the_doctor"  $CIPHER "$PASSOUT" "$PASSIN"


mv *.pem *.pub keys/
