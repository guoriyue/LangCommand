LangCommand
====

![Demo Video](output.gif)


LangCommand is a local inference command-line tool powered by llama.cpp that transforms natural language descriptions into executable shell commands.

* Natural Language Command Generation: Simply describe the command you need in plain language, and LangCommand will generate the appropriate shell command for you.
* Customizable Models: Choose from a variety of preconfigured models or provide your own for custom workflows.
* Multiple Modes:
    * loop mode: Continues to choose and execute commands.
    * exit mode: Executes a single command and exits.
* Command Explanation: Optionally provides an explanation of the generated command.
* Tracing: Enable model tracing to debug and track the execution process.


Install
-------

To build LangCommand from source, use the following command:

    make lac

Usage
-----

To use LangCommand, provide a natural language description:

    lac "your prompt"

You can run `lac` without arguments to explore options:

```bash
--------------------------------- LangCommand params ----------------------------------
-h,    --help, --usage                  Print LangCommand usage
--setup                                 Set up your LangCommand model: choose or customize
--show-args                             Show arguments you saved
--no-explanation                        Disable command explanation
--mode {loop,exit}                      Select the mode of operation.
                                                - loop: Continues to choose and execute commands indefinitely.
                                                - exit: Executes a single command and then stops the program.
--model-help, --model-usage             Print LangCommand default model arguments
--trace                                 Enable tracing for the execution of the default model
```

Supported Models
----------------

LangCommand comes with support for the [following preconfigured models](https://huggingface.co/MingfeiGuo/LangCommand/tree/main):

* Qwen2.5-Coder-7B
* Qwen2.5-7B
* Llama-3.2-3B
* Llama-3.2-1B
* Codellama-13B

You can also provide your own model with custom system prompts.


Troubleshooting
----------------

```bash
ggml_metal_graph_compute: command buffer 1 failed with status 5
error: Insufficient Memory (00000008:kIOGPUCommandBufferCallbackErrorOutOfMemory)
```

This typically occurs when the system does not have enough memory to load and run the selected model. To resolve, consider using a smaller model.


Community & Contact
----------------

Connect to discuss LangCommand, share feedback, and get support from other users:

[Join the LangCommand Discord group](https://discord.gg/zG4x6NuT5q)

For any questions or inquiries, feel free to reach out via email:

[Email me](mailto:mingfeiguoo@gmail.com)
