#include "image_processing/real_environment.hpp"

using namespace are;

RealEnvironment::RealEnvironment(): Environment(){
    current_position.resize(2);
    beacon_position.resize(2);
    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#target",new settings::Sequence<double>({0.,0.,0.12}));
}

void RealEnvironment::init(){

     std::string pipe = settings::getParameter<settings::String>(parameters,"#cameraPipe").value;
     bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
     if(verbose)
         std::cout << "camera pipe: " << pipe << std::endl;
     usingIPCamera = pipe.substr(0,4).compare("http") == 0; // if the pipe starts "http", we're using an IP camera. This is important later because the buffer needs flushing on each update
     if(verbose) std::cout<<"usingIPCamera: "<<usingIPCamera<<std::endl;
     video_capture = cv::VideoCapture(pipe);

     grid_zone = Eigen::MatrixXi::Zero(8,8);


     std::vector<int> colour_bnd = settings::getParameter<settings::Sequence<int>>(parameters,"#colourBoundaries").value;
     colour_range.first = cv::Scalar(colour_bnd[0],colour_bnd[2],colour_bnd[4]);
     colour_range.second = cv::Scalar(colour_bnd[1],colour_bnd[3],colour_bnd[5]);


     target_position = settings::getParameter<settings::Sequence<double>>(parameters,"#target").value;

}

std::vector<double> RealEnvironment::fitnessFunction(const Individual::Ptr &ind){
    int env_type = are::settings::getParameter<are::settings::Integer>(parameters,"#envType").value;

    if(env_type == 0)
        return fit_targeted_locomotion();
    else if(env_type == 1)
        return fit_exploration();
    else if(env_type == 2)
        return fit_foraging();

    std::cerr << "Unknown type of environment type : " << env_type << std::endl;
    return {0};

}

std::vector<double> RealEnvironment::fit_targeted_locomotion(){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
            (a[1] - b[1])*(a[1] - b[1]));
    };
    std::vector<double> d(1);
    d[0] = 1 - distance(current_position,target_position)/max_dist;
    final_position = current_position;
    return d;
}

std::vector<double> RealEnvironment::fit_exploration(){
    return {static_cast<double>(grid_zone.sum())/64.f};
}

std::vector<double> RealEnvironment::fit_foraging(){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                (a[1] - b[1])*(a[1] - b[1]) +
                (a[2] - b[2])*(a[2] - b[2]));
    };
    std::vector<double> d(1);
    // Distance between beacon and target
    //d[0] = 1 - distance(final_position,target_position)/max_dist;
    d[0] = 1 - ((distance(current_position,target_position)/max_dist) *
                + (distance(beacon_position,target_position)/max_dist)) * 0.5;

    for(double& f : d)
        if(std::isnan(f) || std::isinf(f) || f < 0)
            f = 0;
        else if(f > 1) f = 1;

    return d;
}

void RealEnvironment::update_info(double time){
    std::vector<int> crop_rect = settings::getParameter<settings::Sequence<int>>(parameters,"#cropRectangle").value;
    cv::Mat image;
    if (usingIPCamera==true){
        // this is a rather hacky method to flush the buffer created when using an IP camera.
        // We time how long it takes to get a frame - if it was very quick, it probably came from the buffer, so we discard it and get another frame.
        // This only works because we are requesting frames at a slower rate than they are created.
        // inspired by answers.opencv.org/question/29957/highguivideocapture-buffer-introducing-lag/?answer=38217#post-id-38217/
        int framesWithDelayCount = 0;
        while (framesWithDelayCount < 1)
        {
            auto time_before = std::chrono::system_clock::now();
            video_capture.grab();
            auto time_after = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedTime = time_after-time_before;
            if(elapsedTime > std::chrono::milliseconds(3)) framesWithDelayCount++;
        }
        video_capture.retrieve(image);
    }else{
        video_capture.read(image);
    }

    if (image.empty()) {
        std::cerr << "ERROR! the frame is empty\n";
        return;
    }

    cv::KeyPoint key_pt(0,0,0);
    image_proc::blob_detection(image,colour_range.first,colour_range.second,crop_rect,key_pt);
    if(key_pt.pt.x == -999 || key_pt.pt.y == -999)
        return;
    image_proc::pixel_to_world_frame(key_pt,current_position,parameters);

    std::pair<int,int> indexes = real_coordinate_to_matrix_index(current_position);
    grid_zone(indexes.first,indexes.second) = 1;

    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(time >= interval*trajectory.size()){
        waypoint wp;
        wp.position[0] = current_position[0];
        wp.position[1] = current_position[1];
        wp.position[2] = 0;
        wp.orientation[0] = 0;
        wp.orientation[1] = 0;
        wp.orientation[2] = 0;

        trajectory.push_back(wp);
    }

    final_position = current_position;

    for(const auto &val : current_position)
        std::cout << val << ",";
}


std::pair<int,int> RealEnvironment::real_coordinate_to_matrix_index(const std::vector<double> &pos){
    std::pair<int,int> indexes;

    indexes.first = std::trunc(pos[0]/0.25 + 4);
    indexes.second = std::trunc(pos[1]/0.25 + 4);
    if(indexes.first == 8)
        indexes.first = 7;
    if(indexes.second == 8)
        indexes.second = 7;
    return indexes;
}
