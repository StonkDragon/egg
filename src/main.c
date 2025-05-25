#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#define assert(condition) assert_msg(condition, #condition)
#define _to_string0(x) #x
#define _to_string(x) _to_string0(x)
#define assert_msg(x, message) _assert_msgf((x), __FILE__ ":" _to_string(__LINE__) ": Assertion failed: %s\n", (message))
#define assert_msgf(x, message, ...) _assert_msgf((x), __FILE__ ":" _to_string(__LINE__) ": Assertion failed: " message "\n", __VA_ARGS__)

void _assert_msgf(bool x, const char* format, ...) {
    if (!x) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        exit(EXIT_FAILURE);
    }
}

void* malloc_or_die(size_t size) {
    void* ptr = malloc(size);
    assert_msg(ptr != NULL, "Memory allocation failed");
    return ptr;
}

void* realloc_or_die(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    assert_msg(new_ptr != NULL, "Memory reallocation failed");
    return new_ptr;
}

void free_or_die(void* _ptr) {
    void** ptr = (void**) _ptr;
    assert_msg(ptr && *ptr != NULL, "Attempted to free a NULL pointer");
    free(*ptr);
    *ptr = NULL;
}

FILE* fopen_or_die(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);
    assert_msgf(file != NULL, "Could not open file %s with mode %s", filename, mode);
    return file;
}

void fclose_or_die(FILE** file) {
    assert_msg(file != NULL, "Attempted to close a NULL file pointer");
    int result = fclose(*file);
    assert_msg(result == 0, "Could not close file");
    *file = NULL; // Set to NULL to avoid dangling pointer
}

void free_and_replace(void* ptr, void* new_ptr) {
    free_or_die((void**) ptr);
    *(void**) ptr = new_ptr;
}

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} String;

#define String_reserve(da, expected_capacity)                                                     \
    do                                                                                        \
    {                                                                                         \
        if ((expected_capacity) > (da)->capacity)                                             \
        {                                                                                     \
            if ((da)->capacity == 0)                                                          \
            {                                                                                 \
                (da)->capacity = 256;                                                         \
            }                                                                                 \
            while ((expected_capacity) > (da)->capacity)                                      \
            {                                                                                 \
                (da)->capacity *= 2;                                                          \
            }                                                                                 \
            (da)->items = realloc_or_die((da)->items, (da)->capacity * sizeof(*(da)->items)); \
        }                                                                                     \
    } while (0)

#define String_appendChar(da, item)                  \
    do                                       \
    {                                        \
        String_reserve((da), (da)->count + 1);   \
        (da)->items[(da)->count++] = (item); \
    } while (0)

#define String_free(str) free_or_die(&(str).items)

#define String_appendMany(da, new_items, new_items_count)                                            \
    do                                                                                            \
    {                                                                                             \
        String_reserve((da), (da)->count + (new_items_count));                                        \
        memcpy((da)->items + (da)->count, (new_items), (new_items_count) * sizeof(*(da)->items)); \
        (da)->count += (new_items_count);                                                         \
    } while (0)

#define String_appendCStr(str, cstr)  \
    do                            \
    {                             \
        const char *s = (cstr);   \
        size_t n = strlen(s);     \
        String_appendMany(str, s, n); \
    } while (0)

#define String_appendTerminator(str) String_appendMany(str, "", 1)

typedef struct {
    String* items;
    size_t count;
    size_t capacity;
} StringList;

#define StringList_reserve(da, expected_capacity)                                                    \
    do                                                                                        \
    {                                                                                         \
        if ((expected_capacity) > (da)->capacity)                                             \
        {                                                                                     \
            if ((da)->capacity == 0)                                                          \
            {                                                                                 \
                (da)->capacity = 16;                                                          \
            }                                                                                 \
            while ((expected_capacity) > (da)->capacity)                                      \
            {                                                                                 \
                (da)->capacity *= 2;                                                          \
            }                                                                                 \
            (da)->items = realloc_or_die((da)->items, (da)->capacity * sizeof(*(da)->items)); \
        }                                                                                     \
    } while (0)

#define StringList_append(da, item)                  \
    do                                       \
    {                                        \
        StringList_reserve((da), (da)->count + 1);   \
        (da)->items[(da)->count++] = (item); \
    } while (0)

#define StringList_free(str) free_or_die(&(str).items)

