#! /bin/sh

SSH_DO="ssh -t -p 2222 phablet@127.0.0.1"

scp -P 2222 *.qml *.provider phablet@127.0.0.1:/home/phablet/provider-temp

$SSH_DO sudo cp /home/phablet/provider-temp/*.qml /usr/share/accounts/qml-plugins/ubuntuone/
$SSH_DO sudo cp /home/phablet/provider-temp/ubuntuone.provider /usr/share/accounts/providers/ubuntuone.provider
$SSH_DO ls -l --color /usr/share/accounts/qml-plugins/ubuntuone/
