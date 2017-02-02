
/*
 * Copyright (C) Andrey Kulikov
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */

#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#include "ngx_http_dummy_filter_module.h"

static ngx_int_t ngx_http_dummy_filter_parse(ngx_http_request_t *r,
    ngx_http_dummy_filter_ctx_t *ctx);
static ngx_int_t ngx_http_dummy_filter_output(ngx_http_request_t *r, 
    ngx_http_dummy_filter_ctx_t *ctx);

static void *ngx_http_dummy_filter_create_conf(ngx_conf_t *cf);
static char *ngx_http_dummy_filter_merge_conf(ngx_conf_t *cf,
    void *parent, void *child);
static ngx_int_t ngx_http_dummy_filter_init(ngx_conf_t *cf);


static ngx_command_t  ngx_http_dummy_filter_commands[] = {

    { ngx_string("dummy_filter"),
      NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_dummy_filter_loc_conf_t, enabled),
      NULL },

    { ngx_string("dummy_filter_types"),
      NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_types_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_dummy_filter_loc_conf_t, types_keys),
      &ngx_http_html_default_types[0] },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_dummy_filter_module_ctx = {
    NULL,                              /* preconfiguration */
    ngx_http_dummy_filter_init,        /* postconfiguration */

    NULL,                              /* create main configuration */
    NULL,                              /* init main configuration */

    NULL,                              /* create server configuration */
    NULL,                              /* merge server configuration */

    ngx_http_dummy_filter_create_conf, /* create location configuration */
    ngx_http_dummy_filter_merge_conf   /* merge location configuration */
};


ngx_module_t  ngx_http_dummy_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_dummy_filter_module_ctx, /* module context */
    ngx_http_dummy_filter_commands,    /* module directives */
    NGX_HTTP_MODULE,                   /* module type */
    NULL,                              /* init master */
    NULL,                              /* init module */
    NULL,                              /* init process */
    NULL,                              /* init thread */
    NULL,                              /* exit thread */
    NULL,                              /* exit process */
    NULL,                              /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;


static ngx_int_t
ngx_http_dummy_filter_header_filter(ngx_http_request_t *r)
{
    ngx_http_dummy_filter_ctx_t       *ctx;
    ngx_http_dummy_filter_loc_conf_t  *slcf;

    slcf = ngx_http_get_module_loc_conf(r, ngx_http_dummy_filter_module);

    if (r->headers_out.content_length_n == 0
        || ngx_http_test_content_type(r, &slcf->types) == NULL)
    {
        return ngx_http_next_header_filter(r);
    }
    
    if(!slcf->enabled) {
        return ngx_http_next_header_filter(r);
    }

    ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_dummy_filter_ctx_t));
    if (ctx == NULL) {
        return NGX_ERROR;
    }
    
    ngx_http_set_ctx(r, ctx, ngx_http_dummy_filter_module);

    r->filter_need_in_memory = 1;

    if (r == r->main) {
        ngx_http_clear_content_length(r);
    }

    return ngx_http_next_header_filter(r);
}


static ngx_int_t
ngx_http_dummy_filter_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_int_t                          rc;
    ngx_int_t                          pre_url_len;
    ngx_chain_t                       *cl, **cur_cl, *nl;
    ngx_buf_t                         *b;
    ngx_http_dummy_filter_ctx_t       *ctx;
    

    ctx = ngx_http_get_module_ctx(r, ngx_http_dummy_filter_module);

    if (ctx == NULL) {
        return ngx_http_next_body_filter(r, in);
    }
    
    if ((in == NULL
         && ctx->buf == NULL
         && ctx->in == NULL))
    {
        return ngx_http_next_body_filter(r, in);
    }
    
    if (in) {
        if (ngx_chain_add_copy(r->pool, &ctx->in, in) != NGX_OK) {
            return NGX_ERROR;
        }
    }

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http dummy filter \"%V\"", &r->uri);    

    cur_cl = &ctx->out;
    cl = NULL;
            
    while (ctx->in || ctx->buf) {

        if (ctx->buf == NULL) {
            ctx->buf            = ctx->in->buf;
            ctx->in             = ctx->in->next;
            ctx->pos            = ctx->buf->pos;
            ctx->last_pos       = ctx->buf->pos;
        }
        
        b = NULL;

        while (*cur_cl) {
            cl = *cur_cl;
            cur_cl = &cl->next;
        }
        
        rc = ngx_http_dummy_filter_parse(r, ctx);
        if (rc == NGX_ERROR) {
            ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
               "FAILED ngx_http_dummy_filter_parse:%i", rc);    
            return rc;
        }        
        
        if (rc == NGX_ABORT) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
               "ngx_http_dummy_filter_parse: error in parse: %i", rc);    
        }

        pre_url_len = ctx->pos - ctx->last_pos;
        
        if (pre_url_len > 0 && ctx->last_pos) {
            dd("No URL found in buffer");            

            cl = ngx_chain_get_free_buf(r->pool, cur_cl);
            if (cl == NULL) {
                dd("ngx_chain_get_free_buf FAILED");
                return NGX_ERROR;
            }

            b = cl->buf;

            ngx_memzero(b, sizeof(ngx_buf_t));

            b->pos = ngx_pnalloc(r->pool, pre_url_len);
            if (b->pos == NULL) {
                dd("ngx_pnalloc FAILED");
                return NGX_ERROR;
            }

            ngx_memcpy(b->pos, ctx->last_pos, pre_url_len);
            b->last = b->pos + pre_url_len;
            b->memory = 1;

            *cur_cl = cl; 
        }
        

        if (rc == NGX_AGAIN) {
            continue;
        }
        
        if (rc == NGX_ABORT) {
            continue;
        }
        
        if (ctx->buf->last_buf || ctx->buf->flush || ctx->buf->sync
            || ngx_buf_in_memory(ctx->buf))
        {
            if (b == NULL) {
                nl = ngx_alloc_chain_link(r->pool);
                if (nl == NULL) {
                    return NGX_ERROR;
                }

                nl->buf = ngx_calloc_buf(r->pool);
                if (nl->buf == NULL) {
                    return NGX_ERROR;
                }
                
                nl->next = NULL;
                b = nl->buf;
                
                b->sync = 1;
                
                if(*cur_cl){
                    (*cur_cl)->next = nl;
                } else {
                    *cur_cl = nl;
                }                
            }
            
            b->last_buf = ctx->buf->last_buf;
            b->last_in_chain = ctx->buf->last_in_chain;
            b->flush = ctx->buf->flush;
            b->recycled = ctx->buf->recycled;
        }
        
        ctx->buf = NULL;
    }

    if (ctx->out == NULL) {
        return NGX_OK;
    }
   
    return ngx_http_dummy_filter_output(r, ctx);
}


