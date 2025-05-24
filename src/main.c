#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define NOB_IMPLEMENTATION
#include <nob.h>

typedef struct {
    Nob_String_Builder* items;
    size_t count;
    size_t capacity;
} String_List;

#define FREE(ptr) do { if ((ptr)) free((void*) (ptr)); (ptr) = NULL; } while(0)

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

char* tf_upper(char* input) {
    if (!input) {
        return NULL;
    }
    char* result = strdup(input);
    for (size_t i = 0; result[i]; ++i) {
        result[i] = toupper(result[i]);
    }
    FREE(input);
    return result;
}
char* tf_lower(char* input) {
    if (!input) {
        return NULL;
    }
    char* result = strdup(input);
    for (size_t i = 0; result[i]; ++i) {
        result[i] = tolower(result[i]);
    }
    FREE(input);
    return result;
}
char* tf_reverse(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < len; ++i) {
        result[i] = input[len - i - 1];
    }
    result[len] = '\0';
    FREE(input);
    return result;
}
char* tf_capitalize(char* input) {
    if (!input) {
        return NULL;
    }
    char* result = strdup(input);
    bool capitalize_next = true;
    for (size_t i = 0; result[i]; ++i) {
        if (isspace(result[i])) {
            capitalize_next = true;
        } else if (capitalize_next) {
            result[i] = toupper(result[i]);
            capitalize_next = false;
        }
    }
    FREE(input);
    return result;
}
char* tf_decapitalize(char* input) {
    if (!input) {
        return NULL;
    }
    char* result = strdup(input);
    bool decapitalize_next = true;
    for (size_t i = 0; result[i]; ++i) {
        if (isspace(result[i])) {
            decapitalize_next = true;
        } else if (decapitalize_next) {
            result[i] = tolower(result[i]);
            decapitalize_next = false;
        }
    }
    FREE(input);
    return result;
}
char* tf_duplicate(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len * 2 + 1);
    if (!result) {
        return NULL;
    }
    strcpy(result, input);
    strcat(result, input);
    FREE(input);
    return result;
}
char* tf_strip(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
    size_t k = 0;
    for (size_t i = 0; i < len; ++i) {
        if (!isspace(input[i])) {
            result[k++] = input[i];
        }
    }
    result[k] = '\0';
    FREE(input);
    return result;
}
char* tf_trim(char* input) {
    if (!input) {
        return NULL;
    }
    size_t start = 0;
    while (isspace(input[start])) {
        ++start;
    }
    size_t end = strlen(input);
    while (end > start && isspace(input[end - 1])) {
        --end;
    }
    size_t len = end - start;
    char* result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
    strncpy(result, input + start, len);
    result[len] = '\0';
    FREE(input);
    return result;
}
char* tf_join(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
    size_t k = 0;
    for (size_t i = 0; input[i]; ++i) {
        if (isspace(input[i])) {
            result[k++] = '_';
        } else {
            result[k++] = input[i];
        }
    }
    result[k] = '\0';
    FREE(input);
    return result;
}
char* tf_escape(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len * 2 + 1);
    if (!result) {
        return NULL;
    }
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
    FREE(input);
    return result;
}
char* tf_unescape(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
    size_t k = 0;
    for (size_t i = 0; input[i]; ++i) {
        if (input[i] == '\\') {
            i++;
            result[k++] = unescaped_chars[input[i]];
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
    FREE(input);
    return result;
}
char* tf_drop(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = strdup(input);
    if (!result) {
        return NULL;
    }
    result[len - 1] = '\0';
    FREE(input);
    return result;
}

char* tf_base64_encode(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    Nob_String_Builder result = {0};

    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t k = 0;
    for (size_t i = 0; i < len; i += 3) {
        int a = input[i];
        int b = (i + 1 < len) ? input[i + 1] : 0;
        int c = (i + 2 < len) ? input[i + 2] : 0;
        nob_da_append(&result, base64_chars[(a >> 2) & 0x3F]);
        nob_da_append(&result, base64_chars[((a & 0x03) << 4) | ((b >> 4) & 0x0F)]);
        nob_da_append(&result, (i + 1 < len) ? base64_chars[((b & 0x0F) << 2) | ((c >> 6) & 0x03)] : '=');
        nob_da_append(&result, (i + 2 < len) ? base64_chars[c & 0x3F] : '=');
    }
    nob_sb_append_null(&result);
    
    FREE(input);
    return result.items;
}

char* tf_base64_decode(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    if (len % 4 != 0) {
        return NULL;
    }
    Nob_String_Builder result = {0};

    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (size_t i = 0; i < len; i += 4) {
        int a = strchr(base64_chars, input[i]) - base64_chars;
        int b = strchr(base64_chars, input[i + 1]) - base64_chars;
        int c = strchr(base64_chars, input[i + 2]) - base64_chars;
        int d = strchr(base64_chars, input[i + 3]) - base64_chars;

        nob_da_append(&result, (a << 2) | (b >> 4));
        if (input[i + 2] != '=') {
            nob_da_append(&result, ((b & 0x0F) << 4) | (c >> 2));
            if (input[i + 3] != '=') {
                nob_da_append(&result, ((c & 0x03) << 6) | d);
            }
        }
    }
    nob_sb_append_null(&result);
    
    FREE(input);
    return result.items;
}

char* tf_hex_encode(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len * 2 + 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < len; ++i) {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02x", (unsigned char)input[i]);
        result[i * 2] = hex[0];
        result[i * 2 + 1] = hex[1];
    }
    result[len * 2] = '\0';
    FREE(input);
    return result;
}

char* tf_hex_decode(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    if (len % 2 != 0) {
        return NULL;
    }
    char* result = malloc(len / 2 + 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < len; i += 2) {
        sscanf(input + i, "%2hhx", &result[i / 2]);
    }
    result[len / 2] = '\0';
    FREE(input);
    return result;
}

char* tf_xor_cipher(char* input) {
    if (!input) {
        return NULL;
    }
    size_t len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < len; ++i) {
        result[i] = input[i] ^ 0xFF; // simple XOR obfuscation
    }
    result[len] = '\0';
    FREE(input);
    return tf_hex_encode(result);
}

