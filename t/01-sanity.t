# vim:set ft= ts=4 sw=4 et fdm=marker:

use lib 'lib';
use Test::Nginx::Socket;

#worker_connections(1014);
#master_on();
#workers(2);
#log_level('warn');

repeat_each(2);

#no_shuffle();

plan tests => repeat_each() * (blocks() * 4 + 1);

our $StapOutputChains = <<'_EOC_';
global active

F(ngx_http_handler) {
    active = 1
}

/*
F(ngx_http_write_filter) {
    if (active && pid() == target()) {
        printf("http writer filter: %s\n", ngx_chain_dump($in))
    }
}
*/

F(ngx_http_chunked_body_filter) {
    if (active && pid() == target()) {
        printf("http chunked filter: %s\n", ngx_chain_dump($in))
    }
}

F(ngx_http_replace_output) {
    if (active && pid() == target()) {
        printf("http replace output: %s\n", ngx_chain_dump($ctx->out))
    }
}

probe syscall.writev {
    if (active && pid() == target()) {
        printf("writev(%s)", ngx_iovec_dump($vec, $vlen))
        /*
        for (i = 0; i < $vlen; i++) {
            printf(" %p [%s]", $vec[i]->iov_base, text_str(user_string_n($vec[i]->iov_base, $vec[i]->iov_len)))
        }
        */
    }
}

probe syscall.writev.return {
    if (active && pid() == target()) {
        printf(" = %s\n", retstr)
    }
}

_EOC_

#no_diff();
#no_long_string();
run_tests();

__DATA__

=== TEST 1: ambiguous pattern
--- config
    default_type text/html;
    location /t {
        echo abcabcabde;
        dummy_filter on;
    }
--- request
GET /t

--- stap
F(ngx_http_replace_non_capturing_parse) {
    println("non capturing parse")
}

F(ngx_http_replace_capturing_parse) {
    println("capturing parse")
}

F(ngx_http_replace_complex_value) {
    println("complex value")
}

--- stap_out_like chop
^(non capturing parse\n)+$

--- response_body
abcabcabde
--- no_error_log
[alert]
[error]



=== TEST 2: ambiguous pattern
--- config
    default_type text/html;
    location /t {
        echo -n ababac;
        dummy_filter        on;
    }
--- request
GET /t
--- response_body chop
ababac
--- no_error_log
[alert]
[error]



=== TEST 6: 1-byte chain bufs
--- config
    default_type text/html;

    location = /t {
        echo -n a;
        echo -n b;
        echo -n a;
        echo -n b;
        echo -n a;
        echo -n c;
        echo d;
        dummy_filter on;
    }
--- request
GET /t
--- stap2 eval: $::StapOutputChains
--- stap3
probe process("nginx").statement("*@ngx_http_replace_filter_module.c:1413") {
    //printf("chain: %s", ngx_chain_dump($ctx->busy))
    print_ubacktrace()
}

--- response_body
ababacd
--- no_error_log
[alert]
[error]




=== TEST 7: 2-byte chain bufs
--- config
    default_type text/html;

    location = /t {
        echo -n ab;
        echo -n ab;
        echo -n ac;
        echo d;
        dummy_filter on;
    }
--- request
GET /t
--- stap2 eval: $::StapOutputChains
--- response_body
ababacd
--- no_error_log
[alert]
[error]



=== TEST 8: 3-byte chain bufs
--- config
    default_type text/html;

    location = /t {
        echo -n aba;
        echo -n bac;
        echo d;
        dummy_filter on;
    }
--- request
GET /t
--- stap2 eval: $::StapOutputChains
--- response_body
ababacd
--- no_error_log
[alert]
[error]



=== TEST 10: once by default (1st char matched)
--- config
    default_type text/html;
    location /t {
        echo abcabcabde;
        dummy_filter on;
    }
--- request
GET /t
--- response_body
abcabcabde
--- no_error_log
[alert]
[error]



=== TEST 18: custom replace_filter_types
--- config
    default_type text/plain;
    location /t {
        echo abc;
        dummy_filter on;
        dummy_filter_types text/plain;
    }
--- request
GET /t
--- response_body
abc
--- no_error_log
[alert]
[error]



=== TEST 19: multiple replace_filter_types settings
--- config
    default_type text/plain;
    location /t {
        echo abc;
        dummy_filter on;
    }
--- request
GET /t
--- response_body
abc
--- no_error_log
[alert]
[error]



=== TEST 23: pure flush buf in the stream (no data)
--- config
    default_type text/html;
    location = /t {
        echo_flush;
        dummy_filter on;
    }
--- request
GET /t
--- response_body chop
--- no_error_log
[alert]
[error]



=== TEST 24: pure flush buf in the stream (with data)
--- config
    default_type text/html;
    location = /t {
        echo a;
        echo_flush;
        dummy_filter on;
    }
--- request
GET /t
--- stap3 eval: $::StapOutputChains
--- stap2
probe process("nginx").statement("*@ngx_http_replace_filter_module.c:539") {
    printf("chain: %s", ngx_chain_dump($ctx->busy))
    //print_ubacktrace()
}
--- response_body
a
--- no_error_log
[alert]
[error]


