#
# script to configure in-development nginx

cd nginx-1.10.1

CFLAGS="-g -ggdb3 -O0 -W -Wall -Wpointer-arith -Wno-unused-parameter -Werror" ./configure --with-debug --with-pcre \
 --with-pcre-jit --with-ipv6 \
 --with-poll_module --add-module=.. \
 --add-module=../echo-nginx-module \

cd ..
