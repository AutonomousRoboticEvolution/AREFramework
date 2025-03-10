#include <iostream>
#include <random>
#include <memory>
#include <opencv2/opencv.hpp>
#include "neuron.hpp"
#include "af.hpp"
#include "pf.hpp"
#include "nn.hpp"

typedef nn2::PfWSum<> pf_t;
typedef nn2::AfDirect<> af_t;
typedef nn2::Neuron<pf_t,af_t> neuron_t;
typedef nn2::Connection<> connection_t;

template <typename T>
std::shared_ptr<cv::Mat> plotGraph(std::vector<T>& vals,
                                   std::pair<T, T> YRange,
                                   cv::Scalar color = cv::Scalar(255, 0, 0),
                                   std::shared_ptr<cv::Mat> image = nullptr)
{
    auto it = minmax_element(vals.begin(), vals.end());
    float scale = 1./ceil(*it.second - *it.first);
    float bias = *it.first;
    int rows = YRange.second - YRange.first + 1;
    if (image == nullptr) {
        image = std::make_shared<cv::Mat>(cv::Mat::zeros( rows, vals.size(), CV_8UC3 ));
        image->setTo(0);
    }
    for (int i = 0; i < (int)vals.size()-1; i++)
    {
        cv::line(*image,
                 cv::Point(i, rows - 1 - (vals[i] - bias)*scale*YRange.second),
                 cv::Point(i+1, rows - 1 - (vals[i+1] - bias)*scale*YRange.second),
                 color,
                 1);
    }

    return image;
}


int main(int argc, char *argv[])
{
    bool gui = true;
    for (int i=0; i<argc;i++) {
        std::cout << "argument " << argv[i] << std::endl;
    }
    if (argc == 2 && std::strcmp(argv[1], "nogui") == 0) {
        gui = false;
    }
    // Create oscillator couple NN
    nn2::NN<neuron_t, connection_t> oscillator;
    oscillator.set_nb_inputs(0);
    oscillator.set_nb_outputs(2);
    auto neuron_a = oscillator.add_neuron("A");
    auto neuron_b = oscillator.add_neuron("B");
    auto output_a = oscillator.get_output(0);
    auto output_b = oscillator.get_output(1);
    oscillator.create_oscillator_connection(neuron_a, neuron_b, 0.5);
    oscillator.add_connection(neuron_a, output_a, 1.0);
    oscillator.add_connection(neuron_b, output_b, 1.0);
    oscillator.init();

    // Setting the initial values of the oscillators after the init
    // otherwise the are set to zero and do not oscillate.
    auto &n_a = oscillator.get_neuron_by_vertex(neuron_a);
    auto &n_b = oscillator.get_neuron_by_vertex(neuron_b);
    n_a.set_current_output(0.707);
    n_b.set_current_output(-0.707);
    n_a.set_next_output(0.707);
    n_b.set_next_output(-0.707);

    // Activate NN
#define N_ITER 3000
    std::vector<float> outputs_a;
    std::vector<float> outputs_b;
    outputs_a.reserve(N_ITER);
    outputs_b.reserve(N_ITER);

    for (int i=0; i<N_ITER; i++) {
        oscillator.differential_step({}, 0.1);
        //std::cout << "Output, " << oscillator.get_neuron_output(0) << ", " << oscillator.get_neuron_output(1) << std::endl;
        //std::cout << "Oscill " << n_a.get_current_output() << " : " << n_b.get_current_output() << std::endl;
        outputs_a.push_back(n_a.get_current_output() * 100);
        outputs_b.push_back(n_b.get_current_output() * 100);
    }

    // Plot output over time
    std::shared_ptr<cv::Mat> image = plotGraph<float>(outputs_a, {0,600});
    plotGraph<float>(outputs_b, {0,600}, cv::Scalar(0, 255, 0), image);

    // horizontal zero line
    cv::line(*image,
             cv::Point(0, 300),
             cv::Point(N_ITER-1, 300),
             cv::Scalar(125,125,125),
             1);

    if (gui) {
        cv::imshow("Ouputs", *image);

        std::cout << "press esc to close" << std::endl;
#define ESC 27
        int k = -1;
        while (k != ESC) {
            k = cv::waitKey(0);
        }
#undef ESC
    } else {
        cv::imwrite("test_cpg_output.png", *image);
    }

    return 0;

#undef N_ITER
}
