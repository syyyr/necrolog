# NecroLog
One header tiniest logging for C++

compile test
```sh
g++ necro-test.cpp -o necro-test -std=c++11
```
show all the log output
```sh
./necro-test -v
```
show all the log output with level INFO or higher
```sh
./necro-test -v :I
```
show all the log output with level DEBUG or higher and topic or file name containing 'foo'
```sh
./necro-test -v foo
```
show all the log output with level DEBUG or higher and topic or file name containing 'foo' or 'bar'
```sh
./necro-test -v foo,bar
```
