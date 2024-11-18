#include "log.h"
#include "model_manager.h"
#include "config.h"
#include "file_manager.h"
#include "common.h"
#include "console.h"
#include "console_manager.h"

#include "json.hpp"
#include <sys/stat.h>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <string>

#include <fstream>
#include <string>

// #include <sys/stat.h>
// #include <iostream>
// #include <ncurses.h>
// #include <vector>
// #include <string>

// #ifdef _WIN32
//     #include <conio.h>  // Windows specific for _getch()
// #else
//     #include <termios.h>  // POSIX terminal control
//     #include <unistd.h>   // For STDIN_FILENO
// #endif
// #include <cstdio>
void model_manager::show_args() {
    std::string config_path = fs_get_cache_file(CONFIG_FILE);
    std::ifstream
        infile(config_path);
    if (!infile.is_open()) {
        // std::cout << "Error: Unable to open the config file." << std::endl;
        // print_error(("Unable to open the config file " + config_path).c_str());
        json_file_create(config_path);
        infile.open(config_path);
        // return;
    }
    nlohmann::json config_dict;
    infile >> config_dict;  // The >> operator automatically parses the JSON file content
    infile.close();  // Close the file
    for (auto it = config_dict.begin(); it != config_dict.end(); ++it) {
        std::cout << LOG_COL_AQUA << it.key() << LOG_COL_DEFAULT << ": " << it.value() << std::endl;
    }
    // std::string model_path = config_dict["model_path"];
    // std::cout << "Current model path: " << model_path << std::endl;
}

// void model_manager::print_models(const std::vector<std::string>& models, std::size_t current_choice) {
//     // Move the cursor up by the number of model lines to overwrite them
//     if (first_call_print_models) {
//         first_call_print_models = false;
//     } else {
//         for (std::size_t i = 0; i < models.size(); ++i) {
//             std::cout << "\033[F";
//         }
//     }
//     for (std::size_t i = 0; i < models.size(); ++i) {
//         if (i == current_choice) {
//             // Highlight the current choice (reverse video mode)
//             std::cout << "\033[7m" << models[i] << "\033[0m" << std::endl;
//         } else {
//             std::cout << models[i] << std::endl;
//         }
//     }
// }


bool model_manager::save_args(const std::string& args_key, const std::string& args_value) {
    // std::string config_path = file_manager::get_cache_directory(CMD_NAME) + "/" + CONFIG_FILE;
    std::string config_path = fs_get_cache_file(CONFIG_FILE);
    std::ifstream infile(config_path);
    
    if (!infile.is_open()) {
        // std::cout << "Error: Unable to open the config file." << std::endl;
        // print_error(("Unable to open the config file " + config_path).c_str());
        json_file_create(config_path);
        infile.open(config_path);
        // return false;
    }
    nlohmann::json config_dict;
    infile >> config_dict;  // The >> operator automatically parses the JSON file content
    infile.close();  // Close the file after reading
    config_dict[args_key] = args_value;
    std::ofstream outfile(config_path);
    if (outfile.is_open()) {
        outfile << config_dict.dump(4);  // Pretty print with 4 spaces
        outfile.close();
        return true;
    } else {
        // std::cout << "Error: Unable to open the config file." << std::endl;
        print_error(("Unable to open the config file " + config_path).c_str());
        return false;
    }
}

std::string model_manager::get_args(const std::string& args_key) {
    // std::string config_path = file_manager::get_cache_directory(CMD_NAME) + "/" + CONFIG_FILE;
    std::string config_path = fs_get_cache_file(CONFIG_FILE);
    std::ifstream infile(config_path);
    if (!infile.is_open()) {
        // std::cout << "Error: Unable to open the config file." << std::endl;
        // print_warning(("Unable to open the config file " + config_path).c_str());
        json_file_create(config_path);
        // infile.open(config_path);
        return "";
    }
    if (file_is_empty(config_path)) {
        return "";
    }
    nlohmann::json config_dict;
    infile >> config_dict;  // The >> operator automatically parses the JSON file content
    infile.close();  // Close the file after reading
    if (config_dict.find(args_key) == config_dict.end()) {
        return "";
    }
    return config_dict[args_key];
}

// std::size_t model_manager::choose_model() {
//     std::size_t choice = 0;
//     int key;
//     disable_echo();
//     // Print the initial model list
//     print_models(models_to_choose, choice);
//     while (true) {
//         key = console::getchar32();
//         if (key == 27) { // Escape sequence starts with 27 (ESC)
//             key = console::getchar32(); // Skip the '[' character
//             key = console::getchar32(); // Get the actual arrow key

//             switch (key) {
//                 case 'A': // Up arrow key
//                     if (choice > 0) {
//                         choice--;
//                     }
//                     break;
//                 case 'B': // Down arrow key
//                     if (choice < models_to_choose.size() - 1) {
//                         choice++;
//                     }
//                     break;
//             }
//         } else if (key == 10) { // Enter key
//             enable_echo();
//             return choice;
//         }

//         // Reprint the model list with the updated selection
//         print_models(models_to_choose, choice);
//     }
//     enable_echo();
// }



std::string model_manager::set_model() {
    int chosen = choose_from_vector(models_to_choose);
    // enable_echo();
    if (models_to_choose[chosen] == "custom"){
        std::string custom_model_path;
        // std::cout << "Please enter the .gguf path: \n";
        print_info("Please enter the file path (in .gguf format): ");
        // custom_model_path = edit_prefilled_input("");
        custom_model_path = get_input();
        custom_model_path = trim(custom_model_path);
        save_args("model", custom_model_path);
        std::string success_set_model = "Model set to " + custom_model_path;
        print_success(success_set_model.c_str());
        return custom_model_path;
    } else {
        std::string chosen_model = models_to_choose[chosen];
        std::string chosen_model_url = model_urls[chosen];
        size_t last_slash = chosen_model_url.find_last_of('/');

        // Get the substring after the last '/'
        std::string file_name = (last_slash != std::string::npos) ? chosen_model_url.substr(last_slash + 1) : chosen_model_url;
        
        std::string chosen_model_path = fs_get_cache_file(file_name);
        bool file_exists_ = file_exists(chosen_model_path);
        if (!file_exists_) {
            print_warning("model does not exist\n");
            bool download_model = download_file(chosen_model_url, chosen_model_path);
            if (!download_model) {
                print_error("Could not download the model.");
                return "";
            }
            save_args("model", chosen_model_path);
            return chosen_model_path;
        } else {
            // file exists
            print_info("There is already a file at this path. Do you want to overwrite it? (y/n) ");

            do {
                char choice = console::getchar32();
                if (choice == 'y') {
                    printf("\n");
                    bool download_model = download_file(chosen_model_url, chosen_model_path);
                    if (!download_model) {
                        print_error("Could not download the model.");
                        return "";
                    }
                    save_args("model", chosen_model_path);
                    // printf("Model set to: %s\n", chosen_model_path.c_str());
                    return chosen_model_path;
                } else if (choice == 'n') {
                    printf("\n");
                    save_args("model", chosen_model_path);
                    return chosen_model_path;
                }
            } while (true);
        }
    }
}