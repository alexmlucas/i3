//linear_extrude(height = 2, center = true, convexity = 10) import(file = "i3_base.svg", center = false, dpi = 96);


//facet number = resolution
$fn=50;

//rounded_square(60, 2);

enclosureWidth = 126;
enclosureLength = 260;
enclosurePitch = 5;
trillGroupWidth = 100;
frameWidth = 6;

difference()
{
    outline(enclosureWidth, enclosureLength, enclosurePitch);
    offset(-frameWidth) outline(enclosureWidth, enclosureLength, enclosurePitch);
}


for(i = [frameWidth+8:trillGroupWidth/4:trillGroupWidth] )
{
    translate([200, i, 0]) trillBar();
}

module trillBar()
{
    width = 101;
    height = 22;
    radius_corner = 3;
    
    translate([radius_corner, radius_corner, 0]) minkowski()
    {
        square([width - (2 * radius_corner), height - (2 * radius_corner)], false);
        circle(radius_corner);
    }
}

module outline(width, length, pinch)
{
    union() 
    {
        translate([width/2, 0, 0]) polygon([[0, 0], [0, width], [length, width - pinch], [length, pinch]]);
        translate([width/2, width/2, 0]) circle(width/2);
    }
    
}

/*module rounded_square(width, radius_corner)
{
    translate([radius_corner, radius_corner, 0]) minkowski()
    {
        square(width - (2 * radius_corner));
        circle(radius_corner);
    }
}*/