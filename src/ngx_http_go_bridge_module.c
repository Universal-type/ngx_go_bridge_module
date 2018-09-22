#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_str_t dl_name;
    ngx_str_t dl_symbol;
    unsigned loaded :1;
} ngx_http_go_bridge_t;

typedef struct {
    ngx_http_go_bridge_t *go_bridge;
} ngx_http_go_bridge_loc_conf_t;

static ngx_int_t ngx_http_go_bridge_handler(ngx_http_request_t *r);
static char *ngx_http_go_bridge(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_http_go_bridge_create_loc_conf(ngx_conf_t *cf);
static ngx_int_t ngx_http_go_bridge_init(ngx_conf_t *cf);

static ngx_command_t ngx_http_go_bridge_commands[] = {

{ ngx_string("go_bridge"), NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,
        ngx_http_go_bridge, NGX_HTTP_LOC_CONF_OFFSET, 0, NULL },

ngx_null_command };

static ngx_http_module_t ngx_http_go_bridge_module_ctx = { NULL, /* preconfiguration */
ngx_http_go_bridge_init, /* postconfiguration */
NULL, /* create main configuration */
NULL, /* init main configuration */
NULL, /* create server configuration */
NULL, /* merge server configuration */
ngx_http_go_bridge_create_loc_conf, /* create location configuration */
NULL /* merge location configuration */
};

ngx_module_t ngx_http_go_bridge_module = { NGX_MODULE_V1,
        &ngx_http_go_bridge_module_ctx, /* module context */
        ngx_http_go_bridge_commands, /* module directives */
        NGX_HTTP_MODULE, /* module type */
        NULL, /* init master */
        NULL, /* init module */
        NULL, /* init process */
        NULL, /* init thread */
        NULL, /* exit thread */
        NULL, /* exit process */
        NULL, /* exit master */
        NGX_MODULE_V1_PADDING };

static ngx_int_t ngx_http_go_bridge_handler(ngx_http_request_t *r) {
    ngx_http_go_bridge_loc_conf_t *tlcf;

    tlcf = ngx_http_get_module_loc_conf(r, ngx_http_go_bridge_module);

    if (tlcf->go_bridge == NULL) {
        return NGX_DECLINED;
    }

    // TODO
    // Call go function and set response
    u_char response_body[] = {'a', 'b', 'c', '\n', '\0'};
    ngx_str_t content_type = ngx_string("text/plain");

    ngx_http_complex_value_t cv;
    ngx_memzero(&cv, sizeof(ngx_http_complex_value_t));
    cv.value.len = sizeof(&response_body);
    cv.value.data = response_body;

    return ngx_http_send_response(r, NGX_HTTP_OK, &content_type, &cv);
}

static char *
ngx_http_go_bridge(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_go_bridge_loc_conf_t *tlcf = conf;

    if (tlcf->go_bridge) {
        return "is duplicate";
    }

    ngx_str_t *value = cf->args->elts;
    ngx_http_go_bridge_t *tf = ngx_pcalloc(cf->pool,
            sizeof(ngx_http_go_bridge_t));

    tf->dl_name = value[1];
    tf->dl_symbol = value[2];

    // TODO
    // Load go binary here

    tlcf->go_bridge = tf;

    return NGX_CONF_OK;
}

static void *
ngx_http_go_bridge_create_loc_conf(ngx_conf_t *cf) {
    ngx_http_go_bridge_loc_conf_t *tlcf;
    tlcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_go_bridge_loc_conf_t));
    if (tlcf == NULL) {
        return NULL;
    }

    return tlcf;
}

static ngx_int_t ngx_http_go_bridge_init(ngx_conf_t *cf) {
    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_PRECONTENT_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }
    *h = ngx_http_go_bridge_handler;

    return NGX_OK;
}
