// Streaming sentence recognition for a temporary setup scan. No payloads are retained.
#pragma once

#include <stdint.h>
#include <string.h>

struct NavEvidence {
    unsigned zander = 0, flarm = 0, generic = 0;
    bool sufficient() const { return zander >= 3 || flarm >= 3 || generic >= 3; }
};

class NavSentences {
public:
    NavEvidence counts;

    void feed(uint8_t c) {
        if (c == '$') {
            used = 0;
            receiving = true;
        } else if (receiving) {
            if (c == '\r' || c == '\n') {
                line[used] = 0;
                classify();
                receiving = false;
            } else if (c < 32 || c > 126 || used == sizeof(line) - 1) {
                receiving = false;
            } else {
                line[used++] = char(c);
            }
        }
    }

private:
    char line[128]{};
    size_t used = 0;
    bool receiving = false;

    static int hex(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    }

    static bool digits(const char* s) {
        return *s && s[strspn(s, "0123456789")] == 0;
    }

    void classify() {
        char* star = strchr(line, '*');
        if (star) {
            if (strlen(star) != 3) return;
            int high = hex(star[1]), low = hex(star[2]);
            unsigned sum = 0;
            for (char* c = line; c != star; ++c) sum ^= *c;
            if (low < 0 || high < 0 || sum != unsigned(high * 16 + low)) return;
            *star = 0;
        }
        if (strlen(line) < 7 || line[5] != ',') return;
        char* fields[32];
        unsigned n = 0;
        char* next = line + 6;
        do {
            if (n == 32) return;
            fields[n++] = next;
            next = strchr(next, ',');
            if (next) *next++ = 0;
        } while (next);
        line[5] = 0;

        // The two stable numeric sentences appear in ZS1 program 3037 (S22).
        if (!strcmp(line, "PZAN1") || !strcmp(line, "PZAN2")) {
            if (n == 2 && digits(fields[0]) && digits(fields[1])) ++counts.zander;
        } else if (star && !strncmp(line, "PZAN", 4) && line[4] >= '3' && line[4] <= '5') {
            if (n >= 5 && *fields[0]) ++counts.zander;
        } else if (!strcmp(line, "PFLAU")) {
            // FTD-012: RX, TX, GPS, power and alarm level precede the optional alarm data.
            if (star && n >= 9 && digits(fields[0]) && digits(fields[1]) &&
                digits(fields[2]) && digits(fields[3]) && digits(fields[4])) ++counts.flarm;
        } else if (line[0] != 'P' && strspn(line, "ABCDEFGHIJKLMNOPQRSTUVWXYZ") == 5) {
            const char* type = line + 2;
            // Common navigation sentences can also arrive without a checksum.
            if ((!strcmp(type, "RMC") && n >= 9 && (!strcmp(fields[1], "A") || !strcmp(fields[1], "V"))) ||
                (!strcmp(type, "GGA") && n >= 12 && digits(fields[5])) ||
                (!strcmp(type, "RMB") && n >= 13 && (!strcmp(fields[0], "A") || !strcmp(fields[0], "V"))) ||
                (star && n >= 2 && *fields[0] && (!strcmp(type, "GLL") || !strcmp(type, "VTG") ||
                 !strcmp(type, "GSA") || !strcmp(type, "GSV") || !strcmp(type, "ZDA") || !strcmp(type, "TXT"))))
                ++counts.generic;
        }
    }
};
