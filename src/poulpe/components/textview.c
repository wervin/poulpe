#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include <sake/vector.h>
#include <sake/string.h>
#include <sake/macro.h>

#include "poulpe/components/textview.h"
#include "poulpe/components/cursor.h"
#include "poulpe/components/selection.h"
#include "poulpe/components/linenumber.h"
#include "poulpe/components/textedit.h"

#include "poulpe/log.h"
#include "poulpe/theme.h"

struct poulpe_textview * poulpe_textview_new(void)
{
    struct poulpe_textview *textview;

    textview = calloc(1, sizeof(struct poulpe_textview));
    if (!textview)
    {
        POULPE_LOG_ERROR(POULPE_ERROR_MEMORY, "Cannot allocate textview");
        return NULL;
    }

    textview->textedit = (struct poulpe_textedit *) poulpe_component_new(POULPE_COMPONENT_TYPE_TEXTEDIT);
    if (!textview->textedit)
        return NULL;

    textview->linenumber = (struct poulpe_linenumber *) poulpe_component_new(POULPE_COMPONENT_TYPE_LINENUMBER);
    if (!textview->linenumber)
        return NULL;

    poulpe_linenumber_set_textedit(textview->linenumber, textview->textedit);
    
    return textview;
}

void poulpe_textview_free(struct poulpe_textview *textview)
{
    poulpe_component_free((struct poulpe_component *) textview->linenumber);
    poulpe_component_free((struct poulpe_component *) textview->textedit);
    free(textview);
}

enum poulpe_error poulpe_textview_draw(struct poulpe_textview *textview)
{
    poulpe_component_draw((struct poulpe_component *)textview->linenumber);
    poulpe_component_draw((struct poulpe_component *)textview->textedit);

    return POULPE_ERROR_NONE;
}

enum poulpe_error poulpe_textview_notify(struct poulpe_textview *textview, struct poulpe_event *event)
{
    return poulpe_component_notify((struct poulpe_component *) textview->textedit, event);
}

void poulpe_textview_set_textbuffer(struct poulpe_textview *textview, struct poulpe_textbuffer *textbuffer)
{
    poulpe_textedit_set_textbuffer(textview->textedit, textbuffer);
}