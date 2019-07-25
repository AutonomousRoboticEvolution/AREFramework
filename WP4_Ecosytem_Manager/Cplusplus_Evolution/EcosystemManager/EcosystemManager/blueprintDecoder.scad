
// Blueprint data
mr = [
    COORDINATES
];
// Number of organs
numberOfOrgans = len(mr);
// Constants
// These constants make the rotation of the clip match the rotation of the wheel organ in VREP. Ideally these values should be zero. Check with MA or MH if the orientation can be changed from within VREP.
CLIPROTX=0;
CLIPROTY=180;
CLIPROTZ=90;
difference(){
    union(){
        // Main loop
        for (i=[0:numberOfOrgans-1]){
            // Brain slot
            if(mr[i][1]==13){
                // Brain slot should be the last element to be created. 
            }
            // Connector
            if(mr[i][1]==14){
               translate([mr[i][2],mr[i][3],mr[i][4]])
               rotate([mr[i][5]*180/3.1415 + CLIPROTX, 0.0, 0.0])
               rotate([0.0, mr[i][6]*180/3.1415 + CLIPROTY, 0.0])
               rotate([0.0, 0.0, mr[i][7]*180/3.1415+CLIPROTZ])
               scale([0.001, 0.001, 0.001]) import("maleConnector.stl", center=true);
            }
            // Piece of bone
            if(mr[i][1]==17){
                translate([mr[i][2],mr[i][3],mr[i][4]]) rotate([mr[i][5]*180/3.1415,mr[i][6]*180/3.1415,mr[i][7]*180/3.1415]) cube([0.02, 0.02, 0.02], center=true);
            }
        }
    }
    union(){
        // Removes anything under the printing bed
        translate([-0.15,-0.15,-0.3]) cube([0.3,0.3,0.3]);
        // Empty space inside of brain slot
        cube([0.095,0.11,0.05], center=true);
    }
}
// Create brain slot
rotate([0, 0, 0]) translate([0.0, 0.0, 0.0]) scale([0.001, 0.001, 0.001]) import("brainOrganSlot.stl", center=true);