const char unescaped_chars[] = {
    [0] = 0,
    ['n'] = '\n',
    ['t'] = '\t',
    ['r'] = '\r',
    ['b'] = '\b',
    ['f'] = '\f',
    ['v'] = '\v',
    ['a'] = '\a',
    ['0'] = '\0',
    ['\\'] = '\\',
    ['\''] = '\'',
    ['\"'] = '\"',
    ['x'] = 0 // 'x' is used for hex escape sequences
};

char* duplicate_string(const char* str) {
    assert(str != NULL);

    size_t len = strlen(str);
    char* result = malloc_or_die(len + 1);
    strcpy(result, str);
    return result;
}

static inline bool isUpper(char c) {
    return (c >= 'A' && c <= 'Z');
}

static inline bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}

static inline bool isLower(char c) {
    return (c >= 'a' && c <= 'z');
}

static inline bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

static inline char toUpper(char c) {
    return isLower(c) ? c - ('a' - 'A') : c;
}

static inline char toLower(char c) {
    return isUpper(c) ? c + ('a' - 'A') : c;
}

char* tf_upper(char* input) {
    assert(input != NULL);

    for (size_t i = 0; input[i]; ++i) {
        input[i] = toUpper(input[i]);
    }
    return duplicate_string(input);
}
char* tf_lower(char* input) {
    assert(input != NULL);

    for (size_t i = 0; input[i]; ++i) {
        input[i] = toLower(input[i]);
    }
    return duplicate_string(input);
}
char* tf_reverse(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    for (size_t i = 0; i < len / 2; ++i) {
        char temp = input[i];
        input[i] = input[len - i - 1];
        input[len - i - 1] = temp;
    }
    input[len] = '\0';
    return duplicate_string(input);
}
char* tf_capitalize(char* input) {
    assert(input != NULL);

    bool capitalize_next = true;
    for (size_t i = 0; input[i]; ++i) {
        if (isSpace(input[i])) {
            capitalize_next = true;
        } else if (capitalize_next) {
            input[i] = toUpper(input[i]);
            capitalize_next = false;
        }
    }
    return duplicate_string(input);
}
char* tf_decapitalize(char* input) {
    assert(input != NULL);
    
    bool decapitalize_next = true;
    for (size_t i = 0; input[i]; ++i) {
        if (isSpace(input[i])) {
            decapitalize_next = true;
        } else if (decapitalize_next) {
            input[i] = toLower(input[i]);
            decapitalize_next = false;
        }
    }
    return duplicate_string(input);
}
char* tf_duplicate(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    char* result = malloc_or_die(len * 2 + 1);
    strcpy(result, input);
    strcat(result, input);
    return result;
}
char* tf_strip(char* input) {
    assert(input != NULL);

    size_t nonSpaceChars = 0;
    for (size_t i = 0; input[i]; ++i) {
        if (!isSpace(input[i])) {
            nonSpaceChars++;
        }
    }
    size_t len = strlen(input);
    char* result = malloc_or_die(nonSpaceChars + 1);
    size_t k = 0;
    for (size_t i = 0; i < len && k < nonSpaceChars; ++i) {
        if (!isSpace(input[i])) {
            result[k++] = input[i];
        }
    }
    result[k] = '\0';
    result = realloc(result, k + 1);
    return result;
}
char* tf_trim(char* input) {
    assert(input != NULL);

    size_t start = 0;
    while (isSpace(input[start])) {
        ++start;
    }
    size_t end = strlen(input);
    while (end > start && isSpace(input[end - 1])) {
        --end;
    }
    size_t len = end - start;
    char* result = malloc_or_die(len + 1);
    strncpy(result, input + start, len);
    result[len] = '\0';
    return result;
}
char* tf_join(char* input) {
    assert(input != NULL);

    for (size_t i = 0; input[i]; ++i) {
        if (isSpace(input[i])) {
            input[i] = '_';
        }
    }
    return duplicate_string(input);
}
char* tf_escape(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    char* result = malloc_or_die(len * 2 + 1);
    size_t k = 0;
    for (size_t i = 0; input[i]; ++i) {
        for (size_t j = 0; j < sizeof(unescaped_chars); ++j) {
            if (input[i] == unescaped_chars[j]) {
                result[k++] = '\\';
                result[k++] = j;
                goto escape_outer;
            }
        }
        result[k++] = input[i];
    escape_outer: (void)0;
    }
    result[k] = '\0';
    result = realloc(result, k + 1);
    return result;
}
char* tf_unescape(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    char* result = malloc_or_die(len + 1);
    size_t k = 0;
    for (size_t i = 0; input[i]; ++i) {
        if (input[i] == '\\') {
            i++;
            result[k++] = unescaped_chars[(int) input[i]];
            if (input[i] == 'x') {
                i++;
                char hex[3] = {0};
                hex[0] = input[i++];
                hex[1] = input[i];
                int value = (int)strtol(hex, NULL, 16);
                result[k++] = (char)value;
            }
        } else {
            result[k++] = input[i];
        }
    }
    result[k] = '\0';
    result = realloc(result, k + 1);
    return result;
}
char* tf_drop(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    input[len - 1] = '\0';
    return duplicate_string(input);
}

