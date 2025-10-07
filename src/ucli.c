#include "ucli.h"

/* ===== syntax =====
    help      
    help commands
    help <group>
    help <group> <item>

    <group> show
    <group> show <item>
    <group> set <item> <u8>
*/

/* ===== error codes =====
    1 = syntax
    2 = unknown-group
    3 = unknown-item
    4 = parse fail
    5 = unknown (fallback)
    6 = overflow
*/

static inline void pch(ucli_t *cli, char c)
{
    if (cli && cli->putc_cb)
        cli->putc_cb(c);
}

void ucli_puts(ucli_t *cli, const char *s)
{
    if (!cli || !cli->putc_cb)
        return;
    while (*s)
        cli->putc_cb(*s++);
}
void ucli_putsln(ucli_t *cli, const char *s)
{
    ucli_puts(cli, s);
    pch(cli, '\n');
}

void ucli_puts_P(ucli_t *cli, const char *ps)
{
    if (!cli || !cli->putc_cb)
        return;
    char c;
    while ((c = (char)pgm_read_byte(ps++)) != 0)
        cli->putc_cb(c);
}
void ucli_putsln_P(ucli_t *cli, const char *ps)
{
    ucli_puts_P(cli, ps);
    pch(cli, '\n');
}

static void ucli_put_u8_core(ucli_t *cli, uint8_t v)
{
    char b[3];
    uint8_t i = 0;
    if (v == 0)
    {
        pch(cli, '0');
        return;
    }
    while (v)
    {
        b[i++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (i--)
        pch(cli, b[i]);
}
void ucli_putu8(ucli_t *cli, uint8_t v)
{
    if (!cli || !cli->putc_cb)
        return;
    ucli_put_u8_core(cli, v);
    pch(cli, '\n');
}
void ucli_putname_eq_u8_P(ucli_t *cli, const char *name_P, uint8_t v)
{
    if (!cli || !cli->putc_cb)
        return;
    char c;
    const char *p = name_P;
    while ((c = (char)pgm_read_byte(p++)) != 0)
        cli->putc_cb(c);
    cli->putc_cb('=');
    ucli_put_u8_core(cli, v);
    cli->putc_cb('\n');
}

int ucli_cmp(const char *a, const char *b)
{
    for (;;)
    {
        char ca = *a++, cb = *b++;
        if (ca != cb)
            return (int)(unsigned char)ca - (int)(unsigned char)cb;
        if (cb == 0)
            return 0;
    }
}
int ucli_cmp_P(const char *ram, const char *ps)
{
    for (;;)
    {
        char ra = *ram++;
        char pb = (char)pgm_read_byte(ps++);
        if (ra != pb)
            return (int)(unsigned char)ra - (int)(unsigned char)pb;
        if (pb == 0)
            return 0;
    }
}
static inline int lower(int c) { return (c >= 'A' && c <= 'Z') ? (c + 32) : c; }
int ucli_icmp(const char *a, const char *b)
{
    for (;;)
    {
        int ca = lower(*a++), cb = lower(*b++);
        if (ca != cb)
            return ca - cb;
        if (cb == 0)
            return 0;
    }
}
static inline uint8_t issp(char c)
{
    unsigned char u = (unsigned char)c;
    return (u <= 32u) || (u == 127u) || (u >= 128u);
}

static int is_help_token(const char *t) { return t[0] == 'h' && t[1] == 'e' && t[2] == 'l' && t[3] == 'p' && t[4] == 0; }
static int is_show_token(const char *t) { return t[0] == 's' && t[1] == 'h' && t[2] == 'o' && t[3] == 'w' && t[4] == 0; }
static int is_set_token(const char *t) { return t[0] == 's' && t[1] == 'e' && t[2] == 't' && t[3] == 0; }

static const char S_error[] PROGMEM = "error";
static inline void err_code(ucli_t *cli, uint8_t code)
{
    ucli_puts_P(cli, S_error);
    pch(cli, ' ');
    ucli_put_u8_core(cli, code);
    pch(cli, '\n');
}

uint8_t ucli_parse_u8(const char *s, uint8_t *out)
{
    uint16_t v = 0;
    uint8_t any = 0;
    while (*s >= '0' && *s <= '9')
    {
        v = (uint16_t)(v * 10u + (uint16_t)(*s - '0'));
        if (v > 255u)
            return 0;
        s++;
        any = 1;
    }
    if (!any)
        return 0;
    *out = (uint8_t)v;
    return 1;
}

void ucli_init(ucli_t *cli, ucli_putc_fn putc_cb)
{
    cli->idx = 0;
    cli->groups_count = 0;
    cli->overflowed = 0;
    cli->putc_cb = putc_cb;
}

uint8_t ucli_u8_add_group(ucli_t *cli, const ucli_u8_group_t *g)
{
    if (cli->groups_count >= UCLI_MAX_GROUPS)
        return 0;
    cli->groups[cli->groups_count++] = g;
    return 1;
}

static int tokenize(char *line, int len, const char *argv[UCLI_MAX_ARGC])
{
    int argc = 0, i = 0;
    if (len >= UCLI_MAX_LINE)
        len = UCLI_MAX_LINE - 1;
    line[len] = 0;
    while (i < len)
    {
        while (i < len && issp(line[i]))
            i++;
        if (i >= len || argc >= UCLI_MAX_ARGC)
            break;
        argv[argc++] = &line[i];
        while (i < len && !issp(line[i]))
            i++;
        if (i < len)
        {
            line[i] = 0;
            i++;
        }
    }
    return argc;
}

static void print_root_help(ucli_t *cli)
{
    for (uint8_t i = 0; i < cli->groups_count; i++)
    {
        ucli_puts_P(cli, cli->groups[i]->name_P);
        ucli_putsln(cli, "");
    }
    ucli_putsln_P(cli, PSTR("commands"));
}

static void print_commands(ucli_t *cli)
{
    ucli_putsln_P(cli, PSTR("show"));
    ucli_putsln_P(cli, PSTR("set"));
    ucli_putsln_P(cli, PSTR("help"));
}

static const ucli_u8_group_t *find_group(ucli_t *cli, const char *tok)
{
    for (uint8_t i = 0; i < cli->groups_count; i++)
        if (ucli_cmp_P(tok, cli->groups[i]->name_P) == 0)
            return cli->groups[i];
    return 0;
}
static const ucli_u8_item_t *find_item(const ucli_u8_group_t *G, const char *tok)
{
    for (uint8_t k = 0; k < G->count; k++)
    {
        const ucli_u8_item_t *it = &G->items[k];
        if (ucli_cmp_P(tok, it->name_P) == 0)
            return it;
    }
    return 0;
}

static void print_group_items(ucli_t *cli, const ucli_u8_group_t *G)
{
    for (uint8_t k = 0; k < G->count; k++)
    {
        ucli_puts_P(cli, G->items[k].name_P);
        ucli_putsln(cli, "");
    }
}

static void print_group_show_all(ucli_t *cli, const ucli_u8_group_t *G)
{
    for (uint8_t k = 0; k < G->count; k++)
    {
        const ucli_u8_item_t *it = &G->items[k];
        if (it->on_show)
            it->on_show();
        if (it->storage)
            ucli_putname_eq_u8_P(cli, it->name_P, *it->storage);
    }
}

static void handle_help(ucli_t *cli, int argc, const char *const *argv)
{
    if (argc == 1)
    {
        print_root_help(cli);
        return;
    }
    if (ucli_icmp(argv[1], "commands") == 0)
    {
        print_commands(cli);
        return;
    }

    const ucli_u8_group_t *G = find_group(cli, argv[1]);
    if (!G)
    {
        err_code(cli, 2);
        return;
    } /* unknown-group */

    if (argc == 2)
    {
        print_group_items(cli, G);
        return;
    } /* help <group> */

    /* help <group> <item> */
    const ucli_u8_item_t *it = find_item(G, argv[2]);
    if (!it)
    {
        err_code(cli, 3);
        return;
    } /* unknown-item */
    if (it->on_help)
        it->on_help();
}

static void route(ucli_t *cli, char *line, int len)
{
    const char *argv[UCLI_MAX_ARGC];
    int argc = tokenize(line, len, argv);
    if (argc <= 0)
        return;

    if (is_help_token(argv[0]))
    {
        handle_help(cli, argc, argv);
        return;
    }

    const ucli_u8_group_t *G = find_group(cli, argv[0]);
    if (!G)
    {
        err_code(cli, 2);
        return;
    } /* unknown-group */

    if (argc == 1)
    {
        print_group_items(cli, G);
        return;
    }

    if (is_help_token(argv[1]))
    {
        err_code(cli, 1);
        return;
    } /* syntax */

    if (is_show_token(argv[1]))
    {
        if (argc == 2)
        {
            print_group_show_all(cli, G);
            return;
        }
        const ucli_u8_item_t *it = find_item(G, argv[2]);
        if (!it)
        {
            err_code(cli, 3);
            return;
        } /* unknown-item */
        if (it->on_show)
            it->on_show();
        if (it->storage)
            ucli_putname_eq_u8_P(cli, it->name_P, *it->storage);
        return;
    }

    /* "<group> set <item> <u8>" */
    if (is_set_token(argv[1]))
    {
        if (argc < 4)
        {
            err_code(cli, 1);
            return;
        } /* syntax */
        const ucli_u8_item_t *it = find_item(G, argv[2]);
        if (!it)
        {
            err_code(cli, 3);
            return;
        } /* unknown-item */
        uint8_t v;
        if (!ucli_parse_u8(argv[3], &v))
        {
            err_code(cli, 4);
            return;
        } /* parse */
        if (it->on_set)
            it->on_set(v);
        if (it->storage)
            *it->storage = v;
        ucli_putname_eq_u8_P(cli, it->name_P, it->storage ? *it->storage : v);
        return;
    }

    err_code(cli, 5); /* unknown */
}

void ucli_feed_char(ucli_t *cli, char ch)
{
    unsigned char u = (unsigned char)ch;
    if (u >= 128u)
        ch = ' ';

    if (ch == '\r' || ch == '\n')
    {
        cli->line[cli->idx] = 0;
        if (cli->overflowed)
        {
            err_code(cli, 6); // "error 6"
            cli->overflowed = 0;
            cli->idx = 0;
            return;
        }
        route(cli, cli->line, cli->idx);
        cli->idx = 0;
        return;
    }

    if (ch == '\b' || u == 127u)
    {
        if (cli->idx)
            cli->idx--;
        return;
    }

    if (cli->idx < (UCLI_MAX_LINE - 1))
    {
        cli->line[cli->idx++] = ch;
    }
    else
    {
        cli->overflowed = 1;
    }
}
