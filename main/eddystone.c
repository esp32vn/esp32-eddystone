#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "eddystone.h"

#define FLAGS_UID 0x00
#define FLAGS_URL 0x10
#define FLAGS_TLM 0x20

static const char* eddystone_url_prefix_subs[] = {
    "http://www.",
    "https://www.",
    "http://",
    "https://",
    "urn:uuid:",
    NULL
};

static const char* eddystone_url_suffix_subs[] = {
    ".com/",
    ".org/",
    ".edu/",
    ".net/",
    ".info/",
    ".biz/",
    ".gov/",
    ".com",
    ".org",
    ".edu",
    ".net",
    ".info",
    ".biz",
    ".gov",
    NULL
};

static int string_ends_with(const char * str, const char * suffix)
{
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);

    if((str_len >= suffix_len) &&
        (0 == strcmp(str + (str_len - suffix_len), suffix))) {
        return suffix_len;
    }
    return -1;
}

static int string_begin_with(const char * str, const char * prefix)
{
    int prefix_len = strlen(prefix);
    if(strncmp(prefix, str, prefix_len) == 0) {
        return prefix_len;
    }
    return 0;
}
char *create_eddystone_url(const char *url, int *outlen)
{
    int scheme_idx = -1, scheme_len, ext_idx = - 1, ext_len = 1, i, idx, url_idx;
    char *ret_data;
    int url_len = strlen(url);

    ret_data = calloc(1, url_len + 13);



    ret_data[0] = 2;      // Len
    ret_data[1] = 0x01;   // Type Flags
    ret_data[2] = 0x06;   // GENERAL_DISC_MODE 0x02 | BR_EDR_NOT_SUPPORTED 0x04
    ret_data[3] = 3;      // Len
    ret_data[4] = 0x03;   // Type 16-Bit UUID
    ret_data[5] = 0xAA;   // Eddystone UUID 2 -> 0xFEAA LSB
    ret_data[6] = 0xFE;   // Eddystone UUID 1 MSB
    ret_data[7] = 19;     // Length of Beacon Data
    ret_data[8] = 0x16;   // Type Service Data
    ret_data[9] = 0xAA;   // Eddystone UUID 2 -> 0xFEAA LSB
    ret_data[10] = 0xFE;  // Eddystone UUID 1 MSB
    ret_data[11] = 0x10;  // Eddystone Frame Type
    ret_data[12] = 0x20;  // Beacons TX power at 0m

    i = 0, idx = 13, url_idx = 0;

    //replace prefix
    scheme_len = 0;
    while(eddystone_url_prefix_subs[i] != NULL) {
        if((scheme_len = string_begin_with(url, eddystone_url_prefix_subs[i])) > 0) {
            ret_data[idx] = i;
            idx ++;
            url_idx += scheme_len;
            break;
        }
        i++;
    }
    while(url_idx < url_len) {
        i = 0;
        ret_data[idx] = url[url_idx];
        ext_len = 1;
        while(eddystone_url_suffix_subs[i] != NULL) {
            if((ext_len = string_begin_with(&url[url_idx], eddystone_url_suffix_subs[i])) > 0) {
                ret_data[idx] = i;
                break;
            } else {
                ext_len = 1;//inc 1
            }
            i++;
        }
        url_idx += ext_len;
        idx ++;
    }
    *outlen = idx;
    ret_data[7] = idx - 8;
    return ret_data;

}
