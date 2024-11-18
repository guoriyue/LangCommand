#include "log.h"
#include "output_parser.h"
#include "str_parser.h"
#include "console_manager.h"
#include <regex>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>


std::vector<std::string> extract_suggestions(const std::string& input) {
    // may need to move this to handle by llm again, but now we do manual processing
    // many manual work to fix the generated suggestions
    std::vector<std::string> suggestions;

    // the block before ### Instruction or ### Example
    size_t pos_instruction = input.find("### Instruction");
    size_t pos_example = input.find("### Example");
    std::string first_output = "";
    if (pos_instruction != std::string::npos) {
        first_output = input.substr(0, pos_instruction);
    } else if (pos_example != std::string::npos) {
        first_output = input.substr(0, pos_example);
    }
    
    std::string regex_str_example = R"(### Example.*?(?=### ))";
    std::string example = extract_str(input, regex_str_example);
    std::string regex_str_instruction = R"(### Instruction.*?(?=### ))";
    std::string instruction = extract_str(input, regex_str_instruction);

    std::string potential_suggestion = first_output + example + instruction;

    if (potential_suggestion == "") {
        potential_suggestion = input;
    }

    std::vector<std::string> blocks = extract_strs(potential_suggestion, R"(```bash([\s\S]*?)```)");

    if (blocks.size() == 0) {
        blocks = extract_strs(potential_suggestion, R"(```shell([\s\S]*?)```)"); 
    }
    if (blocks.size() == 0) {
        blocks = extract_strs(potential_suggestion, R"(```sh([\s\S]*?)```)"); 
    }
    if (blocks.size() == 0) {
        // blocks = extract_strs(potential_suggestion, R"(```([\s\S]*?)```)"); 
        std::string regex_str = R"(```(?![a-zA-Z]+)([\s\S]*?)```)";
    }

    for (const std::string& block : blocks) {
        std::string clean_bash_command = trim(block);
        if (!clean_bash_command.empty() && std::find(suggestions.begin(), suggestions.end(), clean_bash_command) == suggestions.end()) {
            suggestions.push_back(clean_bash_command);
            // if (clean_bash_command.length() && clean_bash_command[0] == '#') {
            //     continue;
            // }
            // if (clean_bash_command.find("\n") != std::string::npos) {
            //     std::vector<std::string> lines = split_str(clean_bash_command, '\n');
            //     printf("lines.size(): %lu\n", lines.size());
            //     if (lines.size() == 0) {
            //         continue;
            //     } else if (lines.size() == 1) {
            //         // clean_bash_command = lines[0];
            //         suggestions.push_back(lines[0]);
            //     } else {
            //         // clean_bash_command = lines[1];
            //         // if (lines[0].find("#") != std::string::npos && lines[1].find("#") == std::string::npos) {
            //         //     clean_bash_command = lines[1];
            //         // } else {
            //         //     clean_bash_command = lines[0];
            //         // }
            //         for (const std::string& line : lines) {
            //             if (line.find("#") != std::string::npos && line.length() && line[0] == '#') {
            //                 // comments
            //             } else {
            //                 // clean_bash_command = line;
            //                 suggestions.push_back(line);
            //                 // break;
            //             }
            //         }
            //     }
            // }
            
        }
    }
    return suggestions;
}


size_t check_early_stop(const std::string& output_buffer) {
    // if 2 Instruction or Example blocks are found, stop early
    if (get_num_delimiters(output_buffer, "###") >= 6) {
        return get_nth_delimiters(output_buffer, "###", 6);
    }
    if (get_num_delimiters(output_buffer, "### Instruction") >= 2) {
        return get_nth_delimiters(output_buffer, "### Instruction", 2);
    }
    if (get_num_delimiters(output_buffer, "### Example") >= 2) {
        return get_nth_delimiters(output_buffer, "### Example", 2);
    }
    // if has ### References
    if (output_buffer.find("### References") != std::string::npos) {
        return output_buffer.find("### References");
    }
    if (output_buffer.find("####") != std::string::npos) {
        return output_buffer.find("####");
    }
    if (get_num_delimiters(output_buffer, "**") >= 7) {
        return get_nth_delimiters(output_buffer, "**", 7);
    }
    // return false;
    return -1;
}


std::string colorize_text(const std::string& input) {
    std::ostringstream output;

    // Regular expressions for parsing sections
    std::regex header_regex(R"(###\s+([^\n]+))");
    std::regex explanation_regex(R"(- `(.*?)`:(.*))");

    // Split the input into lines for parsing
    std::istringstream lines(input);
    std::string line;
    bool in_code_block = false;
    while (std::getline(lines, line)) {
        std::smatch match;

        // Match headers
        if (std::regex_search(line, match, header_regex)) {
            output << LOG_COL_GREEN << "### " << match[1] << LOG_COL_DEFAULT << std::endl;
        }
        // Match explanation items
        else if (std::regex_search(line, match, explanation_regex)) {
            output << LOG_COL_WHITE << "- " << LOG_COL_AQUA << match[1] << LOG_COL_DEFAULT << ":" << LOG_COL_WHITE << match[2] << LOG_COL_DEFAULT << std::endl;
        }
        else {
            if (line.find("```") != std::string::npos) {
                in_code_block = !in_code_block;
                output << LOG_COL_YELLOW << line << LOG_COL_DEFAULT << std::endl;
            } else {
                if (in_code_block) {
                    output << LOG_COL_YELLOW << line << LOG_COL_DEFAULT << std::endl;
                } else {
                    output << line << std::endl;
                }
            }
        }
    }

    return output.str();
}