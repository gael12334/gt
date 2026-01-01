/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include <dirent.h>
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * global var name : "club";
 *
 * function main(number a; string b; array c; table d; bool e; dict f) {
 *   var i : shell("echo ${a}");
 *   if ($i = 1) {
 *      print(i);
 *   }
 *
 *   $i : expr($i+1);
 *
 *   while ($e = true) {
 *     $e : false;
 *   }
 *
 *   array r : (1; 2; 3; $e;);
 *   table t : [a : 3; b : $a;];
 *   dict d :
 *
 *   r + 1 :
 *
 *   return 0;
 * }
 */

typedef struct {
    char* start;
    char* end;
} string;

typedef const void* error;

error newstring(size_t size, string* text)
{
    if (text == NULL) {
        return "text null";
    }

    size++;
    text->start = malloc(size);
    if (text->start == NULL) {
        return "malloc null";
    }

    size--;
    text->end = text->start + size;
    text->start[0] = '\0';
    text->end[0] = '\0';

    return NULL;
}

error chkstring(const string text)
{
    if (text.start == NULL) {
        return "string start null";
    }

    if (text.end == NULL) {
        return "string end null";
    }

    return NULL;
}

error delstring(const string text)
{
    if (text.start == NULL) {
        return "string start null";
    }

    free(text.start);
    return NULL;
}

error lenstring(const string text, size_t* len)
{
    if (len == NULL) {
        return "len null";
    }

    *len = text.end - text.start;
    return NULL;
}

error outstring(const string text, size_t index)
{
    size_t size = text.end - text.start;
    return (index < size) ? NULL : "invalid index";
}

error fdwstring(string text, size_t off, FILE* fd, size_t size)
{
    if (fd == NULL) {
        return "file description null";
    }

    if (outstring(text, off)) {
        return "invalid offset";
    }

    if (outstring(text, off + size - 1)) {
        return "invalid size";
    }

    fread(text.start + off, 1, size, fd);
    return NULL;
}

error bwstring(string text, size_t off, const char* b, size_t size)
{
    if (b == NULL) {
        return "buffer null";
    }

    if (outstring(text, off)) {
        return "invalid offset";
    }

    if (outstring(text, off + size - 1)) {
        return "invalid size";
    }

    memcpy(text.start + off, b, size);
    return NULL;
}

error swstring(string text, size_t off, const string s, size_t s_off, size_t size)
{
    if (outstring(text, off)) {
        return "destination invalid offset";
    }

    if (outstring(text, off + size - 1)) {
        return "destination invalid size";
    }

    if (outstring(s, s_off)) {
        return "destination invalid offset";
    }

    if (outstring(s, s_off + size - 1)) {
        return "destination invalid size";
    }

    memcpy(text.start + off, s.start, size);
    return NULL;
}

error readfile(const char* path, string* text)
{
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        return "invalid path";
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    error e = newstring(size + 1, text);
    if (e) {
        return e;
    }

    e = fdwstring(*text, 0, f, size);
    if (e) {
        return e;
    }

    fclose(f);
    return NULL;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("invalid args\n");
    }

    string file;
    error e = readfile(argv[0], &file);

    return 0;
}
