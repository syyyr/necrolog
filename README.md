# NecroLog
Tiny logging library for C++.

## Building
*necrolog* supports CMake and the standard CMake build procedure:
```sh
mkdir build
cd build
cmake ..
make
```

## Testing
The CMake build compiles the test by default. You can also compile it manually. From withing the `tests` directory:
```sh
cd tests
g++ necro-test.cpp ../libnecrolog/necrolog.cpp -I../include -o necro-test -std=c++11
```

You can then fiddle with the test binary:
```sh
[~/proj/necrolog/tests]$ ./necro-test --help
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
Show all the log output:
```sh 
./necro-test -v
2017-12-22T11:06:52[necro-test.cpp:27]|D| Debug mesage
2017-12-22T11:06:52[necro-test.cpp:28]|I| Info mesage
2017-12-22T11:06:52[necro-test.cpp:29]|W| Warning mesage
2017-12-22T11:06:52[necro-test.cpp:30]|E| Error mesage
2017-12-22T11:06:52[necro-test.cpp:32]|I| args not used by necro log: ./necro-test
2017-12-22T11:06:52[necro-test.cpp:34](foo)|I| Info mesage foo topic
2017-12-22T11:06:52[necro-test.cpp:35](bar)|D| Debug mesage bar topic
```
Show all the log output with level DEBUG or higher and topic or file name containing 'bar' and with level WARNING or higher and topic or file name containing 'test':
```sh
./necro-test -v bar,test:W                                                                                                                                                                   
2017-12-22T11:07:59[necro-test.cpp:29]|W| Warning mesage
2017-12-22T11:07:59[necro-test.cpp:30]|E| Error mesage
2017-12-22T11:07:59[necro-test.cpp:35](bar)|D| Debug mesage bar topic
```