static const unsigned int crc_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
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
    if (!input) {
        return NULL;
    }
    unsigned int crc = crc32((unsigned char*)input, strlen(input));
    char* result = malloc(9);
    if (!result) {
        return NULL;
    }
    sprintf(result, "%08x", crc);
    FREE(input);
    return result;
}

char* tf_invert_case(char* input) {
    if (!input) {
        return NULL;
    }
    char* result = strdup(input);
    for (size_t i = 0; result[i]; ++i) {
        if (isupper(result[i])) {
            result[i] = tolower(result[i]);
        } else if (islower(result[i])) {
            result[i] = toupper(result[i]);
        }
    }
    FREE(input);
    return result;
}

char* file_contents(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* contents = malloc(size + 1);
    if (!contents) {
        fprintf(stderr, "Error: Could not allocate memory for file contents\n");
        fclose(file);
        return NULL;
    }
    size_t read_size = fread(contents, 1, size, file);
    if (read_size != size) {
        fprintf(stderr, "Error: Could not read file %s completely\n", filename);
        free(contents);
        fclose(file);
        return NULL;
    }
    contents[size] = '\0'; // Null-terminate the string
    fclose(file);
    return contents;
}

char* file_contents_without_lines_with_hash(const char* filename) {
    char* contents = file_contents(filename);
    if (!contents) {
        return NULL;
    }
    // strip lines with #
    Nob_String_Builder sb = {0};
    for (size_t i = 0; contents[i]; ++i) {
        if (contents[i] == '#') {
            // skip to end of line
            while (contents[i] && contents[i] != '\n') i++;
        } else {
            nob_da_append(&sb, contents[i]);
        }
    }
    nob_sb_append_null(&sb);
    FREE(contents);
    return sb.items; // return the string builder's items as the result
}

char *find_file_path_fragment(const char *base_path, const char *needle, const char *relative_path, bool *found) {
    DIR *dir = opendir(base_path);
    if (!dir) return NULL;

    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL && !*found) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, dp->d_name);

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
                return strdup(full_path);
            }
        }
    }

    closedir(dir);
    return NULL;
}

char *find_in_multiple_dirs(const char **dirs, size_t dir_count, const char *needle) {
    for (size_t i = 0; i < dir_count; ++i) {
        bool found = false;
        char *match = find_file_path_fragment(dirs[i], needle, "", &found);
        if (found)
            return match;
    }
    return NULL;
}

