#include "meim_logging.hpp"

using namespace are;

void GenomeInfoLog::saveLog(EA::Ptr &ea)
{
	int pop_size = settings::getParameter<settings::Integer>(ea->get_parameters(),"#populationSize").value;
	if(static_cast<MEIM*>(ea.get())->get_parent_pool().size() < pop_size)
		return;
	
	for(const auto &genome:  static_cast<MEIM*>(ea.get())->get_parent_pool()){

	//- Log the morph genome
	std::stringstream morph_filepath;
	morph_filepath << Logging::log_folder << "/morph_genome_" << genome.morph_genome->id();
	if(boost::filesystem::exists(morph_filepath.str()))
		continue;
	std::ofstream mofstr(morph_filepath.str());
	boost::archive::text_oarchive oarch(mofstr);
	oarch << genome.morph_genome->get_cppn();
	mofstr.close();
	//-
	
	//- Log Descriptor
	std::ofstream mdofs;
	if(!openOLogFile(mdofs,"/morph_descriptors.csv"))
            return;
    	mdofs << genome.morph_genome->id() << ",";
    	Eigen::VectorXd morphDesc = genome.morph_genome->get_cart_desc().getCartDesc();
    	for(int j = 0; j < morphDesc.size(); j++){
	        mdofs << morphDesc(j) << ",";
	}
    	mdofs << std::endl;
   	mdofs.close();
	//-
	
	//- TO DO log controller parameters, trajectory and behavioural descriptor
	//  If there is a ctrl genome there is a trajectory and a behavioural descriptor
	//  Log Trajectory
	//  int i = 0;
	//  for(const auto &traj: genome.trajectories)
	//  {
	//  	if(traj.empty()){
	//      	i++;
	//              continue;
	//      }

	//                                                                                                                                   std::ofstream tofs(traj_filepath.str());
	//                                                                                                                                               for(const auto &wp: traj)
	//                                                                                                                                                               tofs << wp.to_string() << std::endl;
	//                                                                                                                                                                           tofs.close();
	//                                                                                                                                                                                       i++;
	// }
	        //Log descriptor
		//        std::ofstream dofs;
		//                if(!openOLogFile(dofs,"/behavioral_descriptors"))
		//                            return;
		//                                    dofs << genome.morph_genome.id() << std::endl;
		//                                            dofs << genome.behavioral_descriptor << std::endl;
		//                                                    dofs.close();
		//                                                    
        //- Log the fitness values and parents ids
	std::ofstream fitness_file_stream;
	if(!openOLogFile(fitness_file_stream))
		return;
	fitness_file_stream << genome.morph_genome->id() << ",";
	for(const int& id: genome.morph_genome->get_parents_ids())
		fitness_file_stream << id << ";";
	for(const double& obj: genome.objectives)
		fitness_file_stream << obj << ",";
	fitness_file_stream << std::endl;
	fitness_file_stream.close();
	//-	
	}
}
