#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

enum gui_element_type {
    GUI_ELEMENT_TYPE_IMAGE,
};

struct gui_element {
    enum gui_element_type type;
    void *data;
};

// Origin in format x_y
enum gui_element_origin {
    GUI_ELEMENT_ORIGIN_LEFT_TOP,
    GUI_ELEMENT_ORIGIN_CENTER_TOP,
    GUI_ELEMENT_ORIGIN_RIGHT_TOP,
    GUI_ELEMENT_ORIGIN_LEFT_CENTER,
    GUI_ELEMENT_ORIGIN_CENTER_CENTER,
    GUI_ELEMENT_ORIGIN_RIGHT_CENTER,
    GUI_ELEMENT_ORIGIN_LEFT_BOTTOM,
    GUI_ELEMENT_ORIGIN_CENTER_BOTTOM,
    GUI_ELEMENT_ORIGIN_RIGHT_BOTTOM,
};

// 0 is back 9 is front
// number for layer instead of enum
enum gui_element_layer {
    GUI_ELEMENT_LAYER_0,
    GUI_ELEMENT_LAYER_1,
    GUI_ELEMENT_LAYER_2,
    GUI_ELEMENT_LAYER_3,
    GUI_ELEMENT_LAYER_4,
    GUI_ELEMENT_LAYER_5,
    GUI_ELEMENT_LAYER_6,
    GUI_ELEMENT_LAYER_7,
    GUI_ELEMENT_LAYER_8,
    GUI_ELEMENT_LAYER_9,
    GUI_ELEMENT_LAYER_LAST
};

#endif
