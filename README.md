ETVDB_CLI
=========

1) What is it?
--------------
ETVDB_CLI is a command line tool to get data from [The TV Database (TVDB)][1].
It is written in C and makes use of the [Enlightenment Foundation Libraries][2]
and [the etvdb library][3].

[1]: http://thetvdb.com/
[2]: http://www.enlightenment.org/
[3]: https://github.com/tg--/etvdb

2) How to build
---------------
This is super easy:
```
mkdir BUILD/ && cd BUILD/
cmake .. && make && make install
```

The dependencies are Eina, Ecore and etvdb.

3) License
----------
libetvdb is available under the GPLv3 or any later version.

See COPYING for details.
