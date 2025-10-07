#ifndef UCLI_H
#define UCLI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>
#else
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef PSTR
#define PSTR(x) (x)
#endif
static inline unsigned char pgm_read_byte(const char *p) { return (unsigned char)(*p); }
#endif

#ifndef UCLI_MAX_LINE
#define UCLI_MAX_LINE 64
#endif
#ifndef UCLI_MAX_ARGC
#define UCLI_MAX_ARGC 8
#endif
#ifndef UCLI_MAX_GROUPS
#define UCLI_MAX_GROUPS 8
#endif

    typedef void (*ucli_putc_fn)(char c);

    typedef void (*ucli_cb_u8)(uint8_t v);
    typedef void (*ucli_cb_void)(void);

    typedef struct
    {
        const char *name_P;
        volatile uint8_t *storage;
        ucli_cb_u8 on_set;
        ucli_cb_void on_show;
        ucli_cb_void on_help;
    } ucli_u8_item_t;

    /* skupina */
    typedef struct
    {
        const char *name_P;
        const ucli_u8_item_t *items;
        uint8_t count;
    } ucli_u8_group_t;

    /* instance CLI */
    typedef struct
    {
        char line[UCLI_MAX_LINE];
        uint8_t idx;
        uint8_t overflowed;
        const ucli_u8_group_t *groups[UCLI_MAX_GROUPS];
        uint8_t groups_count;
        ucli_putc_fn putc_cb;
    } ucli_t;

    /* ===== API ===== */
    void ucli_init(ucli_t *cli, ucli_putc_fn putc_cb);
    uint8_t ucli_u8_add_group(ucli_t *cli, const ucli_u8_group_t *g);
    void ucli_feed_char(ucli_t *cli, char ch);

    /* výpisy */
    void ucli_puts(ucli_t *cli, const char *s);
    void ucli_putsln(ucli_t *cli, const char *s);
    void ucli_puts_P(ucli_t *cli, const char *progmem_s);
    void ucli_putsln_P(ucli_t *cli, const char *progmem_s);

    /* util */
    int ucli_cmp(const char *a, const char *b);
    int ucli_cmp_P(const char *ram, const char *progmem_s);
    int ucli_icmp(const char *a, const char *b);
    uint8_t ucli_parse_u8(const char *s, uint8_t *out);
    void ucli_putu8(ucli_t *cli, uint8_t v);
    void ucli_putname_eq_u8_P(ucli_t *cli, const char *name_P, uint8_t v);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UCLI_H */