char* tf_base64_encode(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    String result = {0};

    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (size_t i = 0; i < len; i += 3) {
        int a = input[i];
        int b = (i + 1 < len) ? input[i + 1] : 0;
        int c = (i + 2 < len) ? input[i + 2] : 0;
        String_appendChar(&result, base64_chars[(a >> 2) & 0x3F]);
        String_appendChar(&result, base64_chars[((a & 0x03) << 4) | ((b >> 4) & 0x0F)]);
        String_appendChar(&result, (i + 1 < len) ? base64_chars[((b & 0x0F) << 2) | ((c >> 6) & 0x03)] : '=');
        String_appendChar(&result, (i + 2 < len) ? base64_chars[c & 0x3F] : '=');
    }
    String_appendTerminator(&result);
    
    return result.items;
}

char* tf_base64_decode(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    if (len % 4 != 0) {
        return ""; // Invalid base64 input, return empty string
    }
    String result = {0};

    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (size_t i = 0; i < len; i += 4) {
        int a = strchr(base64_chars, input[i]) - base64_chars;
        int b = strchr(base64_chars, input[i + 1]) - base64_chars;
        int c = strchr(base64_chars, input[i + 2]) - base64_chars;
        int d = strchr(base64_chars, input[i + 3]) - base64_chars;

        String_appendChar(&result, (a << 2) | (b >> 4));
        if (input[i + 2] != '=') {
            String_appendChar(&result, ((b & 0x0F) << 4) | (c >> 2));
            if (input[i + 3] != '=') {
                String_appendChar(&result, ((c & 0x03) << 6) | d);
            }
        }
    }
    String_appendTerminator(&result);
    
    return result.items;
}

char* tf_hex_encode(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    char* result = malloc_or_die(len * 2 + 1);
    for (size_t i = 0; i < len; ++i) {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02x", (unsigned char)input[i]);
        result[i * 2] = hex[0];
        result[i * 2 + 1] = hex[1];
    }
    result[len * 2] = '\0';
    return result;
}

char* tf_hex_decode(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    if (len % 2 != 0) {
        return ""; // Invalid hex input, return empty string
    }
    char* result = malloc_or_die(len / 2 + 1);
    for (size_t i = 0; i < len; i += 2) {
        sscanf(input + i, "%2hhx", &result[i / 2]);
    }
    result[len / 2] = '\0';
    return result;
}

char* tf_xor_cipher(char* input) {
    assert(input != NULL);

    size_t len = strlen(input);
    for (size_t i = 0; i < len; ++i) {
        input[i] ^= 0xFF; // simple XOR obfuscation
    }
    return tf_hex_encode(input);
}

static const unsigned int crc_table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

unsigned int crc32(const unsigned char *buffer, unsigned int len) {
	unsigned int crc;
	crc = 0;
	crc = crc ^ 0xffffffffL;
	while(len >= 8) {
		DO8(buffer);
		len -= 8;
	}
	if(len) do {
		DO1(buffer);
	} while(--len);
	return crc ^ 0xffffffffL;
}

#undef DO1
#undef DO2
#undef DO4
#undef DO8

char* tf_crc32(char* input) {
    assert(input != NULL);

    unsigned int crc = crc32((unsigned char*)input, strlen(input));
    char* result = malloc_or_die(9);
    sprintf(result, "%08x", crc);
    return result;
}

char* tf_invert_case(char* input) {
    assert(input != NULL);

    char* result = duplicate_string(input);
    for (size_t i = 0; result[i]; ++i) {
        if (isUpper(result[i])) {
            result[i] = toLower(result[i]);
        } else if (isLower(result[i])) {
            result[i] = toUpper(result[i]);
        }
    }
    return result;
}

