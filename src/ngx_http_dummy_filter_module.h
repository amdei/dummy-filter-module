#ifndef _NGX_HTTP_DUMMY_FILTER_MODULE_H_INCLUDED_
#define _NGX_HTTP_DUMMY_FILTER_MODULE_H_INCLUDED_


typedef struct {
    ngx_flag_t                 enabled;
    
    ngx_hash_t                 types;
    ngx_array_t               *types_keys;
} ngx_http_dummy_filter_loc_conf_t;


typedef struct {
    ngx_buf_t           *buf;
    u_char              *pos;        /** < parse position of the current buffer */
    u_char              *last_pos;   /** < parse position on previous parse round. */

    ngx_chain_t         *in;         /** < Input chain */
    ngx_chain_t         *out;        /** < Output chain, where all output 
                                      *    data being copied. */

} ngx_http_dummy_filter_ctx_t;

#endif /* _NGX_HTTP_DUMMY_FILTER_MODULE_H_INCLUDED_ */
