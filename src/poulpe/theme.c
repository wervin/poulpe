#include "poulpe/theme.h"

#define BLACK           0.0, 0.0, 0.0, 1.0

#define BASE_990        0.06, 0.06, 0.06, 1.0
#define BASE_975        0.08, 0.08, 0.08, 1.0
#define BASE_950        0.11, 0.11, 0.10, 1.0
#define BASE_900        0.16, 0.15, 0.15, 1.0
#define BASE_850        0.20, 0.20, 0.19, 1.0
#define BASE_800        0.25, 0.24, 0.24, 1.0
#define BASE_700        0.34, 0.34, 0.33, 1.0
#define BASE_600        0.44, 0.43, 0.41, 1.0
#define BASE_500        0.53, 0.52, 0.50, 1.0
#define BASE_400        0.63, 0.62, 0.60, 1.0
#define BASE_300        0.72, 0.71, 0.67, 1.0
#define BASE_200        0.81, 0.80, 0.76, 1.0
#define BASE_150        0.85, 0.85, 0.81, 1.0
#define BASE_100        0.90, 0.89, 0.85, 1.0
#define BASE_50         0.95, 0.94, 0.90, 1.0
#define BASE_25         0.98, 0.97, 0.92, 1.0

#define PAPER           1.00, 0.99, 0.94, 1.0

#define RED_600         0.69, 0.19, 0.16, 1.0
#define RED_400         0.82, 0.30, 0.25, 1.0

#define ORANGE_600      0.74, 0.32, 0.08, 1.0
#define ORANGE_400      0.85, 0.44, 0.17, 1.0

#define YELLOW_600      0.68, 0.51, 0.00, 1.0
#define YELLOW_400      0.82, 0.64, 0.08, 1.0

#define GREEN_600       0.40, 0.50, 0.04, 1.0
#define GREEN_400       0.53, 0.60, 0.22, 1.0

#define TURQUOISE_900   0.00, 0.09, 0.08, 1.0
#define TURQUOISE_100   0.68, 0.87, 0.81, 1.0

#define CYAN_600        0.14, 0.51, 0.48, 1.0
#define CYAN_400        0.23, 0.66, 0.62, 1.0

#define BLUE_600        0.13, 0.37, 0.65, 1.0
#define BLUE_400        0.26, 0.52, 0.75, 1.0

#define PURPLE_600      0.37, 0.25, 0.62, 1.0
#define PURPLE_400      0.55, 0.49, 0.78, 1.0

#define MAGENTA_600     0.63, 0.18, 0.44, 1.0
#define MAGENTA_400     0.81, 0.36, 0.59, 1.0

const struct poulpe_theme poulpe_theme_dark = {
    .main_background = {BASE_990},
    .secondary_background = {BASE_950},
    .border = {BASE_900},
    .hovered_border = {BASE_850},
    .active_border = {BASE_800},
    .widget = {BASE_700},
    .hovered_widget = {BASE_600},
    .active_widget = {BASE_500},
    .faint_text = {BASE_800},
    .muted_text = {BASE_500},
    .primary_text = {BASE_200},
    .secondary_text = {BASE_100},
    .highlighted_text = {TURQUOISE_900},
    .dimmed_text = {BASE_975},
    .error_text = {RED_400},
    .warning_text = {ORANGE_400},
    .success_text = {GREEN_400},
    .link = {CYAN_400},
    .comment = {GREEN_400},
    .operator = {BASE_200},
    .import = {RED_400},
    .function = {ORANGE_400},
    .constant = {YELLOW_400},
    .keyword = {MAGENTA_400},
    .string = {CYAN_400},
    .variable = {BLUE_400},
    .number = {PURPLE_400},
    .feature = {MAGENTA_400}
};

const struct poulpe_theme poulpe_theme_light = {
    .main_background = {PAPER},
    .secondary_background = {BASE_50},
    .border = {BASE_100},
    .hovered_border = {BASE_150},
    .active_border = {BASE_200},
    .widget = {BASE_600},
    .hovered_widget = {BASE_500},
    .active_widget = {BASE_400},
    .faint_text = {BASE_200},
    .muted_text = {BASE_600},
    .primary_text = {BASE_990},
    .secondary_text = {BLACK},
    .highlighted_text = {TURQUOISE_100},
    .dimmed_text = {BASE_150},
    .error_text = {RED_600},
    .warning_text = {ORANGE_600},
    .success_text = {GREEN_600},
    .link = {CYAN_600},
    .comment = {GREEN_600},
    .operator= {BASE_990},
    .import = {RED_600},
    .function = {ORANGE_600},
    .constant = {YELLOW_600},
    .keyword = {MAGENTA_600},
    .string = {CYAN_600},
    .variable = {BLUE_600},
    .number = {PURPLE_600},
    .feature = {MAGENTA_600}
};