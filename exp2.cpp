#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

// 对应用户要求的 unsigned double，C++ 不支持该类型，使用别名
using unsigned_double = double;

static const double PI = 3.14159265358979323846;

int generate_cover_signal(unsigned_double *cover, const int size) {
    if (!cover || size <= 0) return -1;
    // 载波：单位幅值正弦，频率 f_c = 10 Hz，采样率 fs = 1000 Hz
    double fs = 1000.0;
    double fc = 10.0;
    for (int i = 0; i < size; ++i) {
        double t = i / fs;
        cover[i] = sin(2.0 * PI * fc * t);
    }
    return size;
}

int simulate_digital_modulation_signal(unsigned char *message, const int size) {
    if (!message || size <= 0) return -1;
    // 产生重复的 0/1 码型
    for (int i = 0; i < size; ++i) {
        message[i] = (i % 4 < 2) ? 0u : 1u;
    }
    return size;
}

int simulate_analog_modulation_signal(unsigned_double *message, const int size) {
    if (!message || size <= 0) return -1;
    // 产生低频基带信号：双频正弦复合
    double fs = 1000.0;
    double fm1 = 1.0;
    double fm2 = 2.5;
    for (int i = 0; i < size; ++i) {
        double t = i / fs;
        message[i] = 0.6 * sin(2.0 * PI * fm1 * t) + 0.4 * sin(2.0 * PI * fm2 * t);
    }
    return size;
}

int modulate_digital_frequency(unsigned_double *cover, const int cover_len, const unsigned char *message, const int msg_len) {
    if (!cover || !message || cover_len <= 0 || msg_len <= 0) return -1;
    if (cover_len < msg_len * 10) return -1; // 每 bit 至少 10 采样

    double fs = 1000.0;
    double f0 = 8.0;
    double f1 = 12.0;
    int samples_per_bit = cover_len / msg_len;
    if (samples_per_bit <= 0) return -1;

    for (int i = 0; i < msg_len; ++i) {
        double f = (message[i] == 0) ? f0 : f1;
        for (int j = 0; j < samples_per_bit; ++j) {
            int idx = i * samples_per_bit + j;
            if (idx >= cover_len) break;
            double t = idx / fs;
            cover[idx] = sin(2.0 * PI * f * t);
        }
    }
    return cover_len;
}

int modulate_analog_frequency(unsigned_double *cover, const int cover_len, const unsigned_double *message, const int msg_len) {
    if (!cover || !message || cover_len <= 0 || msg_len <= 0) return -1;
    if (cover_len < msg_len) return -1;

    double fs = 1000.0;
    double fc = 10.0;
    double kf = 5.0;

    for (int i = 0; i < cover_len; ++i) {
        double t = i / fs;
        // message index 用线性插值
        double pos = (double)i * (msg_len - 1) / (cover_len - 1);
        int idx = (int)floor(pos);
        double frac = pos - idx;
        double m = message[idx];
        if (idx + 1 < msg_len) {
            m = m * (1 - frac) + message[idx + 1] * frac;
        }
        double instantaneous_f = fc + kf * m;
        cover[i] = sin(2.0 * PI * instantaneous_f * t);
    }
    return cover_len;
}

int modulate_digital_amplitude(unsigned_double *cover, const int cover_len, const unsigned char *message, const int msg_len) {
    if (!cover || !message || cover_len <= 0 || msg_len <= 0) return -1;
    if (cover_len < msg_len * 10) return -1;

    double fs = 1000.0;
    double fc = 10.0;
    int samples_per_bit = cover_len / msg_len;

    for (int i = 0; i < msg_len; ++i) {
        double amp = (message[i] == 0) ? 0.2 : 1.0;
        for (int j = 0; j < samples_per_bit; ++j) {
            int idx = i * samples_per_bit + j;
            if (idx >= cover_len) break;
            double t = idx / fs;
            cover[idx] = amp * sin(2.0 * PI * fc * t);
        }
    }
    return cover_len;
}

