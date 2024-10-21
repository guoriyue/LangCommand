#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <string>
#include <vector>

class model_manager {
public:
    std::string prompt;
    std::vector<std::string> models_to_choose;
    std::vector<std::string> model_urls;
    bool first_call_print_models = true;

    // Constructor
    model_manager() {
        init_models();
    }

    // Function to initialize the models vector
    void init_models() {

        models_to_choose.push_back("Llama-3.2-3B-Instruct-Q8_0");
        model_urls.push_back("https://huggingface.co/MingfeiGuo/llmc/resolve/main/Llama-3.2-3B-Instruct-Q8_0.gguf");
        
        models_to_choose.push_back("Llama-3.2-1B-Instruct-Q8_0");
        model_urls.push_back("https://huggingface.co/MingfeiGuo/llmc/resolve/main/Llama-3.2-1B-Instruct-Q8_0.gguf");

        models_to_choose.push_back("qwen2.5-7b-instruct-q8_0");
        model_urls.push_back("https://huggingface.co/MingfeiGuo/llmc/resolve/main/qwen2.5-7b-instruct-q8_0.gguf");

        models_to_choose.push_back("codellama-13b.Q8_0");
        model_urls.push_back("https://huggingface.co/MingfeiGuo/llmc/resolve/main/codellama-13b.Q8_0.gguf");

        models_to_choose.push_back("custom");
    }

    std::string set_model();
    void show_args();

    bool save_args(const std::string& args_key, const std::string& args_value);
    std::string get_args(const std::string& args_key);
};

#endif // MODEL_MANAGER_H