Nob_String_Builder read_transformation(const char* transformation, size_t* i) {
    #define i (*i)
    Nob_String_Builder trans = {0};
    while (isspace(transformation[i])) i++;
    if (transformation[i] == '{') {
        int brace_depth = 0;
        nob_da_append(&trans, '{');
        i++;
        while ((transformation[i] != '}' || brace_depth > 0) && transformation[i] != 0) {
            if (transformation[i] == '{') {
                brace_depth++;
            } else if (transformation[i] == '}') {
                brace_depth--;
            }
            nob_da_append(&trans, transformation[i]);
            i++;
        }
        if (transformation[i] == '}') {
            nob_da_append(&trans, '}');
        } else {
            fprintf(stderr, "Error: Unmatched '{' in transformation.\n");
        }
    } else if (transformation[i] == '[') {
        int bracket_depth = 0;
        nob_da_append(&trans, '[');
        i++;
        while ((transformation[i] != ']' || bracket_depth > 0) && transformation[i] != 0) {
            if (transformation[i] == '[') {
                bracket_depth++;
            } else if (transformation[i] == ']') {
                bracket_depth--;
            }
            nob_da_append(&trans, transformation[i]);
            i++;
        }
        if (transformation[i] == ']') {
            nob_da_append(&trans, ']');
        } else {
            fprintf(stderr, "Error: Unmatched '[' in transformation.\n");
        }
    } else if (transformation[i] == '(') {
        int paren_depth = 0;
        i++;
        while ((transformation[i] != ')' || paren_depth > 0) && transformation[i] != 0) {
            if (transformation[i] == '(') {
                paren_depth++;
            } else if (transformation[i] == ')') {
                paren_depth--;
            }
            nob_da_append(&trans, transformation[i]);
            i++;
        }
        if (transformation[i] != ')') {
            fprintf(stderr, "Error: Unmatched '(' in transformation.\n");
        }
    } else {
        // single char
        if (transformation[i] == '\\') {
            nob_da_append(&trans, transformation[i]);
            i++;
        }
        nob_da_append(&trans, transformation[i]);
    }
    nob_sb_append_null(&trans);
    #undef i
    return trans;
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
    if (!input || !transformation) {
        return NULL;
    }
    if (strlen(transformation) == 0) {
        return strdup(input);
    }
    
    char* result = strdup(input);
    if (!result) {
        return NULL;
    }
    FREE(input);
    for (size_t i = 0; transformation[i]; ++i) {
        char op = transformation[i];
        switch (op) {
            case 'u': result = tf_upper(result); break;
            case 'l': result = tf_lower(result); break;
            case 'r': result = tf_reverse(result); break;
            case 'C': result = tf_capitalize(result); break;
            case 'D': result = tf_decapitalize(result); break;
            case 'd': result = tf_duplicate(result); break;
            case 's': result = tf_strip(result); break;
            case 't': result = tf_trim(result); break;
            case 'j': result = tf_join(result); break;
            case 'e': result = tf_escape(result); break;
            case 'n': result = tf_unescape(result); break;
            case '-': result = tf_drop(result); break;
            case 'i': result = tf_invert_case(result); break;
            
            case 'b': result = tf_base64_encode(result); break;
            case 'B': result = tf_base64_decode(result); break;
            case 'h': result = tf_hex_encode(result); break;
            case 'H': result = tf_hex_decode(result); break;
            case '^': result = tf_xor_cipher(result); break;
            case 'c': result = tf_crc32(result); break;
            
            case 'a': // Append(string)
                {
                    Nob_String_Builder sb = {0};
                    nob_sb_append_cstr(&sb, result);
                    i++;
                    while (isspace(transformation[i])) i++;
                    if (transformation[i] != '\'') {
                        // just one char
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
                                    nob_da_append(&sb, (char)value);
                                } break;
                                default:
                                    nob_da_append(&sb, unescaped_chars[transformation[i]]);
                                    break;
                            }
                        } else {
                            nob_da_append(&sb, transformation[i]);
                        }
                    } else {
                        // string
                        i++;
                        while (transformation[i] != '\'' && transformation[i] != 0) {
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
                                        nob_da_append(&sb, (char)value);
                                    } break;
                                    default:
                                        nob_da_append(&sb, unescaped_chars[transformation[i]]);
                                        break;
                                }
                            } else {
                                nob_da_append(&sb, transformation[i]);
                            }
                            i++;
                        }
                    }
                    nob_sb_append_null(&sb);
                    FREE(result);
                    result = sb.items;
                }
                break;
            case 'p': // Prepend(string)
                {
                    Nob_String_Builder sb = {0};
                    i++;
                    while (isspace(transformation[i])) i++;
                    if (transformation[i] != '\'') {
                        // just one char
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
                                    nob_da_append(&sb, (char)value);
                                } break;
                                default:
                                    nob_da_append(&sb, unescaped_chars[transformation[i]]);
                                    break;
                            }
                        } else {
                            nob_da_append(&sb, transformation[i]);
                        }
                    } else {
                        // string
                        i++;
                        while (transformation[i] != '\'' && transformation[i] != 0) {
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
                                        nob_da_append(&sb, (char)value);
                                    } break;
                                    default:
                                        nob_da_append(&sb, unescaped_chars[transformation[i]]);
                                        break;
                                }
                            } else {
                                nob_da_append(&sb, transformation[i]);
                            }
                            i++;
                        }
                    }
                    nob_sb_append_null(&sb);
                    size_t result_len = strlen(result);
                    char* new_result = malloc(result_len + sb.count + 1);
                    if (!new_result) {
                        FREE(result);
                        return NULL;
                    }
                    strcpy(new_result, sb.items);
                    strcat(new_result, result);
                    FREE(result);
                    result = new_result;
                }
                break;
            case 'x': // Remove(string)
                {
                    Nob_String_Builder sb = {0};
                    i++;
                    while (isspace(transformation[i])) i++;
                    if (transformation[i] != '\'') {
                        // just one char
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
                                    nob_da_append(&sb, (char)value);
                                } break;
                                default:
                                    nob_da_append(&sb, unescaped_chars[transformation[i]]);
                                    break;
                            }
                        } else {
                            nob_da_append(&sb, transformation[i]);
                        }
                    } else {
                        // string
                        i++;
                        while (transformation[i] != '\'' && transformation[i] != 0) {
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
                                        nob_da_append(&sb, (char)value);
                                    } break;
                                    default:
                                        nob_da_append(&sb, unescaped_chars[transformation[i]]);
                                        break;
                                }
                            } else {
                                nob_da_append(&sb, transformation[i]);
                            }
                            i++;
                        }
                    }
                    nob_sb_append_null(&sb);
                    
                    size_t result_len = strlen(result);
                    Nob_String_Builder new_result = {0};

                    for (size_t j = 0; j < result_len; ++j) {
                        bool found = strstr(&result[j], sb.items) == &result[j];
                        if (!found) {
                            nob_da_append(&new_result, result[j]);
                        } else {
                            // skip the length of the string to remove
                            j += sb.count - 2; // -1 because we will increment j in the loop
                        }
                    }
                    nob_sb_append_null(&new_result);
                    FREE(result);
                    result = new_result.items;
                }
                break;
            case 'E': // For Each Char
                {
                    i++;
                    Nob_String_Builder trans = read_transformation(transformation, &i);
                    
                    Nob_String_Builder sb = {0};
                    for (size_t j = 0; result[j]; ++j) {
                        char* temp = malloc(2);
                        if (!temp) {
                            FREE(result);
                            return NULL;
                        }
                        temp[0] = result[j];
                        temp[1] = '\0';
                        char* new_result = run_transformation(trans.items, temp);
                        if (new_result) {
                            nob_sb_append_cstr(&sb, new_result);
                            FREE(new_result);
                        }
                    }
                    nob_sb_free(trans);
                    nob_sb_append_null(&sb);
                    FREE(result);
                    result = sb.items;
                }
                break;
            case 'L': // limit length (e.g. l10)
                {
                    i++;
                    while (isspace(transformation[i])) i++;
                    int limit = 0;
                    while (isdigit(transformation[i])) {
                        limit = limit * 10 + (transformation[i] - '0');
                        i++;
                    }
                    i--;
                    if (limit < 0) {
                        return result; // invalid limit, return original result
                    }
                    size_t result_len = strlen(result);
                    if (result_len > (size_t)limit) {
                        char* new_result = malloc(limit + 1);
                        if (!new_result) {
                            FREE(result);
                            return NULL;
                        }
                        strncpy(new_result, result, limit);
                        new_result[limit] = '\0';
                        FREE(result);
                        result = new_result;
                    } else {
                        char* new = strdup(result); // ensure null-termination
                        if (!new) {
                            return NULL;
                        }
                        FREE(result);
                        result = new;
                    }
                }
                break;
            case '@': // only for nth char (e.g. @3)
                {
                    i++;
                    while (isspace(transformation[i])) i++;
                    int charN = 0;
                    while (isdigit(transformation[i])) {
                        charN = charN * 10 + (transformation[i] - '0');
                        i++;
                    }
                    if (charN < 0) {
                        return result; // invalid charN, return original result
                    }

                    size_t result_len = strlen(result);
                    if (charN >= result_len) {
                        return result; // no change if charN is out of bounds
                    }

                    Nob_String_Builder trans = read_transformation(transformation, &i);

                    char* new_result = malloc(2);
                    if (!new_result) {
                        FREE(result);
                        return NULL;
                    }
                    new_result[0] = result[charN];
                    new_result[1] = '\0';
                    char* transformed = run_transformation(trans.items, new_result);
                    // insert the transformed char at the position of charN
                    if (transformed) {
                        char* new = malloc(result_len + strlen(transformed) + 1);
                        if (!new) {
                            FREE(transformed);
                            FREE(result);
                            return NULL;
                        }
                        strncpy(new, result, charN);
                        strcpy(new + charN, transformed);
                        strcpy(new + charN + strlen(transformed), result + charN + 1);
                        FREE(result);
                        result = new;
                        FREE(transformed);
                    } else {
                        FREE(result);
                        return NULL;
                    }
                    nob_sb_free(trans);
                }
                break;
            case '\'':
                {
                    Nob_String_Builder command = {0};
                    i++;
                    while (transformation[i] != '\'' && transformation[i] != 0) {
                        nob_da_append(&command, transformation[i]);
                        i++;
                    }
                    nob_sb_append_cstr(&command, ".basket");
                    nob_sb_append_null(&command);
                    char* name = strdup(command.items);
                    nob_sb_free(command);
                    command = (Nob_String_Builder) {0};

                    if (!name) {
                        fprintf(stderr, "Could not allocate memory for file name.\n");
                        return result;
                    }
                    // look through all files in the current directory and ~/.egg/**/**/
                    // if the file exists, read it and run the transformation
                    char* home = getenv("HOME");
                    Nob_String_Builder home_dir = {0};
                    if (home) {
                        nob_sb_append_cstr(&home_dir, home);
                        nob_da_append(&home_dir, '/');
                    } else {
                        fprintf(stderr, "Could not find HOME environment variable.\n");
                        FREE(name);
                        return result;
                    }
                    nob_sb_append_cstr(&home_dir, ".egg");
                    nob_sb_append_null(&home_dir);
                    char* dirs[] = {
                        home_dir.items,
                        ".",
                    };
                    
                    // add the current directory and ~/.egg/ to the search paths
                    char* file = find_in_multiple_dirs((const char**) dirs, 2, name);
                    nob_sb_free(home_dir);
                    if (!file) {
                        fprintf(stderr, "Could not find file: %s\n", name);
                        FREE(name);
                        return result;
                    }
                    
                    char* file_content = file_contents_without_lines_with_hash(file);
                    if (!file_content) {
                        fprintf(stderr, "Could not read file: %s\n", file);
                        FREE(file);
                        FREE(name);
                        return result;
                    }
                    result = run_transformation(file_content, result);
                    FREE(file_content);
                    FREE(command.items);
                    FREE(file);
                    FREE(name);
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
                    i++;
                    
                    while (transformation[i] != '}' && transformation[i] != 0) {
                        Nob_String_Builder from = {0};
                        Nob_String_Builder to = {0};
                        while (isspace(transformation[i])) i++;
                        if (transformation[i] == '\'') {
                            i++;
                            while (transformation[i] != '\'' && transformation[i] != 0) {
                                if (transformation[i] == '\\') {
                                    i++;
                                    char unescaped = unescaped_chars[transformation[i]];
                                    if (unescaped) {
                                        nob_da_append(&from, unescaped);
                                    }
                                } else {
                                    nob_da_append(&from, transformation[i]);
                                }
                                i++;
                            }
                        } else {
                            fprintf(stderr, "Error: Expected ' after match string.\n");
                            nob_sb_free(from);
                            nob_sb_free(to);
                            FREE(result);
                            return NULL;
                        }
                        nob_sb_append_null(&from);
                        i++;
                     
                        while (isspace(transformation[i])) i++;
                        i++; // skip '='
                     
                        while (isspace(transformation[i])) i++;
                        if (transformation[i] == '\'') {
                            i++;
                            while (transformation[i] != '\'' && transformation[i] != 0) {
                                if (transformation[i] == '\\') {
                                    i++;
                                    char unescaped = unescaped_chars[transformation[i]];
                                    if (unescaped) {
                                        nob_da_append(&to, unescaped);
                                    }
                                } else {
                                    nob_da_append(&to, transformation[i]);
                                }
                                i++;
                            }
                        } else {
                            fprintf(stderr, "Error: Expected ' after replacement string.\n");
                            nob_sb_free(from);
                            nob_sb_free(to);
                            FREE(result);
                            return NULL;
                        }
                        
                        nob_sb_append_null(&to);
                        i++;

                        while (isspace(transformation[i])) i++;
                        
                        struct match_replace item = {
                            .from = from.items,
                            .to = to.items
                        };
                        nob_da_append(&match_replace, item);
                    }

                    qsort(match_replace.items, match_replace.count, sizeof(struct match_replace), sort_match_replace);

                    Nob_String_Builder new_result = {0};
                    for (size_t j = 0; result[j]; ++j) {
                        bool replaced = false;
                        for (size_t k = 0; k < match_replace.count; ++k) {
                            size_t from_len = strlen(match_replace.items[k].from);
                            if (strncmp(&result[j], match_replace.items[k].from, from_len) == 0) {
                                nob_sb_append_cstr(&new_result, match_replace.items[k].to);
                                replaced = true;
                                j += from_len;
                                if (from_len) j--;
                                break;
                            }
                        }
                        if (!replaced) {
                            nob_da_append(&new_result, result[j]);
                        }
                    }
                    nob_sb_append_null(&new_result);
                    FREE(result);
                    result = new_result.items;
                }
                break;
            case '[': // window of commands
                {
                    String_List commands = {0};
                    i++;
                    while (transformation[i] != ']' && transformation[i] != 0) {
                        Nob_String_Builder trans = read_transformation(transformation, &i);
                        nob_da_append(&commands, trans);
                        i++;
                    }
                    
                    size_t result_len = strlen(result);
                    Nob_String_Builder new_result = {0};

                    for (size_t j = 0; j < result_len; ++j) {
                        char* temp = malloc(2);
                        if (!temp) {
                            nob_sb_free(new_result);
                            for (size_t k = 0; k < commands.count; ++k) {
                                FREE(commands.items[k].items);
                            }
                            nob_da_free(commands);
                            return NULL;
                        }
                        temp[0] = result[j];
                        temp[1] = '\0';
                        char* new = run_transformation(commands.items[j % commands.count].items, temp);
                        if (new) {
                            nob_sb_append_cstr(&new_result, new);
                            FREE(new);
                        }
                    }
                    nob_sb_append_null(&new_result);
                    FREE(result);
                    result = new_result.items;
                    for (size_t k = 0; k < commands.count; ++k) {
                        FREE(commands.items[k].items);
                    }
                    nob_da_free(commands);
                }
                break;
            case ':': // repeat
                {
                    i++;
                    Nob_String_Builder trans = read_transformation(transformation, &i);

                    char* new_result = NULL;
                    do {
                        char* old_result = strdup(result);
                        new_result = run_transformation(trans.items, result);
                        if (new_result && strcmp(new_result, old_result) == 0) {
                            FREE(old_result);
                            result = new_result;
                            break; // no change, stop repeating
                        }
                        FREE(old_result);
                        result = new_result;
                    } while (result != NULL);
                }
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '\0':
                // ignore whitespace
                break;
                
            default:
                fprintf(stderr, "Unknown transformation: %c\n", op);
                FREE(result);
                return NULL;
        }
    }
    return result;
}

int main(int argc, char const *argv[]) {
    Nob_String_Builder transform = {0};
    for (int i = 1; i < argc; ++i) {
        if (i > 1) {
            nob_sb_append_cstr(&transform, " ");
        }
        nob_sb_append_cstr(&transform, argv[i]);
    }
    nob_sb_append_null(&transform);
    const char* transformation = transform.items;

    Nob_String_Builder sb = {0};
    char data[512];
    while (fgets(data, sizeof(data), stdin) != NULL) {
        nob_sb_append_cstr(&sb, data);
    }
    nob_sb_append_null(&sb);

    if (sb.items) {
        printf("%s", run_transformation(transformation, sb.items));
    }
    return 0;
}
