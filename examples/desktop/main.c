
#include <stdio.h>

#define UCLI_MAX_LINE   34
#define UCLI_MAX_ARGC   4
#define UCLI_MAX_GROUPS 2
#define UCLI_IMPLEMENTATION
#define UCLI_CLI_GEN_IMPLEMENTATION
#include "ucli_gen.h"


void on_set_wirelesskjjkjjk_scan(uint8_t v){ (void)v; /* TODO */ }

ucli_t cli;
static void host_putc(char c){ putchar((unsigned char)c); }
int main(void){
  ucli_init(&cli, host_putc);
  ucli_cli_register(&cli);
  int ch; while((ch=getchar())!=EOF){ ucli_feed_char(&cli, (char)ch); }
  return 0;
}