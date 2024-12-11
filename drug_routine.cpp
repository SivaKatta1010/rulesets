#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include "drug_routine.h"
#include <utility>
#include <functional>
#include <string>

// Parse a rule line and return a rule-function pair
RuleFunctionPair parseRule(const std::string &ruleLine)
{
    std::stringstream ss(ruleLine);
    std::string token, paramName, op, drugName;
    float value, dosage;

    // Define the rule function lambda
    std::vector<std::tuple<std::string, std::string, float>> conditions;

    // Parse the conditions
    while (ss >> token && token != "=>")
    {
        if (token == "(parameter")
        {
            ss >> paramName >> op >> value;
            conditions.push_back({paramName, op, value});
        }
    }

    // Parse the drug name and dosage
    ss.ignore(std::numeric_limits<std::streamsize>::max(), '\"'); // ignore until "
    std::getline(ss, drugName, '\"');                             // get until "
    ss >> dosage;

    auto ruleFunction = [conditions](const std::vector<ParameterPair> &parameters)
    {
        return std::all_of(conditions.begin(), conditions.end(), [&parameters](const auto &condition)
                           {
            auto it = std::find_if(parameters.begin(), parameters.end(), [&condition](const auto& parameter) {
                if (parameter.first == std::get<0>(condition)) {
                    if (std::get<1>(condition) == ">=") {
                        return parameter.second >= std::get<2>(condition);
                    } else if (std::get<1>(condition) == "<") {
                        return parameter.second < std::get<2>(condition);
                    } else if (std::get<1>(condition) == "<=") {
                        return parameter.second <= std::get<2>(condition);
                    } else if (std::get<1>(condition) == ">") {
                        return parameter.second > std::get<2>(condition);
                    }
                }
                return false;
            });
            return it != parameters.end(); });
    };
    RuleFunctionPair rule(ruleFunction, std::pair<std::string, float>(drugName, dosage));
    return rule;
}

/**
 * @param parameters A vector of ParameterPair objects representing the parameters for drug suggestion.
 * @param rules A vector of RuleFunctionPair objects representing the rules for drug suggestion.
 * @return A pair of strings representing the suggested drug name and dosage.
 */
std::pair<std::string, float> suggestDrug(const std::vector<ParameterPair> &parameters, const std::vector<RuleFunctionPair> &rules)
{
    /**
     *  Sample Parameter pair -  std::vector<ParameterPair> parameters = {
        {"num_loc", 20},
        {"max_time", 5},
        {"tel_len", 8},
        {"mut_prob", 0.7},
        {"death_prob", 0.1}}
    */
    for (const auto &rule : rules)
    {
        // Access the rule function and the drug name and dosage
        const auto &ruleFunction = rule.first;
        const auto &drugName = rule.second.first;
        const auto &dosage = rule.second.second;

        // Use the rule function
        if (ruleFunction(parameters))
        {
            // The rule condition is met
            return std::make_pair(drugName, dosage);
        }
    }

    std::cerr << "ERROR: Some Error encountered when the rules are read" << std::endl;
    return;
}

std::vector<RuleFunctionPair> getRulesFromRulesFile(std::string fileName)
{
    // Check if fileName is empty
    if (fileName.empty())
    {
        std::cerr << "ERROR: Filename is empty." << std::endl;
        return {};
    }

    std::ifstream rulesFile(fileName);
    // Check if the file stream is in a good state
    if (!rulesFile.good())
    {
        std::cerr << "ERROR: Invalid filename or file could not be opened." << std::endl;
        return {};
    }

    // Read rules from file
    std::string line;
    std::string rule;
    std::vector<RuleFunctionPair> rules;

    // Parse rules from file
    while (std::getline(rulesFile, line))
    {
        if (line.find("(defrule") != std::string::npos)
        {
            // Start of a new rule
            rule = line;
        }
        else if (line.find("(assert") != std::string::npos)
        {
            // End of a rule
            rule += line;
            rules.push_back(parseRule(rule));
            rule.clear();
        }
        else if (!rule.empty())
        {
            // Middle of a rule
            rule += line;
        }
    }

    rulesFile.close();

    return rules;
}
