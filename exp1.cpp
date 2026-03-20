#include <cstdio>
#include <cstring>
#include <algorithm>

// 统计时分多路复用（Statistical TDM）
int multiplex_statistical(unsigned char *c, const int c_size,
                         const unsigned char *a, const int a_len,
                         const unsigned char *b, const int b_len) {
    if (!c || !a || !b || c_size <= 0 || a_len < 0 || b_len < 0) return -1;
    int pos = 0;
    int ia = 0, ib = 0;
    while (ia < a_len || ib < b_len) {
        if (ia < a_len && pos < c_size) {
            c[pos++] = a[ia++] ? 1u : 0u;
        }
        if (ib < b_len && pos < c_size) {
            c[pos++] = b[ib++] ? 1u : 0u;
        }
    }
    return pos;
}

// 同步时分多路复用（Synchronous TDM）
int multiplex(unsigned char *c, const int c_size,  
              const unsigned char *a, const int a_len,
              const unsigned char *b, const int b_len) {
    if (!c || !a || !b || c_size <= 0 || a_len < 0 || b_len < 0) return -1;
    int n = std::max(a_len, b_len);
    int needed = 2 * n;
    if (c_size < needed) return -1;

    for (int i = 0; i < n; ++i) {
        c[2 * i] = (i < a_len && a[i]) ? 1u : 0u;
        c[2 * i + 1] = (i < b_len && b[i]) ? 1u : 0u;
    }
    return needed;
}

// 频分多路复用（FDM）
// 直接在一个符号里编码 a/b 状态 ： bit0->a, bit1->b
int multiplex_frequency(unsigned char *c, const int c_size,
                        const unsigned char *a, const int a_len,
                        const unsigned char *b, const int b_len) {
    if (!c || !a || !b || c_size <= 0 || a_len < 0 || b_len < 0) return -1;
    int n = std::max(a_len, b_len);
    if (c_size < n) return -1;
    for (int i = 0; i < n; ++i) {
        unsigned char va = (i < a_len && a[i]) ? 1u : 0u;
        unsigned char vb = (i < b_len && b[i]) ? 2u : 0u;
        c[i] = va | vb;
    }
    return n;
}

// 码分多路复用（CDM）
// 使用两个码片：a 的码为 {1,0}, b 的码为 {0,1}，对每个信号形成2个芯片
int multiplex_code(unsigned char *c, const int c_size,
                   const unsigned char *a, const int a_len,
                   const unsigned char *b, const int b_len) {
    if (!c || !a || !b || c_size <= 0 || a_len < 0 || b_len < 0) return -1;
    int n = std::max(a_len, b_len);
    int needed = 2 * n;
    if (c_size < needed) return -1;

    for (int i = 0; i < n; ++i) {
        unsigned char va = (i < a_len && a[i]) ? 1u : 0u;
        unsigned char vb = (i < b_len && b[i]) ? 1u : 0u;
        c[2 * i] = va;      // a 码片1
        c[2 * i + 1] = vb;  // b 码片2
    }
    return needed;
}

// 同步时分解复用（对应上面 multiplex）
int demultiplex(unsigned char *a, const int a_size,
                unsigned char *b, const int b_size,
                const unsigned char *c, const int c_len) {
    if (!a || !b || !c || a_size < 0 || b_size < 0 || c_len < 0) return -1;
    if (c_len % 2 != 0) return -1;
    int n = c_len / 2;
    if (a_size < n || b_size < n) return -1;

    for (int i = 0; i < n; ++i) {
        a[i] = c[2 * i] ? 1u : 0u;
        b[i] = c[2 * i + 1] ? 1u : 0u;
    }
    return n;
}

// 统计时分解复用
int demultiplex_statistical(unsigned char *a, const int a_size,
                            unsigned char *b, const int b_size,
                            const unsigned char *c, const int c_len,
                            const int a_len, const int b_len) {
    if (!a || !b || !c || a_size < a_len || b_size < b_len) return -1;
    if (c_len < a_len + b_len) return -1;
    // 先 a 后 b
    for (int i = 0; i < a_len; ++i) a[i] = c[i] ? 1u : 0u;
    for (int i = 0; i < b_len; ++i) b[i] = c[a_len + i] ? 1u : 0u;
    return std::max(a_len, b_len);
}

// 频分解复用
int demultiplex_frequency(unsigned char *a, const int a_size,
                          unsigned char *b, const int b_size,
                          const unsigned char *c, const int c_len) {
    if (!a || !b || !c || a_size < c_len || b_size < c_len) return -1;
    for (int i = 0; i < c_len; ++i) {
        a[i] = (c[i] & 1u) ? 1u : 0u;
        b[i] = (c[i] & 2u) ? 1u : 0u;
    }
    return c_len;
}

// 码分解复用
int demultiplex_code(unsigned char *a, const int a_size,
                      unsigned char *b, const int b_size,
                      const unsigned char *c, const int c_len) {
    if (!a || !b || !c || c_len % 2 != 0) return -1;
    int n = c_len / 2;
    if (a_size < n || b_size < n) return -1;
    for (int i = 0; i < n; ++i) {
        a[i] = c[2 * i] ? 1u : 0u;
        b[i] = c[2 * i + 1] ? 1u : 0u;
    }
    return n;
}

static void printSeq(const char *title, const unsigned char *s, int len) {
    printf("%s:", title);
    for (int i = 0; i < len; ++i) printf(" %u", (unsigned)s[i]);
    printf("\n");
}

int main() {
    unsigned char a[] = {1, 0, 1, 1, 0};
    unsigned char b[] = {0, 1, 1, 0};
    int a_len = sizeof(a) / sizeof(a[0]);
    int b_len = sizeof(b) / sizeof(b[0]);

    unsigned char c[256];
    unsigned char da[256], db[256];

    // 同步时分检查
    int clen = multiplex(c, sizeof(c), a, a_len, b, b_len);
    int out = demultiplex(da, 256, db, 256, c, clen);
    printf("\n=== 同步时分多路复用 ===\n");
    printSeq("c", c, clen);
    printSeq("da", da, out);
    printSeq("db", db, out);

    // 统计时分检查
    int st_clen = multiplex_statistical(c, sizeof(c), a, a_len, b, b_len);
    int st_out = demultiplex_statistical(da, 256, db, 256, c, st_clen, a_len, b_len);
    printf("\n=== 统计时分多路复用 ===\n");
    printSeq("c", c, st_clen);
    printSeq("da", da, a_len);
    printSeq("db", db, b_len);

    // 频分检查
    int f_clen = multiplex_frequency(c, sizeof(c), a, a_len, b, b_len);
    int f_out = demultiplex_frequency(da, 256, db, 256, c, f_clen);
    printf("\n=== 频分多路复用 ===\n");
    printSeq("c", c, f_clen);
    printSeq("da", da, f_out);
    printSeq("db", db, f_out);

    // 码分检查
    int cd_clen = multiplex_code(c, sizeof(c), a, a_len, b, b_len);
    int cd_out = demultiplex_code(da, 256, db, 256, c, cd_clen);
    printf("\n=== 码分多路复用 ===\n");
    printSeq("c", c, cd_clen);
    printSeq("da", da, cd_out);
    printSeq("db", db, cd_out);

    return 0;
}
 