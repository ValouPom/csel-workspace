start() {
    ls /opt/test.sh && sh /opt/test.sh
}

restart() {
    start
}
case "$1" in
    start)
        start
        ;;
    restart|reload)
        restart
        ;;
    *)
    echo "Usage: $0 {start|restart}"
    exit 1
esac