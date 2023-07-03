# CUPP++ 
Common User Passwords Profiler in C++, ideal for generating unreasonably large lists.



## About

The most common form of authentication is the combination of a username
and a password or passphrase. If both match values stored within a locally
stored table, the user is authenticated for a connection. Password strength is
a measure of the difficulty involved in guessing or breaking the password
through cryptographic techniques or library-based automated testing of
alternate values.

A weak password might be very short or only use alphanumberic characters,
making decryption simple. A weak password can also be one that is easily
guessed by someone profiling the user, such as a birthday, nickname, address,
name of a pet or relative, or a common word such as God, love, money or password.

That is why CUPP was born, and it can be used in situations like legal
penetration tests or forensic crime investigations.


Why a new C++ version
--------------
With modern GPUs it has become feasible to crack a password against 
a list of 10s of millions potential passwords in a short amount of time. This project
began as an attempt to modify the original python CUPP library uploaded by Mebus (created 
by j0rgan) to create more permutations and add additional fields. However, 
I found creating the dictionary (specifically removing duplicates) in python 
to be painfully slow.


Since C++ generally outperforms Python I started manually building this port. During 
the build process, I discovered that by using an unordered sets 
we can remove the need to check for duplicates, removing an O(n+m) operation
for the largest n possible(the final list of all combinations
) and the number of unique words(m).

For large lists I found a roughly **1000%+ increase in performance**, which works out 
spending 5 minutes instead of an hour. Of course your mileage my vary depending on the
number of collisions / duplicates generated.

Due to this discovery and the performant nature of C++ I was able to add a number of 
fields and permutations.


New Features only in CUPP++
------------
* New Fields Added:
  * Child 2
  * Child 2 Nickname 
  * Child 2 Birthday
  * Street Address
  * City 
  * State
  * School
* Prepend and Bookend Special Chars (Append Still Exists)
* Adjust amount of permutations based on information about the target
* Every possible combination of l337 (leet character replacement)
  * (i.e. lee7, le37,l337, 1337, 1ee7, l33t...)
* Combine the above modes 
  * (for example add special chars to l337 words and concatenated words)
* AlectoDB (2010s) and cultural wordlists(1990s) now found in same mode
* Added danielmieser/SecLists


Requirements
------------

Ensure you select the right binary for your OS and ensure the config.ini file is
in the same directory as the executable. 

Boost libraries are currently required. On linux OpenSSL is often already on your system
to install Boost try the following 

`sudo apt-get install libboost-all-dev`

This will compile and has been tested on Windows and Mac.

It was built with the latest boost lib for each respective package manager. Future versions will standardize this,
or move to static libs but if you need to make edits and recompile change the boost version in CMakeLists.txt
    * Homebrew (OSX) 1.82
    * pacman (Arch) 1.81
    * apt-get (Kali) 1.74

Quick start
-----------
Ensure you have Boost and OpenSSL on your system then

   `$ ./cupp++ -h`

## Options

Usage: cupp.py [OPTIONS]

        -h      this menu

        -i      Interactive questions for user password profiling

        -w      Use this option to improve an existing wordlist

        -d      Download huge wordlists from SecLists repository, alectoDB, or cultural wordlists

        -v      Version of the program



## Configuration

CUPP has configuration file config.ini with instructions.



## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

See './LICENSE' for more information.





## Contributors
#### CUPP++

C++ port and major version increase by Ryan
  http://github.com/the-ryan-1


#### Legacy CUPP

* Original author - Muris Kurgas aka j0rgan  
  j0rgan@remote-exploit.org  
  http://www.remote-exploit.org  
  http://www.azuzi.me


* Bosko Petrovic aka bolexxx  
  bole_loser@hotmail.com  
  http://www.offensive-security.com  
  http://www.bolexxx.net

* Mebus  
  https://github.com/Mebus/

* Abhro  
  https://github.com/Abhro/

* Andrea Giacomo  
  https://github.com/codepr

* quantumcore  
  https://github.com/quantumcore


### Additional Notes

There are still outstanding TODO items for anyone who wants to contribute. For example,
GZ files are not properly handled in download mode. Input validation could also be added. 

But it was in a good enough place that the broader community should find value in my current improvements.
    
