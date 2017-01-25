# vim:set ft= ts=4 sw=4 et fdm=marker:

use lib 'lib';
use Test::Nginx::Socket;

#worker_connections(1014);
#master_on();
#workers(2);
#log_level('warn');

repeat_each(1);

no_shuffle();

plan tests => repeat_each() * (blocks() * 4);

run_tests();

__DATA__

=== TEST 1: ambiguous pattern
--- config
    default_type text/html;
    client_body_buffer_size 1;
    client_header_buffer_size 1;
    proxy_buffer_size 1;
    location /t {
        echo -n a;
        echo -n b;
	echo -n c;
        echo -n d;
        echo efghijklmnopqrstuvwxyzabacabac;
        dummy_filter on;
    }
--- request
GET /t
--- response_body
abcdefghijklmnopqrstuvwxyzabacabac
--- no_error_log
[alert]
[error]