char* file_contents(const char* filename) {
    FILE* file = fopen_or_die(filename, "rb");
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* contents = malloc_or_die(size + 1);
    size_t read_size = fread(contents, 1, size, file);
    assert_msgf(read_size == size, "Could not read file completely! Requested %zu bytes, read %zu bytes", size, read_size);
    contents[size] = '\0';
    fclose_or_die(&file);
    return contents;
}

char* file_contents_without_lines_with_hash(const char* filename) {
    char* contents = file_contents(filename);
    
    String str = {0};
    for (size_t i = 0; contents[i]; ++i) {
        if (contents[i] == '#') {
            // skip to end of line
            while (contents[i] && contents[i] != '\n') i++;
        } else {
            String_appendChar(&str, contents[i]);
        }
    }
    String_appendTerminator(&str);
    free_or_die(&contents);
    return str.items; // return the string builder's items as the result
}

void join_path(char *out, size_t out_size, const char *a, const char *b) {
#ifdef _WIN32
    snprintf(out, out_size, "%s\\%s", a, b);
#else
    snprintf(out, out_size, "%s/%s", a, b);
#endif
}

char *find_file_path_fragment(const char *base_path, const char *needle, const char *relative_path, bool *found) {
#ifdef _WIN32
    WIN32_FIND_DATAA ffd;
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", base_path);
    HANDLE hFind = FindFirstFileA(search_path, &ffd);
    assert_msg(hFind != INVALID_HANDLE_VALUE, "Could not open directory");

    do {
        if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0)
            continue;

        char full_path[MAX_PATH];
        join_path(full_path, sizeof(full_path), base_path, ffd.cFileName);

        char new_rel_path[4096];
        if (relative_path[0])
            snprintf(new_rel_path, sizeof(new_rel_path), "%s/%s", relative_path, ffd.cFileName);
        else
            snprintf(new_rel_path, sizeof(new_rel_path), "%s", ffd.cFileName);

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char *result = find_file_path_fragment(full_path, needle, new_rel_path, found);
            if (*found) {
                FindClose(hFind);
                return result;
            }
        } else {
            if (strstr(new_rel_path, needle)) {
                *found = true;
                FindClose(hFind);
                return duplicate_string(full_path);
            }
        }
    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);
    return NULL;
#else
    DIR *dir = opendir(base_path);
    assert_msg(dir != NULL, "Could not open directory");

    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL && !*found) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        char full_path[4096];
        join_path(full_path, sizeof(full_path), base_path, dp->d_name);

        char new_rel_path[4096];
        if (relative_path[0])
            snprintf(new_rel_path, sizeof(new_rel_path), "%s/%s", relative_path, dp->d_name);
        else
            snprintf(new_rel_path, sizeof(new_rel_path), "%s", dp->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            char *result = find_file_path_fragment(full_path, needle, new_rel_path, found);
            if (*found) {
                closedir(dir);
                return result;
            }
        } else if (S_ISREG(st.st_mode)) {
            if (strstr(new_rel_path, needle)) {
                *found = true;
                closedir(dir);
                return duplicate_string(full_path);
            }
        }
    }

    closedir(dir);
    return NULL;
#endif
}

char *find_in_multiple_dirs(const char **dirs, size_t dir_count, const char *needle) {
    for (size_t i = 0; i < dir_count; ++i) {
        bool found = false;
        char *match = find_file_path_fragment(dirs[i], needle, "", &found);
        if (found)
            return match;
    }
    return NULL; // Not found in any directory
}

