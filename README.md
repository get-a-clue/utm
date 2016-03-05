# utm
UTM - Underlying TMeter Framework.

This is the framework used in TMeter (http://www.tmeter.ru).

--------------------------------------------------------------------------------------------------------------
WHAT IS TMETER ?

TMeter is a powerful traffic accounting and Internet sharing tool for Microsoft Windows. 
Using TMeter, you can make flexible and precision measurement of the traffic by any condition
(for example, IP address of source/destination, protocol, port and etc) in real-time with immediately
output of the collected statistic to graph or other reports.
 
TMeter has own built-in NAT (network address translation) engine which enables multiple hosts on 
a private network to access the Internet using a single public IP address. The firewall (also built-in TMeter) 
defends your host or your network against unwanted intrusions from the Internet. The Traffic Shaper allows 
restricting the speed of the Internet access (for example, 256 kbit/s) for specific users.

--------------------------------------------------------------------------------------------------------------
HOW TO BUILD UTM

1. Install Microsoft Visual Studio Profession 2013+ with C++ compilier
2. Download and build boost library (http://www.boost.org):
   Run: bootstrap.bat
   Run: b2 -a toolset=msvc-12.0 --build-type=complete --with-thread --with-atomic --with-exception --with-filesystem --with-regex --with-system --with-date_time --with-signals --with-test --abbreviate-paths architecture=x86 install -j4

3. Set environmnet variables:
   BOOST_INCLUDE=c:\install\boost
   BOOST_LIB=c:\install\boost\stage\lib
4. Run build_all.bat or open solution in Visual Studio and build every project manually.