int modulate_analog_amplitude(unsigned_double *cover, const int cover_len, const unsigned_double *message, const int msg_len) {
    if (!cover || !message || cover_len <= 0 || msg_len <= 0) return -1;
    if (cover_len < msg_len) return -1;

    double fs = 1000.0;
    double fc = 10.0;
    double ka = 0.7;

    for (int i = 0; i < cover_len; ++i) {
        double t = i / fs;
        double pos = (double)i * (msg_len - 1) / (cover_len - 1);
        int idx = (int)floor(pos);
        double frac = pos - idx;
        double m = message[idx];
        if (idx + 1 < msg_len) {
            m = m * (1 - frac) + message[idx + 1] * frac;
        }
        double amplitude = 0.2 + ka * m; // 保证非负
        if (amplitude < 0) amplitude = 0.0;
        cover[i] = amplitude * sin(2.0 * PI * fc * t);
    }
    return cover_len;
}

int modulate_digital_phase(unsigned_double *cover, const int cover_len, const unsigned char *message, const int msg_len) {
    if (!cover || !message || cover_len <= 0 || msg_len <= 0) return -1;
    if (cover_len < msg_len * 10) return -1;

    double fs = 1000.0;
    double fc = 10.0;
    int samples_per_bit = cover_len / msg_len;

    for (int i = 0; i < msg_len; ++i) {
        double phase = (message[i] == 0) ? 0.0 : PI;
        for (int j = 0; j < samples_per_bit; ++j) {
            int idx = i * samples_per_bit + j;
            if (idx >= cover_len) break;
            double t = idx / fs;
            cover[idx] = sin(2.0 * PI * fc * t + phase);
        }
    }
    return cover_len;
}

int modulate_analog_phase(unsigned_double *cover, const int cover_len, const unsigned_double *message, const int msg_len) {
    if (!cover || !message || cover_len <= 0 || msg_len <= 0) return -1;
    if (cover_len < msg_len) return -1;

    double fs = 1000.0;
    double fc = 10.0;
    double kp = PI / 2.0;

    for (int i = 0; i < cover_len; ++i) {
        double t = i / fs;
        double pos = (double)i * (msg_len - 1) / (cover_len - 1);
        int idx = (int)floor(pos);
        double frac = pos - idx;
        double m = message[idx];
        if (idx + 1 < msg_len) {
            m = m * (1 - frac) + message[idx + 1] * frac;
        }
        double phase = kp * m;
        cover[i] = sin(2.0 * PI * fc * t + phase);
    }
    return cover_len;
}

static void printSeq(const char *title, const double *s, int len) {
    printf("%s", title);
    for (int i = 0; i < len; ++i) {
        printf(" % .3f", s[i]);
    }
    printf("\n");
}

static void printBits(const char *title, const unsigned char *s, int len) {
    printf("%s", title);
    for (int i = 0; i < len; ++i) {
        printf(" %u", s[i]);
    }
    printf("\n");
}

int main() {
    const int len = 200;
    unsigned_double cover[len];
    unsigned char msg_d[20];
    unsigned_double msg_a[20];

    generate_cover_signal(cover, len);
    printf("=== 载波信号(前20点) ===\n");
    printSeq("cover", cover, 20);

    simulate_digital_modulation_signal(msg_d, 20);
    printf("=== 数字调制信号 ===\n");
    printBits("msg_d", msg_d, 20);

    simulate_analog_modulation_signal(msg_a, 20);
    printf("=== 模拟调制信号(前20点) ===\n");
    printSeq("msg_a", msg_a, 20);

    unsigned_double out[len];

    modulate_digital_frequency(out, len, msg_d, 20);
    printf("=== 数字调频(前20点) ===\n");
    printSeq("fsk", out, 20);

    modulate_analog_frequency(out, len, msg_a, 20);
    printf("=== 模拟调频(前20点) ===\n");
    printSeq("fsk_a", out, 20);

    modulate_digital_amplitude(out, len, msg_d, 20);
    printf("=== 数字调幅(前20点) ===\n");
    printSeq("ask", out, 20);

    modulate_analog_amplitude(out, len, msg_a, 20);
    printf("=== 模拟调幅(前20点) ===\n");
    printSeq("ask_a", out, 20);

    modulate_digital_phase(out, len, msg_d, 20);
    printf("=== 数字调相(前20点) ===\n");
    printSeq("psk", out, 20);

    modulate_analog_phase(out, len, msg_a, 20);
    printf("=== 模拟调相(前20点) ===\n");
    printSeq("psk_a", out, 20);

    return 0;
}