String read_transformation(const char* transformation, size_t* i) {
    assert(i && transformation);
    #define i (*i)
    String trans = {0};
    while (isSpace(transformation[i])) i++;
    switch (transformation[i]) {
        case '{': {
                int brace_depth = 0;
                String_appendChar(&trans, '{');
                i++;
                while ((transformation[i] != '}' || brace_depth > 0) && transformation[i] != 0) {
                    if (transformation[i] == '{') {
                        brace_depth++;
                    } else if (transformation[i] == '}') {
                        brace_depth--;
                    }
                    String_appendChar(&trans, transformation[i]);
                    i++;
                }
                if (transformation[i] == '}') {
                    String_appendChar(&trans, '}');
                } else {
                    fprintf(stderr, "Error: Unmatched '{' in transformation.\n");
                }
            }
            break;
        case '[': {
                int bracket_depth = 0;
                String_appendChar(&trans, '[');
                i++;
                while ((transformation[i] != ']' || bracket_depth > 0) && transformation[i] != 0) {
                    if (transformation[i] == '[') {
                        bracket_depth++;
                    } else if (transformation[i] == ']') {
                        bracket_depth--;
                    }
                    String_appendChar(&trans, transformation[i]);
                    i++;
                }
                if (transformation[i] == ']') {
                    String_appendChar(&trans, ']');
                } else {
                    fprintf(stderr, "Error: Unmatched '[' in transformation.\n");
                }
            }
            break;
        case '(': {
                int paren_depth = 0;
                i++;
                while ((transformation[i] != ')' || paren_depth > 0) && transformation[i] != 0) {
                    if (transformation[i] == '(') {
                        paren_depth++;
                    } else if (transformation[i] == ')') {
                        paren_depth--;
                    }
                    String_appendChar(&trans, transformation[i]);
                    i++;
                }
                if (transformation[i] != ')') {
                    fprintf(stderr, "Error: Unmatched '(' in transformation.\n");
                }
            }
            break;
        default: {
                if (transformation[i] == '\\') {
                    String_appendChar(&trans, transformation[i]);
                    i++;
                }
                String_appendChar(&trans, transformation[i]);
            }
            break;
    }
    String_appendTerminator(&trans);
    #undef i
    return trans;
}

char read_char(const char* transformation, size_t* i) {
    assert(i && transformation);
    #define i (*i)
    if (transformation[i] == '\\') {
        i++;
        switch (transformation[i]) {
            case 'x': {
                // hex
                i++;
                char hex[3] = {0};
                hex[0] = transformation[i++];
                hex[1] = transformation[i];
                int value = (int)strtol(hex, NULL, 16);
                return (char)value;
            } break;
            default:
                return unescaped_chars[(int) transformation[i]];
                break;
        }
    } else {
        return transformation[i];
    }
    #undef i
}

String read_string(const char* transformation, size_t* i) {
    assert(i && transformation);
    #define i (*i)
    String str = {0};
    if (transformation[i] == '\'') {
        i++;
        while (transformation[i] != '\'' && transformation[i] != 0) {
            String_appendChar(&str, read_char(transformation, &i));
            i++;
        }
    } else {
        // just one char
        String_appendChar(&str, read_char(transformation, &i));
    }
    String_appendTerminator(&str);
    #undef i
    return str;
}

int sort_match_replace(const void* va, const void* vb) {
    const char* a = *(const char**) va;
    const char* b = *(const char**) vb;
    // Sort by length first, then lexicographically
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    if (len_a != len_b) {
        return len_b - len_a;
    }
    return -strcmp(a, b);
}

