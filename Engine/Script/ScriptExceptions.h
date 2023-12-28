#pragma once

#include <exception>

class exception_script_entity_name_invalid
    : public std::exception
{
public:
    const char* what() const throw() override { return "Script Exception! Invalid entity name/id parameter. Entity: "; }
};

class exception_script_entity_component_invalid
    : public std::exception
{
public:
    const char* what() const throw() override { return "Script Exception! Entity does not contain specified component. Entity: "; }
};

extern exception_script_entity_name_invalid ex_ent_invalid_name;
extern exception_script_entity_component_invalid ex_ent_invalid_comp;
