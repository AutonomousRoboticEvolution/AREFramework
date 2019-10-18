#ifndef EXP_PLUGIN_LOADER_HPP
#define EXP_PLUGIN_LOADER_HPP

#include <iostream>
#include <string>
#include <dlibxx.hxx>

template <typename fct_type>
int load_exp_plugin(std::function<fct_type> fct, const std::string& plugin_name, const std::string& fct_name){
    dlibxx::handle libhandler;
    libhandler.resolve_policy(dlibxx::resolve::lazy);
    libhandler.set_options(dlibxx::options::global | dlibxx::options::no_delete);
    libhandler.load(plugin_name);
    if(!libhandler.loaded()){
        std::cerr << libhandler.error() << std::endl;
        return 0;
    }
    auto fct_sym = libhandler.lookup<fct_type>(fct_name);
    if(!fct_sym)
    {
        std::cerr << "Symbol" << fct_name << " not found" << std::endl;
        return 0;
    }

    fct = fct_sym.get();

    libhandler.close();

    return 1;
}

#endif //EXP_PLUGIN_LOADER_HPP
