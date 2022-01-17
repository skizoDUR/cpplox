#ifndef ENV_PTR_HPP
#define ENV_PTR_HPP
#include <memory>
class environment;
using env_ptr = std::shared_ptr<environment>;
#endif