static ngx_int_t
ngx_http_dummy_filter_parse(ngx_http_request_t *r, 
                                  ngx_http_dummy_filter_ctx_t *ctx)
{
/*    u_char                   *p; */
    u_char                   *start, *end;
    ngx_buf_t                *b;
    ngx_int_t                 rc;

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "dummy parse");
    
    b     = ctx->buf;
    start = ctx->pos;
    end   = b->last;

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "Processing %d chars.", end - start);
/*
    p = start;

    for (p = start; p < end; p++) {
        ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "catch body in: 0x%02Xd: %c", *p, *p);
    }
*/   
    if(start == end){
        ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "empty buffer");
        return NGX_OK;
    }
    
    rc = NGX_OK; /* = ngx_http_dummy_filter_parse_internal(r, state); */

    if(rc == NGX_ABORT){
        ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "ngx_http_dummy_filter_parse_internal FAILED");
    } else {
	/* Some work... */
    }
    
    /* update position */
    ctx->pos = end;
    
    
    return rc;
}


static ngx_int_t
ngx_http_dummy_filter_output(ngx_http_request_t *r, ngx_http_dummy_filter_ctx_t *ctx)
{
    ngx_int_t     rc;
    ngx_buf_t    *b;
    ngx_chain_t  *cl;

#if 1
    b = NULL;
    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "dummy out");
    for (cl = ctx->out; cl; cl = cl->next) {
        ngx_log_debug4(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "dummy out: %p %p-%p (%i)", cl->buf, cl->buf->pos,
                       cl->buf->last, cl->buf->last - cl->buf->pos);
        if (cl->buf == b) {
            ngx_log_error(NGX_LOG_ALERT, r->connection->log, 0,
                          "the same buf was used in dummy");
            ngx_debug_point();
            return NGX_ERROR;
        }
        b = cl->buf;
    }
#endif

    rc = ngx_http_next_body_filter(r, ctx->out);

    while (ctx->out) {
        cl = ctx->out;
        ctx->out = cl->next;
    }

    if (ctx->in || ctx->buf) {
        r->buffered |= NGX_HTTP_SUB_BUFFERED;
    } else {
        r->buffered &= ~NGX_HTTP_SUB_BUFFERED;
    }

    return rc;
}


static void *
ngx_http_dummy_filter_create_conf(ngx_conf_t *cf)
{
    ngx_http_dummy_filter_loc_conf_t  *slcf;

    slcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_dummy_filter_loc_conf_t));
    if (slcf == NULL) {
        return NULL;
    }

    /*
     * set by ngx_pcalloc():
     *
     *     conf->types = { NULL };
     *     conf->types_keys = NULL;
     *     conf->resource_name = { NULL, 0 };
     *     conf->host_alt_name = { NULL, 0 };
     *     conf->host_name = { NULL, 0 };
     */


    slcf->enabled = NGX_CONF_UNSET;

    return slcf;
}


static char *
ngx_http_dummy_filter_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_dummy_filter_loc_conf_t  *prev = parent;
    ngx_http_dummy_filter_loc_conf_t  *conf = child;


    ngx_conf_merge_value(conf->enabled, prev->enabled, 0);
    
    if (ngx_http_merge_types(cf, &conf->types_keys, &conf->types,
                             &prev->types_keys, &prev->types,
                             ngx_http_html_default_types)
        != NGX_OK)
    {
        return NGX_CONF_ERROR;
    }
    
    return NGX_CONF_OK;
}


static ngx_int_t
ngx_http_dummy_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_dummy_filter_header_filter;

    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_dummy_filter_body_filter;

    return NGX_OK;
}
