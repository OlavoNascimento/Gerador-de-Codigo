#pragma once

enum RegisterTypes {
    ARGUMENT = 'a',
    SAVED = 's',
    TEMP = 't',
};

struct Register {
    enum RegisterTypes type;
    int position;
};

struct Register identifier_get_register(char *identifier_id);

int find_empty_temporary_register();
