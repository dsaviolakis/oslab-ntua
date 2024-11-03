for i in {1..40}; do
    PID=$(pgrep riddle)
    if [ -n "$PID" ]; then
        kill -SIGURG $PID
    else
        echo "Process 'riddle' not found."
        break
    fi
    sleep 4
done

