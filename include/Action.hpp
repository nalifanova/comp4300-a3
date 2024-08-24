#ifndef ACTION_HPP
#define ACTION_HPP

#include <iostream>

// Action Types: Start and End

class Action
{
public:
    Action() = default;
    Action(std::string  name, std::string  type);
    ~Action() = default;

    [[nodiscard]] const std::string& name() const;
    [[nodiscard]] const std::string& type() const;

private:
    std::string m_name;
    std::string m_type;
};

#endif //ACTION_HPP
