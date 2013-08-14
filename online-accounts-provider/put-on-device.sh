

scp -P 2222 *.qml phablet@127.0.0.1:/home/phablet/provider-temp

ssh -t -p 2222 phablet@127.0.0.1 sudo cp /home/phablet/provider-temp/*.qml /usr/share/accounts/qml-plugins/ubuntuone/
ssh -t -p 2222 phablet@127.0.0.1 ls -l --color /usr/share/accounts/qml-plugins/ubuntuone/
