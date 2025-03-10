#ifndef TEST_CPPN_HPP
#define TEST_CPPN_HPP

struct params{
    struct cppn{
        static float _mutation_rate;
        static float _mutation_type;
        static bool _mutate_connections;
        static bool _mutate_neurons;
        static float _rate_mutate_conn;
        static float _rate_mutate_neur;
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
        static constexpr float min = -1;
        static constexpr float max = 1;
    };
};

#endif //TEST_CPPN_HPP
