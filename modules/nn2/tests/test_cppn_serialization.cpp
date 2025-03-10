#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "cppn.hpp"
#include <Eigen/Core>

std::mt19937 nn2::rgen_t::gen;

std::vector<Eigen::MatrixXd> generate_image(nn2::default_cppn_t& cppn){
    cppn.init();

    std::vector<Eigen::MatrixXd> mats = {Eigen::MatrixXd::Zero(200,200),Eigen::MatrixXd::Zero(200,200),Eigen::MatrixXd::Zero(200,200)};

    //cv::Mat image(200,200,CV_8UC3);
    std::vector<double> outs;
    for(int i = 0; i < 200; i++){
        for(int j = 0; j < 200; j++){
            float u = static_cast<float>(i)/100. - 1;
            float v = static_cast<float>(j)/100. - 1;
            cppn.step({u,v});
            outs = cppn.outf();

            mats[0](i,j) = outs[0];
            mats[1](i,j) = outs[1];
            mats[2](i,j) = outs[2];


     //       image.at<uchar>(i,j,0) = 128*outs[0]+127;
            //image.at<uchar>(i,j,1) = 128*outs[1]+127;
//            image.at<uchar>(i,j,2) = 128*outs[2]+127;
        }
    }

return mats;

//    cv::imshow("Random CPPN Generated Image",image);
//    std::cout << "press esc to close" << std::endl;
//#define ESC 27
//    int k = -1;
//    while (k != ESC) {
//        k = cv::waitKey(0);
//    }
//#undef ESC
}

int main(int argc,char** argv){

    nn2::rgen_t::gen.seed(time(0));


    nn2::default_cppn_t cppn(2,3);

    std::cout << "Original" << std::endl;

    cppn.random();

    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch << cppn;

    auto mats_1 = generate_image(cppn);


    nn2::default_cppn_t cppn2;

    boost::archive::text_iarchive iarch(sstream);

    iarch >> cppn2;

    std::cout << "After serialization and deseriallization" << std::endl;

    auto mats_2 = generate_image(cppn2);

    bool equal = (mats_1[0] == mats_2[0] && mats_1[1] == mats_2[1] && mats_1[2] == mats_2[2]);

    std::cout << "mats_1 == mats_2 : " << equal << std::endl;



    return 0;
}
