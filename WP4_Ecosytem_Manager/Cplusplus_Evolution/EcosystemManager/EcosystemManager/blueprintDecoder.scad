
// Blueprint data
mr = [
    COORDINATES
];
// Number of organs
numberOfOrgans = len(mr);
// Main loop
for (i=[0:numberOfOrgans-1]){
    if(mr[i][1]==13){
        rotate([0, 0, 0]) translate([mr[i][2],mr[i][3],mr[i][4]]) scale([0.001, 0.001, 0.001]) import("brainOrganSlot.stl", center=true);
    }
    if(mr[i][1]==14){
        translate([mr[i][2],mr[i][3],mr[i][4]]) rotate([mr[i][5]*180/3.1415,mr[i][6]*180/3.1415,mr[i][7]*180/3.1415]) scale([0.001, 0.001, 0.001]) import("maleConnector.stl", center=true);
    }
    if(mr[i][1]==17){
        translate([mr[i][2],mr[i][3],mr[i][4]]) rotate([mr[i][5]*180/3.1415,mr[i][6]*180/3.1415,mr[i][7]*180/3.1415]) cube([0.02, 0.02, 0.02]);
    }
}