/*
 * MODIFICATION HISTORY
 *      1.1         12:mar:03  pws     Created: straight copy from spi/emucust.
 */

#define TYPE_VOID     0x0001
#define TYPE_CHAR     0x0002
#define TYPE_INT      0x0003
#define TYPE_FLOAT    0x0004
#define TYPE_DOUBLE   0x0005
#define TYPE_VALUE    0x0006
#define TYPE_SHORT    0x0010
#define TYPE_LONG     0x0020
#define TYPE_SIGNED   0x0040
#define TYPE_UNSIGNED 0x0080

#define TYPE_POINTER  0x0100
#define TYPE_ARRAY    0x0200
#define TYPE_FUNCTION 0x0300

#define TYPE_NAMED    0x0000
#define TYPE_ENUM     0x2000
#define TYPE_STRUCT   0x4000
#define TYPE_UNION    0x5000

#define TYPE_COMPOUND 0xf000
#define TYPE_COMPLEX  0xff00
#define TYPE_BASIC    0xff0f

struct var_info {
    char *name;
    int type;
    int dimension; /* or value */
    int top_level;
    struct var_info *comp;
    struct var_info *next;
    struct var_info *next_clean;
};

extern struct var_info *top_head;

void clean_var(void);
int parse_cont(int (*)(void *), void *);
struct var_info *find_var(int, char *);
unsigned long variable_sizeof(struct var_info *);
int find_enum_symbol(char *, long *);
void struct_perror(const char *);

