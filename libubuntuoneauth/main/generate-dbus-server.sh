#!/bin/bash

qdbusxml2cpp -c UserAdaptor -a user_adaptor.h:user_adaptor.cpp com.ubuntu.sso.User.xml
echo "UserAdapter generated..."

