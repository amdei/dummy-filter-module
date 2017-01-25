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

=== TEST 1: Simple URL in one buf
--- config
    default_type text/html;
    chunked_transfer_encoding off;
    location /t {
        echo -n "<a href=\"this-is-URL\">";
        dummy_filter on;
    }
--- request
GET /t
--- response_body chop
<a href="this-is-URL">
--- no_error_log
[alert]
[error]