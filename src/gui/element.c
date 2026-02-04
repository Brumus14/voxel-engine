#include "element.h"
#include "image.h"

void gui_element_destroy(struct gui_element *element) {
    switch (element->type) {
    case GUI_ELEMENT_TYPE_IMAGE:
        gui_image_destroy(element->data);
        break;
    }
}
