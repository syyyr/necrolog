# NecroLog
One header tiniest logging for C++

compile test
```sh
g++ necro-test.cpp -o necro-test -std=c++11
```
run test
```sh
[~/proj/necrolog]$ ./necro-test --help                                                                                                                                                                           *[master] 
-lfn, --log-long-file-names
        Log long file names
-d, -v, --verbose [<pattern>]:[D|I|W|E]
        Set files or topics log treshold
        set treshold for all files or topics containing pattern to treshold D|I|W|E
        when pattern is not set, set treshold for any filename or topic
        when treshold is not set, set treshold D (Debug) for all files or topics containing pattern
        when nothing is not set, set treshold D (Debug) for all files or topics
        Examples:
                -d              set treshold D (Debug) for all files or topics
                -d :W           set treshold W (Warning) for all files or topics
                -d foo,bar              set treshold D for all files or topics containing 'foo' or 'bar'
                -d bar:W        set treshold W (Warning) for all files or topics containing 'bar'
```
show all the log output
```sh
./necro-test -v 
```
show all the log output with level DEBUG or higher and topic or file name containing 'foo'
```sh
./necro-test -v foo
```