char* run_transformation(const char* transformation, const char* input) {
    assert_msg(input && transformation, "Input and transformation must not be NULL");
    if (strlen(transformation) == 0) {
        return duplicate_string(input);
    }
    
    char* result = duplicate_string(input);
    free_or_die(&input);
    for (size_t i = 0; transformation[i]; ++i) {
        char op = transformation[i];
        
        #define checkIncrement() do { assert_msgf(transformation[i + 1], "Transformation '%s' is incomplete at position %zu: Got 0x%02x (%c)", transformation, i, transformation[i + 1]); i++; } while (0)

        switch (op) {
            case ' ':
            case '(':
            case ')':
            case '\t':
            case '\n':
            case '\r':
            case '\0': break;
            case 'u': free_and_replace(&result, tf_upper(result)); break;
            case 'l': free_and_replace(&result, tf_lower(result)); break;
            case 'r': free_and_replace(&result, tf_reverse(result)); break;
            case 'C': free_and_replace(&result, tf_capitalize(result)); break;
            case 'D': free_and_replace(&result, tf_decapitalize(result)); break;
            case 'd': free_and_replace(&result, tf_duplicate(result)); break;
            case 's': free_and_replace(&result, tf_strip(result)); break;
            case 't': free_and_replace(&result, tf_trim(result)); break;
            case 'j': free_and_replace(&result, tf_join(result)); break;
            case 'e': free_and_replace(&result, tf_escape(result)); break;
            case 'n': free_and_replace(&result, tf_unescape(result)); break;
            case '-': free_and_replace(&result, tf_drop(result)); break;
            case 'i': free_and_replace(&result, tf_invert_case(result)); break;
            
            case 'b': free_and_replace(&result, tf_base64_encode(result)); break;
            case 'B': free_and_replace(&result, tf_base64_decode(result)); break;
            case 'h': free_and_replace(&result, tf_hex_encode(result)); break;
            case 'H': free_and_replace(&result, tf_hex_decode(result)); break;
            case '^': free_and_replace(&result, tf_xor_cipher(result)); break;
            case 'c': free_and_replace(&result, tf_crc32(result)); break;
            case '.': free_and_replace(&result, duplicate_string(result)); break;
            case '|': // Split at(string)
                {
                    checkIncrement();
                    String splitStr = read_string(transformation, &i);

                    checkIncrement();
                    String transformExpr = read_transformation(transformation, &i);

                    // Step 1: Split the result
                    StringList segments = {0};

                    if (splitStr.items[0] == '\0') {
                        // Special case: split into individual characters
                        for (size_t k = 0; result[k]; ++k) {
                            String part = {0};
                            char *char_str = malloc_or_die(2);
                            char_str[0] = result[k];
                            char_str[1] = '\0';
                            String_appendCStr(&part, char_str);
                            String_appendTerminator(&part);
                            StringList_append(&segments, part);
                            free_or_die(&char_str);
                        }
                    } else {
                        char *start = result;
                        char *next = strstr(start, splitStr.items);

                        while (next) {
                            size_t len = next - start;
                            if (len > 0) {
                                String part = {0};
                                char *buffer = malloc_or_die(len + 1);
                                strncpy(buffer, start, len);
                                buffer[len] = '\0';
                                String_appendCStr(&part, buffer);
                                String_appendTerminator(&part);
                                StringList_append(&segments, part);
                                free_or_die(&buffer);
                            }
                            start = next + splitStr.count - 1;
                            next = strstr(start, splitStr.items);
                        }

                        if (*start) {
                            String tail = {0};
                            String_appendCStr(&tail, start);
                            String_appendTerminator(&tail);
                            StringList_append(&segments, tail);
                        }
                    }

                    // Step 2: Transform each segment
                    StringList transformedSegments = {0};
                    for (size_t j = 0; j < segments.count; ++j) {
                        char *transformed = run_transformation(transformExpr.items, segments.items[j].items);

                        String str = {0};
                        String_appendCStr(&str, transformed);
                        String_appendTerminator(&str);
                        StringList_append(&transformedSegments, str);

                        free_or_die(&transformed);
                    }

                    // Step 3: Rejoin transformed segments
                    String finalResult = {0};
                    for (size_t j = 0; j < transformedSegments.count; ++j) {
                        String_appendCStr(&finalResult, transformedSegments.items[j].items);
                        if (j < transformedSegments.count - 1) {
                            String_appendCStr(&finalResult, splitStr.items);  // safe even if empty
                        }
                    }
                    String_appendTerminator(&finalResult);

                    // Cleanup
                    StringList_free(segments);
                    for (size_t j = 0; j < transformedSegments.count; ++j) {
                        String_free(transformedSegments.items[j]);
                    }
                    StringList_free(transformedSegments);

                    String_free(splitStr);
                    String_free(transformExpr);

                    free_and_replace(&result, finalResult.items);
                }

                break;
            case 'a': // Append(string)
                {
                    String str = {0};
                    String_appendCStr(&str, result);
                    checkIncrement();
                    while (isSpace(transformation[i])) checkIncrement();
                    String append_str = read_string(transformation, &i);
                    String_appendCStr(&str, append_str.items);
                    String_free(append_str);

                    String_appendTerminator(&str);
                    free_and_replace(&result, str.items);
                }
                break;
            case 'p': // Prepend(string)
                {
                    checkIncrement();
                    while (isSpace(transformation[i])) checkIncrement();
                    String str = read_string(transformation, &i);
                    str.count--; // remove the null terminator
                    String_appendCStr(&str, result);
                    String_appendTerminator(&str);
                    free_and_replace(&result, str.items);
                }
                break;
            case 'x': // Remove(string)
                {
                    checkIncrement();
                    while (isSpace(transformation[i])) checkIncrement();
                    String str = read_string(transformation, &i);   
                    
                    size_t result_len = strlen(result);
                    String new_result = {0};

                    for (size_t j = 0; j < result_len; ++j) {
                        bool found = str.count != 1 && strstr(&result[j], str.items) == &result[j];
                        if (!found) {
                            String_appendChar(&new_result, result[j]);
                        } else {
                            // skip the length of the string to remove
                            j += str.count - 2; // -1 because we will increment j in the loop
                        }
                    }
                    String_appendTerminator(&new_result);
                    String_free(str);
                    free_and_replace(&result, new_result.items);
                }
                break;
            case 'E': // For Each Char
                {
                    checkIncrement();
                    String trans = read_transformation(transformation, &i);
                    
                    String str = {0};
                    for (size_t j = 0; result[j]; ++j) {
                        char* temp = malloc_or_die(2);
                        temp[0] = result[j];
                        temp[1] = '\0';
                        char* new_result = run_transformation(trans.items, temp);
                        if (new_result) {
                            String_appendCStr(&str, new_result);
                            free_or_die(&new_result);
                        }
                    }
                    String_appendTerminator(&str);
                    String_free(trans);
                    free_and_replace(&result, str.items);
                }
                break;
            case 'L': // limit length (e.g. l10)
                {
                    checkIncrement();
                    while (isSpace(transformation[i])) checkIncrement();
                    int limit = 0;
                    while (isDigit(transformation[i])) {
                        limit = limit * 10 + (transformation[i] - '0');
                        i++; // increment to skip the digit, checked by isDigit
                    }
                    i--;
                    if (limit < 0) {
                        return result; // invalid limit, return original result
                    }
                    size_t result_len = strlen(result);
                    if (result_len > (size_t)limit) {
                        char* new_result = malloc_or_die(limit + 1);
                        strncpy(new_result, result, limit);
                        new_result[limit] = '\0';
                        free_and_replace(&result, new_result);
                    }
                }
                break;
            case '@': // only for nth char (e.g. @3)
                {
                    checkIncrement();
                    while (isSpace(transformation[i])) checkIncrement();
                    size_t charN = 0;
                    while (isDigit(transformation[i])) {
                        charN = charN * 10 + (transformation[i] - '0');
                        i++; // increment to skip the digit, checked by isDigit
                    }
                    if (charN < 0) {
                        return result; // invalid charN, return original result
                    }

                    size_t result_len = strlen(result);
                    if (charN >= result_len) {
                        return result; // no change if charN is out of bounds
                    }

                    String trans = read_transformation(transformation, &i);

                    char* new_result = malloc_or_die(2);
                    new_result[0] = result[charN];
                    new_result[1] = '\0';
                    char* transformed = run_transformation(trans.items, new_result);
                    // insert the transformed char at the position of charN
                    char* new = malloc_or_die(result_len + strlen(transformed) + 1);
                    strncpy(new, result, charN);
                    strcpy(new + charN, transformed);
                    strcpy(new + charN + strlen(transformed), result + charN + 1);
                    
                    free_and_replace(&result, new);

                    free_or_die(&transformed);
                    String_free(trans);
                    free_or_die(&new_result);
                }
                break;
            case '\'':
                {
                    String name = {0};
                    checkIncrement();
                    while (transformation[i] != '\'') {
                        String_appendChar(&name, transformation[i]);
                        checkIncrement();
                    }
                    String_appendCStr(&name, ".basket");
                    String_appendTerminator(&name);
                    
                    // look through all files in the current directory and ~/.egg/**/**/
                    // if the file exists, read it and run the transformation
                    char* home = getenv("HOME");
                    assert_msg(home != NULL, "HOME environment variable is not set");
                    
                    String home_dir = {0};

                    String_appendCStr(&home_dir, home);
                    String_appendChar(&home_dir, '/');
                    String_appendCStr(&home_dir, ".egg");
                    String_appendTerminator(&home_dir);
                    char* dirs[] = {
                        home_dir.items,
                        ".",
                    };
                    
                    // add the current directory and ~/.egg/ to the search paths
                    char* file = find_in_multiple_dirs((const char**) dirs, 2, name.items);
                    assert_msgf(file != NULL, "Could not find file %s in directories", name.items);
                    String_free(home_dir);
                    
                    char* file_content = file_contents_without_lines_with_hash(file);
                    
                    free_and_replace(&result, run_transformation(file_content, duplicate_string(result)));
                    free_or_die(&file_content);
                    free_or_die(&file);
                    String_free(name);
                }
                break;
            case '{': // match and replace
                {
                    struct {
                        struct match_replace {
                            char* from;
                            char* to;
                        }* items;
                        size_t count;
                        size_t capacity;
                    } match_replace = {0};
                    checkIncrement();
                    
                    while (transformation[i] != '}' && transformation[i] != 0) {
                        while (isSpace(transformation[i])) checkIncrement();
                        String from = read_string(transformation, &i);
                        checkIncrement();
                        while (isSpace(transformation[i])) checkIncrement();
                        checkIncrement(); // skip '='
                        while (isSpace(transformation[i])) checkIncrement();
                        String to = read_string(transformation, &i);
                        checkIncrement();
                        while (isSpace(transformation[i])) checkIncrement();

                        struct match_replace item = {
                            .from = from.items,
                            .to = to.items
                        };
                        String_appendChar(&match_replace, item);
                    }

                    if (match_replace.items) {
                        qsort(match_replace.items, match_replace.count, sizeof(struct match_replace), sort_match_replace);
                    }

                    String new_result = {0};
                    for (size_t j = 0; result[j]; ++j) {
                        bool replaced = false;
                        for (size_t k = 0; k < match_replace.count; ++k) {
                            size_t from_len = strlen(match_replace.items[k].from);
                            if (strncmp(&result[j], match_replace.items[k].from, from_len) == 0) {
                                String_appendCStr(&new_result, match_replace.items[k].to);
                                replaced = true;
                                j += from_len;
                                if (from_len) j--;
                                break;
                            }
                        }
                        if (!replaced) {
                            String_appendChar(&new_result, result[j]);
                        }
                    }
                    String_appendTerminator(&new_result);
                    free_and_replace(&result, new_result.items);
                    for (size_t k = 0; k < match_replace.count; ++k) {
                        free_or_die(&match_replace.items[k].from);
                        free_or_die(&match_replace.items[k].to);
                    }
                    if (match_replace.items) {
                        String_free(match_replace);
                    }
                }
                break;
            case '[': // window of commands
                {
                    StringList commands = {0};
                    checkIncrement();
                    while (transformation[i] != ']' && transformation[i] != 0) {
                        String trans = read_transformation(transformation, &i);
                        String_appendChar(&commands, trans);
                        checkIncrement();
                    }
                    
                    if (commands.count == 0) {
                        // No commands, just return the result as is
                        return result;
                    }

                    size_t result_len = strlen(result);
                    String new_result = {0};

                    for (size_t j = 0; j < result_len; ++j) {
                        char* temp = malloc_or_die(2);
                        temp[0] = result[j];
                        temp[1] = '\0';
                        char* new = run_transformation(commands.items[j % commands.count].items, temp);
                        String_appendCStr(&new_result, new);
                        free_or_die(&new);
                    }
                    String_appendTerminator(&new_result);
                    for (size_t k = 0; k < commands.count; ++k) {
                        free_or_die(&commands.items[k].items);
                    }
                    String_free(commands);
                    free_and_replace(&result, new_result.items);
                }
                break;
            case ':': // repeat
                {
                    checkIncrement();
                    String trans = read_transformation(transformation, &i);

                    char* new_result = NULL;
                    while (1) {
                        new_result = run_transformation(trans.items, duplicate_string(result));
                        if (strcmp(new_result, result) == 0) {
                            free_and_replace(&result, new_result);
                            break; // no change, stop repeating
                        }
                        free_and_replace(&result, new_result);
                    }
                    String_free(trans);
                }
                break;

            default:
                assert_msgf(false, "Unknown transformation: %c", op);
        }
        #undef checkIncrement
    }
    return result;
}

int main(int argc, char const *argv[]) {
    String transform = {0};
    for (int i = 1; i < argc; ++i) {
        if (i > 1) {
            String_appendCStr(&transform, " ");
        }
        String_appendCStr(&transform, argv[i]);
    }
    String_appendTerminator(&transform);

    String str = {0};
    char data[512];
    while (fgets(data, sizeof(data), stdin) != NULL) {
        String_appendCStr(&str, data);
    }
    String_appendTerminator(&str);

    if (str.items) {
        char* result = run_transformation(transform.items, str.items);
        printf("%s", result);
        free_or_die(&result);
    }
    String_free(transform);
    return 0;
}
