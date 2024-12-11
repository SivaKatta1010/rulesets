#include <utility>
#include <functional>
#include <string>

// Define a type alias for parameter pairs
using ParameterPair = std::pair<std::string, float>;

// Define a type alias for rule-function pairs
using RuleFunctionPair = std::pair<std::function<bool(const std::vector<ParameterPair>&)>, std::pair<std::string, float>>;

// Function to Parse Rules and return the rule-function pair
RuleFunctionPair parseRule(const std::string& ruleLine);

// Function to suggest a drug based on parameters and rules
std::pair<std::string, float> suggestDrug(const std::vector<ParameterPair>& parameters, const std::vector<RuleFunctionPair>& rules);

// Function to return a rules vector from a file
std::vector<RuleFunctionPair> getRulesFromFile(const std::string& filename);
