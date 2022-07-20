numberOfOrgans = len(organs_blueprint_list);
echo("Manual version of skeleton_maker");
    
module add_clip(skeleton_id, organ_type,x,y,z,rotx,roty,rotz){
    
    
    if (skeleton_id==0){ // check this organ should be attached to the root skeleton part. Later, we need to deal with other skeleton parts as well, but for now they are ignored
        translate([x, y, z]) rotate([rotx,0,0]) rotate([0,roty,0]) rotate([0,0,rotz])
        if (organ_type==0) import("head_slot_manual.stl"); else import("slide_clip.stl"); // manual version!
    }
    
}

module negative_at_clip_location(skeleton_id, organ_type,x,y,z ,rotx,roty,rotz){
    translate([x,y,z]) rotate([rotx,0,0]) rotate([0,roty,0]) rotate([0,0,rotz]) import("clip_negative.stl");
}


union(){
    difference() { // next intersection minus space above Head
    intersection() { // printer volume, voxel structure, and organ clips (not head slot)
        // the printer volume: (inlcuding the bottom layer, since this is the version that will be manually printed)
        translate([0,0,125]){cube([280,280,250],center=true);}
        union(){ // add together the voxel structure and clip stl files
            difference(){ //  import voxel structure but remove sphere where each clip will be
                // import the main skeleton voxel structure.
                    // Need to scale up because it is in meters (not millimeters)
                    // translate needed if the export from morphogenesis code doesn't put the centre of the robot in x,y at [0,0], and the bottom surface to be printed at z=0.
                    translate([-118.5,-118.5,-9.822656400501728])scale([1000,1000,1000]){ import("inputMesh.stl");}
                   
                    
                    for (i=[1:numberOfOrgans-1]){ // need a negative clip shape at each organ location (excluding head)
                        negative_at_clip_location(organs_blueprint_list[i][0], organs_blueprint_list[i][1], organs_blueprint_list[i][2]*1000, organs_blueprint_list[i][3]*1000, organs_blueprint_list[i][4]*1000, organs_blueprint_list[i][5]*57.2958, organs_blueprint_list[i][6]*57.2958, organs_blueprint_list[i][7]*57.2958 );
                        }
                        } // end of difference between [voxels] and [clip_negatives]

                    // loop though every organ (except Head) and add the clip for it
                    for (i=[1:numberOfOrgans-1]){
                        // get values from blueprint list, converting to millimeters and degrees:
                        add_clip( organs_blueprint_list[i][0], organs_blueprint_list[i][1], organs_blueprint_list[i][2]*1000, organs_blueprint_list[i][3]*1000, organs_blueprint_list[i][4]*1000, organs_blueprint_list[i][5]*57.2958, organs_blueprint_list[i][6]*57.2958, organs_blueprint_list[i][7]*57.2958);
                    } // end of organ clip adding loop
        
                } //end of union which adds the clips to everything else
    } // end of printer volume intersection
    
    // space above Head to be subtracted
    cube([100,100,9999],center=true);
}

difference(){ //  add Head slot but remove sphere where each clip will be
    add_clip( organs_blueprint_list[0][0], organs_blueprint_list[0][1], organs_blueprint_list[0][2]*1000, organs_blueprint_list[0][3]*1000, organs_blueprint_list[0][4]*1000, organs_blueprint_list[0][5]*57.2958, organs_blueprint_list[0][6]*57.2958, organs_blueprint_list[0][7]*57.2958);
    
    for (i=[1:numberOfOrgans-1]){ // need a negative clip shape at each organ location (excluding head)
                        negative_at_clip_location(organs_blueprint_list[i][0], organs_blueprint_list[i][1], organs_blueprint_list[i][2]*1000, organs_blueprint_list[i][3]*1000, organs_blueprint_list[i][4]*1000, organs_blueprint_list[i][5]*57.2958, organs_blueprint_list[i][6]*57.2958, organs_blueprint_list[i][7]*57.2958 );
        } // end of for loop
    } // end of difference between [head slot] and [clip_negatives]
}