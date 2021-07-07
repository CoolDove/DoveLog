# DoveLog
A very simple printf-style log system.
## How to use
 - Use **DLOG_INIT** to initialize. And use **DLOG_CLOSE** to close
 - Set **DLOG_ON_PUSH**/**DLOG_ON_INIT**/**DLOG_ON_CLOSE** callback as wish if u want to log to console or a log file.
 - Call **DLOG_ERROR**/**DLOG_WARN**/**DLOG_DEBUG**... to log.
