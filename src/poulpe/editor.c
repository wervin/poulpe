#include <stdlib.h>

#include <cimgui.h>

#include "poulpe/editor.h"
#include "poulpe/log.h"
#include "poulpe/text.h"
#include "poulpe/theme.h"
#include "poulpe/io.h"
#include "poulpe/event.h"

#include "poulpe/component.h"
#include "poulpe/components/textview.h"

#include "sake/vector.h"

struct _editor
{    
    poulpe_text text;
    struct poulpe_textview *textview;
};

struct _editor _editor = {0};

enum poulpe_error poulpe_editor_init(void)
{    
    enum poulpe_error error = POULPE_ERROR_NONE;

    error = poulpe_editor_open_file("../../README.md");
    if (error != POULPE_ERROR_NONE)
        return error;

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_editor_draw(void)
{   
    enum poulpe_error error = POULPE_ERROR_NONE;

    if (!igBegin("Poulpe", NULL, ImGuiWindowFlags_HorizontalScrollbar))
        goto end;

    error = poulpe_io_handle_keyboard((struct poulpe_component *) _editor.textview);
    if (error != POULPE_ERROR_NONE)
        goto end;
    
    error = poulpe_io_handle_mouse((struct poulpe_component *) _editor.textview);
    if (error != POULPE_ERROR_NONE)
        goto end;
    
    error = poulpe_component_draw((struct poulpe_component *) _editor.textview);
    if (error != POULPE_ERROR_NONE)
        goto end;

end:
    igEnd();
    return error;
}

enum poulpe_error poulpe_editor_open_file(const char *filename)
{
    uint32_t size;
    uint8_t *buffer;

    FILE *fd = fopen(filename, "r");
    if (!fd)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, filename);
        return POULPE_ERROR_MEMORY;
    }

    fseek(fd, 0, SEEK_END);
    size = ftell(fd);

    buffer = malloc(size * sizeof(uint8_t));
    if (buffer == NULL)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate memory");
        return POULPE_ERROR_MEMORY;
    }

    rewind(fd);
    fread(buffer, 1, size, fd);
    fclose(fd);

    _editor.text = poulpe_text_new();
    if (!_editor.text)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate text");
        return POULPE_ERROR_MEMORY;
    }

    poulpe_line line = poulpe_line_new();
    if (!line)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
        return POULPE_ERROR_MEMORY;
    }

    for (uint32_t i = 0; i < size; i++)
    {
        if (buffer[i] == '\r')
            continue;
        
        if (buffer[i] == '\n')
        {
            _editor.text = poulpe_text_push_back(_editor.text, &line);
            if (!_editor.text)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
                return POULPE_ERROR_MEMORY;
            }

            line = poulpe_line_new();
            if (!line)
            {
                POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to allocate line");
                return POULPE_ERROR_MEMORY;
            }
            
            continue;
        }

        struct poulpe_glyph g = {.c = buffer[i]};
        line = poulpe_line_push_back(line, &g);
        if (!line)
        {
            POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back glyph");
            return POULPE_ERROR_MEMORY;
        }
    }

    if (sake_vector_size(line))
    {
        _editor.text = poulpe_text_push_back(_editor.text, &line);
        if (!_editor.text)
        {
            POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Failed to push back line");
            return POULPE_ERROR_MEMORY;
        }
    }

    free(buffer);

    _editor.textview = (struct poulpe_textview *) poulpe_component_new(POULPE_COMPONENT_TYPE_TEXTVIEW);
    poulpe_textview_set_text(_editor.textview, _editor.text);

    return POULPE_ERROR_NONE;
}

void poulpe_editor_destroy(void)
{
    poulpe_component_free((struct poulpe_component *) _editor.textview);
    poulpe_text_free(_editor.text);
}