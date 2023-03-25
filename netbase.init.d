#! /bin/sh
### BEGIN INIT INFO
# Provides:          netbase
# Required-Start:    $syslog
# Required-Stop:     $syslog
# Should-Start:      $network
# Should-Stop:       $network
# Default-Start:     4 5
# Default-Stop:      1
# Short-Description: Netbase Graph Database
# Description:       Manage the netbase graph database
#                    make it's web api accessible on http://localhost:6060/
### END INIT INFO

# Author: Karsten Flügge @ Pannous info@pannous.com


# /etc/systemd/system/netbase.service

# [Unit]
# Description=My service

# [Service]
# Type=forking
# ExecStart=/bin/sh $HOME/theFolder/run.sh
# Restart=on-failure

# [Install]
# WantedBy=multi-user.target
# Alias=mine.service

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
NAME=netbase
DIR=~/$NAME/
DAEMON=$DIR/$NAME
PIDFILE=/run/$NAME/$NAME.pid
DESC="Netbase Graph Database"
SCRIPTNAME=/etc/init.d/$NAME

. /lib/lsb/init-functions

# alias kill_screens='screen -ls | grep netbase | cut -d. -f1 | awk {print $1} | xargs echo'

# function kill_screens(){ sytax error
# 	screen -ls | grep netbase | cut -d. -f1 | awk '{print $1}' | xargs kill
# }


case "$1" in
  start)
		log_daemon_msg "Starting $DESC" "$NAME"
		screen -ls | grep netbase || screen -Sdm netbase
		cd $DIR && screen -S netbase -X stuff './server.sh^M'
	;;
  stop)
		log_daemon_msg "Stopping $DESC" "$NAME"
		killall $NAME
	;;
  test)
		# kill_screens
		cd $DIR
		screen -ls | grep netbase || ./clear-shared-memory.sh # danger
		./netbase :import wins
		screen -ls | grep netbase || screen -Sdm netbase
		screen -S netbase -X stuff './server.sh^M'
		echo "Test"
	;;
  reload|force-reload)
		log_daemon_msg "Reloading $DESC" "$NAME"
  ;;
  restart)
		log_daemon_msg "Restarting $DESC" "$NAME"
	;;
  status)
		status_of_proc -p "$PIDFILE" "$DAEMON" "$NAME" && exit 0 || exit $?
	;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop|restart|force-reload|status}" >&2
	exit 3
	;;
esac

exit 0