#ifndef TEST_CPPN_HPP
#define TEST_CPPN_HPP

struct params{
    struct cppn{
        static bool _mutate_connections;
        static bool _mutate_neurons;
        static float _rate_add_neuron;
        static float _rate_del_neuron;
        static float _rate_add_conn;
        static float _rate_del_conn;
        static float _rate_change_conn;

        static size_t _min_nb_neurons;
        static size_t _max_nb_neurons;
        static size_t _min_nb_conns;
        static size_t _max_nb_conns;
    };
    struct evo_float{
        static float mutation_rate;
        static float cross_rate;
        static constexpr nn2::evo_float::mutation_t mutation_type = nn2::evo_float::polynomial;
        static constexpr nn2::evo_float::cross_over_t cross_over_type = nn2::evo_float::no_cross_over;
        static float eta_m;
        static float eta_c;
    };
};

#endif //TEST_CPPN_HPP
