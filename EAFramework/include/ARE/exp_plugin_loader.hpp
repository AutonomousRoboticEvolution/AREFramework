#ifndef EXP_PLUGIN_LOADER_HPP
#define EXP_PLUGIN_LOADER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <dlibxx.hxx>
#include <unordered_map>

inline std::unique_ptr<dlibxx::handle> &load_plugin(const std::string &plugin_name)
{
    static std::unordered_map<std::string, std::unique_ptr<dlibxx::handle>> loaded_plugins;
    auto find_library = loaded_plugins.find(plugin_name);
    if (find_library != loaded_plugins.end()) {
        return find_library->second;
    }

    std::unique_ptr<dlibxx::handle> libhandler = std::make_unique<dlibxx::handle>();
    libhandler->resolve_policy(dlibxx::resolve::lazy);
    libhandler->set_options(dlibxx::options::global | dlibxx::options::no_delete);
    libhandler->load(plugin_name);

    if(!libhandler->loaded()){
        std::ostringstream error_message;
        error_message << "Unable to load plugin : " << libhandler->error();
        throw std::runtime_error(error_message.str());
    }

    auto insertion = loaded_plugins.insert(std::make_pair(plugin_name, std::move(libhandler)));

    // True if the pair was actually inserted
//    assert(insertion.second);

    return insertion.first->second;
}

template <typename fct_type>
int load_fct_exp_plugin(std::function<fct_type>& fct, std::unique_ptr<dlibxx::handle> &libhandler, const std::string& fct_name) {
    try {
//        std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(plugin_name);

        auto fct_sym = libhandler->lookup<fct_type>(fct_name);
        if(!fct_sym)
        {
            std::cerr << "Function under symbol " << fct_name << " not found" << std::endl;
            return 0;
        }

        fct = fct_sym.get();

//        libhandler->close();

    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }

    return 1;
}

template <class class_type>
int load_class_exp_plugin(std::shared_ptr<class_type>& class_, const std::string& plugin_name, const std::string& class_name){
    try {
        std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(plugin_name);

        class_ = libhandler->create<class_type>(class_name);

        if(class_.get() == nullptr)
        {
            std::cerr << "Class under symbol " << class_name << " not found" << std::endl;
            return 0;
        }

        //libhandler.close();

    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }

    return 1;
}

#endif //EXP_PLUGIN_LOADER_HPP
