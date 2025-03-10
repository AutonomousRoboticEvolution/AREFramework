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

    auto neuron_a1 = oscillator.add_neuron("A1");
    auto neuron_b1 = oscillator.add_neuron("B1");
    oscillator.create_oscillator_connection(neuron_a1, neuron_b1, 0.5);
    auto output_servo1 = oscillator.get_output(0);
    oscillator.add_connection(neuron_b1, output_servo1, 1.0);

    auto neuron_a2 = oscillator.add_neuron("A2");
    auto neuron_b2 = oscillator.add_neuron("B2");
    oscillator.create_oscillator_connection(neuron_a2, neuron_b2, 0.3);
    auto output_servo2 = oscillator.get_output(1);
    oscillator.add_connection(neuron_b2, output_servo2, 1.0);

    oscillator.add_connection(neuron_a1, neuron_a2,  0.2);
    oscillator.add_connection(neuron_a2, neuron_a1, -0.2);

    oscillator.init();

    // Setting the initial values of the oscillators after the init
    // otherwise the are set to zero and do not oscillate.
    auto &n_a1 = oscillator.get_neuron_by_vertex(neuron_a1);
    auto &n_b1 = oscillator.get_neuron_by_vertex(neuron_b1);
    n_a1.set_current_output(-0.707);
    n_b1.set_current_output(0.707);
    n_a1.set_next_output(-0.707);
    n_b1.set_next_output(0.707);

    auto &n_a2 = oscillator.get_neuron_by_vertex(neuron_a2);
    auto &n_b2 = oscillator.get_neuron_by_vertex(neuron_b2);
    n_a2.set_current_output(0.707);
    n_b2.set_current_output(-0.707);
    n_a2.set_next_output(0.707);
    n_b2.set_next_output(-0.707);

    // Activate NN
#define N_ITER 15000
    std::vector<float> outputs_a1;
    std::vector<float> outputs_a2;
    std::vector<float> outputs_b1;
    std::vector<float> outputs_b2;
    outputs_a1.reserve(N_ITER);
    outputs_a2.reserve(N_ITER);
    outputs_b1.reserve(N_ITER);
    outputs_b2.reserve(N_ITER);

    for (int i=0; i<N_ITER; i++) {
        oscillator.differential_step({}, 0.1);
//        std::cout << "Output, " << oscillator.get_neuron_output(0) << ", " << oscillator.get_neuron_output(1) << std::endl;
//        std::cout << "Oscill " << n_a.get_current_output() << " : " << n_b.get_current_output() << std::endl;

        outputs_b1.push_back(oscillator.get_neuron_output(0) * 100);
        outputs_b2.push_back(oscillator.get_neuron_output(1) * 100);

        outputs_a1.push_back(n_a1.get_current_output() * 100);
//        outputs_b1.push_back(n_b1.get_current_output() * 100);
        outputs_a2.push_back(n_a2.get_current_output() * 100);
//        outputs_b2.push_back(n_b2.get_current_output() * 100);
    }

    // Plot output over time
    std::shared_ptr<cv::Mat> image = plotGraph<float>(outputs_a1, {0,600}, cv::Scalar(0, 255, 0));
    plotGraph<float>(outputs_b1, {0,600}, cv::Scalar(50, 125, 50), image);
    plotGraph<float>(outputs_a2, {0,600}, cv::Scalar(0, 0, 255), image);
    plotGraph<float>(outputs_b2, {0,600}, cv::Scalar(50, 50, 125), image);

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
        cv::imwrite("test_cpg2_output.png", *image);
    }

    return 0;

#undef N_ITER
}
