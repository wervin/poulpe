#include "poulpe/theme.h"

#define GREY1           {.80f, .80f, .80f, 1.0f}
#define GREY2           {.85f, .85f, .85f, 1.0f}
#define GREY3           {.55f, .55f, .55f, 1.0f}
#define GREY4           {.4f, .4f, .4f, .55f}
#define GREY5           {.5f, .5f, .5f, 1.0f}
#define GREY6           {.65f, .65f, .65f, 1.0f}
#define GREY7           {.45f, .45f, .45f, .55f}
#define GREY8           {.3f, .3f, .3, 1.0f}

#define WHITE1          {1.0f, 1.0f, 1.0f, 1.0f}
#define WHITE2          {1.0f, 1.0f, 1.0f, .9f}
#define WHITE3          {.98f, .98f, .98f, 1.0f}

#define GREEN1          {0.1f, .4f, .1f, 1.0f}
#define GREEN2          {.15f, .85f, .15f, 1.0f}
#define GREEN3          {.13f, 0.6f, .13f, 1.0f}
#define GREEN4          {.13f, .4f, .13f, 1.0f}
#define GREEN5          {.2f, 0.8f, 0.2f, 1.0f}
#define GREEN6          {0.1f, .3f, .2f, 1.0f}
#define GREEN7          {.49f, 0.60f, 0.45f, 1.0f}
#define GREEN8          {0.0f, 1.0f, .1f, 1.0f}
#define GREEN9          {.15f, 0.6f, .15f, 1.0f}
#define GREEN10         {.13f, 1.0f, .13f, 1.0f}

#define BLUE1           {.50f, .55f, .90f, 1.0f}
#define BLUE2           {0.1f, .2f, .3f, 1.0f}
#define BLUE3           {0.1f, .1f, .4f, 1.0f}
#define BLUE4           {0.15f, .2f, .89f, 1.0f}
#define BLUE5           {0.15f, .2f, .65f, 1.0f}

#define CYAN1           {0.1f, 1.0f, 1.0f, 1.0f}

#define BLACK1          {0.0f, 0.0f, 0.0f, 1.0f}
#define BLACK2          {0.2f, .2f, .1f, 1.0f}
#define BLACK3          {0.2f, .2f, .2f, 1.0f}
#define BLACK4          {0.11f, 0.11f, 0.11f, 1.0f}
#define BLACK5          {.25f, .25f, .25f, 1.0f}
#define BLACK6          {.13f, .13f, .13f, 1.0f}

#define RED1            {0.89f, .2f, .15f, 1.0f}
#define RED2            {0.65f, .2f, .15f, 1.0f}

#define ORANGE1         {0.8f, .4f, .05f, 1.0f}
#define ORANGE2         {1.0f, .75f, 0.5f, 1.0f}

#define YELLOW1         {.9f, .1f, .1f, 1.0f}
#define YELLOW2         {1.0f, 1.0f, 0.1f, 1.0f}

#define PINK1           {1.0f, 0.5f, 1.0f, 1.0f}

#define BROWN1          {.47f, 0.30f, 0.25f, 1.0f}

const struct poulpe_theme poulpe_theme_light = {
    .airline_backgound = GREY1,
    .backgound = WHITE1,
    .comment = GREEN1,
    .cursor_insert = WHITE2,
    .cursor_line_background = GREY2,
    .cursor_normal = BLUE1,
    .dark = BLACK1,
    .error = RED1,
    .flash_color = ORANGE1,
    .hidden_text = YELLOW1,
    .identifier = BLACK2,
    .info = GREEN2,
    .keyword = BLUE2,
    .light = WHITE1,
    .line_number = GREEN4,
    .line_number_active = GREEN3,
    .line_number_background = WHITE3,
    .mode = GREEN5,
    .normal = BLACK1,
    .number = GREEN6,
    .parenthesis = BLACK1,
    .string = BLUE3,
    .tab_active = GREY3,
    .tab_border = GREY3,
    .tab_inactive = GREY4,
    .text = BLACK1,
    .text_dim = GREY3,
    .visual_select_background = GREEN7,
    .warning = BLUE4,
    .whitespace = BLACK3,
    .widger_border = GREY5,
    .widget_active = GREY3,
    .widget_background = GREY1,
    .widget_inactive = GREY4,
};

const struct poulpe_theme poulpe_theme_dark = {
    .airline_backgound = BLACK3,
    .backgound = BLACK4,
    .comment = GREEN8,
    .cursor_insert = WHITE2,
    .cursor_line_background = BLACK5,
    .cursor_normal = BLUE1,
    .dark = BLACK1,
    .error = RED2,
    .flash_color = ORANGE1,
    .hidden_text = YELLOW1,
    .identifier = ORANGE2,
    .info = GREEN9,
    .keyword = CYAN1,
    .light = WHITE1,
    .line_number = GREEN7,
    .line_number_active = GREEN10,
    .line_number_background = BLACK4,
    .mode = GREEN5,
    .normal = WHITE1,
    .number = YELLOW2,
    .parenthesis = WHITE1,
    .string = PINK1,
    .tab_active = GREY6,
    .tab_border = GREY3,
    .tab_inactive = GREY4,
    .text = WHITE1,
    .text_dim = GREY7,
    .visual_select_background = BROWN1,
    .warning = BLUE5,
    .whitespace = GREY8,
    .widger_border = GREY5,
    .widget_active = GREY6,
    .widget_background = BLACK3,
    .widget_inactive = GREY4,
};