#include "cactus_ffi.h"
#include <unordered_map>
#include <string>
#include <iostream>

struct NCPContext {
    std::unordered_map<std::string, cactus_model_t> models;
};

static NCPContext ctx;

std::string handle_request(const std::string& func, const std::vector<std::string>& args) {
    if (func == "init") {
        if (args.size() < 3) return "error: init requires model_path, corpus_dir, cache";

        const char* model_path = args[0].c_str();
        const char* corpus_dir = args[1].c_str();
        bool cache = (args[2] == "true");

        cactus_model_t model = cactus_init(model_path, corpus_dir, cache);

        if (!model) {
            return std::string("error: ") + cactus_get_last_error();
        }

        std::string model_id = "model_" + std::to_string(ctx.models.size());
        ctx.models[model_id] = model;

        return "ok: " + model_id;
    }

    else if (func == "destroy") {
        std::string model_id = args[0];

        if (ctx.models.count(model_id)) {
            cactus_destroy(ctx.models[model_id]);
            ctx.models.erase(model_id);
            return "ok";
        }
        return "error: model not found";
    }

    else if (func == "reset") {
        std::string model_id = args[0];

        if (ctx.models.count(model_id)) {
            cactus_reset(ctx.models[model_id]);
            return "ok";
        }
        return "error: model not found";
    }

    else if (func == "stop") {
        std::string model_id = args[0];

        if (ctx.models.count(model_id)) {
            cactus_stop(ctx.models[model_id]);
            return "ok";
        }
        return "error: model not found";
    }

    return "error: unknown function";
}

int main() {
    std::cout << handle_request("init", {"./model", "./corpus", "true"}) << std::endl;
}