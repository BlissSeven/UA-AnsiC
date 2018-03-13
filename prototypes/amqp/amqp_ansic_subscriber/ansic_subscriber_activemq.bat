@echo off
setlocal

set SUBSCRIBER=amqp_ansic_subscriber.exe

set BROKER=amqps://opcfoundation-prototyping.org
set USERNAME=receiver
set PASSWORD=password
set AMQPNODENAME=topic://Topic1

echo -b %BROKER% -u %USERNAME% -p %PASSWORD% -t %AMQPNODENAME% 
%SUBSCRIBER% -b %BROKER% -u %USERNAME% -p %PASSWORD% -t %AMQPNODENAME% 