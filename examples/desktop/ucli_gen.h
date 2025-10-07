#ifndef UCLI_CLI_GEN_H
#define UCLI_CLI_GEN_H

#ifndef UCLI_MAX_LINE
#define UCLI_MAX_LINE 34
#endif
#ifndef UCLI_MAX_ARGC
#define UCLI_MAX_ARGC 4
#endif
#ifndef UCLI_MAX_GROUPS
#define UCLI_MAX_GROUPS 2
#endif
#include "ucli.h"

#ifndef UCLI_COUNT
#define UCLI_COUNT(a) ((uint8_t)(sizeof(a)/sizeof((a)[0])))
#endif

#ifdef __AVR__
#  define STR_DEF(sym,lit) static const char sym[] PROGMEM = lit
#else
#  define STR_DEF(sym,lit) static const char sym[] = lit
#endif

STR_DEF(S_group2, "group2");
STR_DEF(S_wirelesskjjkjjk, "wirelesskjjkjjk");
STR_DEF(S_item2, "item2");
STR_DEF(S_item1, "item1");
STR_DEF(S_channel, "channel");
STR_DEF(S_power, "power");
STR_DEF(S_scan, "scan");

STR_DEF(S_HELP_1, "1..11");
STR_DEF(S_HELP_2, "0..20");
STR_DEF(S_HELP_3, "0,1");

static ucli_t *ucli__ctx = 0;

#ifndef UCLI_CLI_GEN_IMPLEMENTATION
extern volatile uint8_t g_group2_item2;
#else
volatile uint8_t g_group2_item2 = 0;
#endif
#ifndef UCLI_CLI_GEN_IMPLEMENTATION
extern volatile uint8_t g_group2_item1;
#else
volatile uint8_t g_group2_item1 = 0;
#endif
#ifndef UCLI_CLI_GEN_IMPLEMENTATION
extern volatile uint8_t g_wirelesskjjkjjk_channel;
#else
volatile uint8_t g_wirelesskjjkjjk_channel = 1;
#endif
#ifndef UCLI_CLI_GEN_IMPLEMENTATION
extern volatile uint8_t g_wirelesskjjkjjk_power;
#else
volatile uint8_t g_wirelesskjjkjjk_power = 0;
#endif
#ifndef UCLI_CLI_GEN_IMPLEMENTATION
extern volatile uint8_t g_wirelesskjjkjjk_scan;
#else
volatile uint8_t g_wirelesskjjkjjk_scan = 0;
#endif

static void help_wirelesskjjkjjk_channel(void){ ucli_putsln_P(ucli__ctx, S_HELP_1); }
static void help_wirelesskjjkjjk_power(void){ ucli_putsln_P(ucli__ctx, S_HELP_2); }
static void help_wirelesskjjkjjk_scan(void){ ucli_putsln_P(ucli__ctx, S_HELP_3); }

void on_set_wirelesskjjkjjk_scan(uint8_t v);

static const ucli_u8_item_t ARR_GROUP2[] = {
  { S_item2, &g_group2_item2, 0, 0, 0 },
  { S_item1, &g_group2_item1, 0, 0, 0 }
};
static const ucli_u8_group_t G_group2 = { S_group2, ARR_GROUP2, UCLI_COUNT(ARR_GROUP2) };

static const ucli_u8_item_t ARR_WIRELESSKJJKJJK[] = {
  { S_channel, &g_wirelesskjjkjjk_channel, 0, 0, help_wirelesskjjkjjk_channel },
  { S_power, &g_wirelesskjjkjjk_power, 0, 0, help_wirelesskjjkjjk_power },
  { S_scan, &g_wirelesskjjkjjk_scan, on_set_wirelesskjjkjjk_scan, 0, help_wirelesskjjkjjk_scan }
};
static const ucli_u8_group_t G_wirelesskjjkjjk = { S_wirelesskjjkjjk, ARR_WIRELESSKJJKJJK, UCLI_COUNT(ARR_WIRELESSKJJKJJK) };

static inline void ucli_cli_register(ucli_t* cli){
  ucli__ctx = cli;
  ucli_u8_add_group(cli, &G_group2);
  ucli_u8_add_group(cli, &G_wirelesskjjkjjk);
}

#endif /* UCLI_CLI_GEN_H */