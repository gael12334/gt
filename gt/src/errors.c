/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "errors.h"

static elf_trace m_trace = { 0 };

// static void elf_dump_trace(void)
// {
//     char dump_filename[20] = { 0 };
//     snprintf(dump_filename, sizeof(dump_filename), "%li", elfpv.trace.timestamp);
//
//     FILE* dump = fopen(dump_filename, "a+");
//     elf_print_to_file_error_trace(dump);
//     fclose(dump);
//
//     elfpv.trace.length = 0;
//     memset(elfpv.trace.trace, 0, sizeof(elfpv.trace.trace));
// }

void elf_print_to_file_error_trace(FILE* output)
{
    if (output == NULL) {
        return;
    }

    for (size_t i = 0; i < m_trace.length; i++) {
        fprintf(output, "%zu: %i\n", i, m_trace.trace[i].code);
        fprintf(output, "\tfile: %s\n", m_trace.trace[i].file);
        fprintf(output, "\tfunc: %s\n", m_trace.trace[i].func);
        fprintf(output, "\tline: %i\n\n", m_trace.trace[i].line);
    }
}

int elf_stack_error_struct(elf_error error)
{
    if (m_trace.old) {
        m_trace.length = 0;
        m_trace.old = 0;
        time(&m_trace.timestamp);
    }

    if (m_trace.length < ELF_TRACE_SIZE) {
        size_t* i = &m_trace.length;
        m_trace.trace[*i] = error;
        (*i)++;
        (*i) = (*i) % ELF_TRACE_SIZE;
    }

    return error.code;
}

void elf_reset_error_trace(void)
{
    m_trace.old = 1;
}

int elf_has_error(void)
{
    for (size_t i = 0; i < m_trace.length; i++) {
        if (m_trace.trace[i].code != ELF_OK)
            return 1;
    }

    return 0;
}
