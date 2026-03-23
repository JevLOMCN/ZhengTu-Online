!#/bin/bash

pid=`ps aux|grep ./SuperServer|awk '{ print $2 }'`
kill -9 $pid
pid=`ps aux|grep ./FLServer|awk '{ print $2 }'`
kill -9 $pid
pid=`ps aux|grep ./roleRegServer|awk '{ print $2 }'`
kill -9 $pid
pid=`ps aux|grep ./InfoServer|awk '{ print $2 }'`
kill -9 $pid
