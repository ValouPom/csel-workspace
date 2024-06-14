start() {
    insmod /root/module.ko
    /root/app &
}

restart() {
    start
}
stop() {
	rmmod module
}
case "$1" in
    start)
        start
        ;;
    restart|reload)
        restart
        ;;
    stop)
	stop
	;;
    *)
    echo "Usage: $0 {start|restart}"
    exit 1
esac