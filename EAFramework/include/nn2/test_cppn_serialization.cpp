#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "cppn.hpp"

std::mt19937 nn2::rgen_t::gen;

void generate_image(nn2::default_cppn_t& cppn){
    cppn.init();

    cv::Mat image(200,200,CV_8UC3);
    std::vector<double> outs;
    for(int i = 0; i < 200; i++){
        for(int j = 0; j < 200; j++){
            float u = static_cast<float>(i)/100. - 1;
            float v = static_cast<float>(j)/100. - 1;
            cppn.step({12*u,12*v});
            outs = cppn.outf();

            image.at<uchar>(i,j,0) = 128*outs[0]+127;
            image.at<uchar>(i,j,1) = 128*outs[1]+127;
            image.at<uchar>(i,j,2) = 128*outs[2]+127;
        }
    }
    cv::imshow("Random CPPN Generated Image",image);
    cv::waitKey(0);
}

int main(int argc,char** argv){

    nn2::rgen_t::gen.seed(time(0));


    nn2::default_cppn_t cppn(2,3);

    std::cout << "Original" << std::endl;

    cppn.random();

    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch << cppn;

    generate_image(cppn);


    nn2::default_cppn_t cppn2;

    boost::archive::text_iarchive iarch(sstream);

    iarch >> cppn2;

    std::cout << "After serialization and deseriallization" << std::endl;

    generate_image(cppn2);


    return 0;
}
