// =============================================================================
// insectLamp - 4-channel LED holder, v2
// =============================================================================
// Two-part snap-fit housing with passive cooling, cable management, and
// flexible mounting (pole clamp / flat surface / keyhole / tilt bracket).
//
// Render selector: set `part` to one of "top", "bottom", "clamp", "tilt",
// "assembly", or "all_plate" then export STL via:
//   openscad -o top.stl    -D 'part="top"'    led_holder_v2.scad
//   openscad -o bottom.stl -D 'part="bottom"' led_holder_v2.scad
//   openscad -o clamp.stl  -D 'part="clamp"'  led_holder_v2.scad
//   openscad -o tilt.stl   -D 'part="tilt"'   led_holder_v2.scad
// =============================================================================

part = "assembly";   // "top" | "bottom" | "clamp" | "tilt" | "assembly" | "all_plate"

// -----------------------------------------------------------------------------
// Core housing dimensions
// -----------------------------------------------------------------------------
holder_length   = 80;    // long axis (X). v1 was 60; bumped to fit cable channels.
holder_width    = 36;    // short axis (Y). v1 was 30.
holder_height   = 22;    // total stack height (top + bottom).
wall            = 2.4;   // generic wall thickness.
heat_floor      = 3.6;   // thickened LED-side floor (acts as heat spreader).
split_ratio     = 0.55;  // top portion = split_ratio * holder_height.

// LED window in the top: rectangular opening that exposes the 4 LEDs.
led_window_len  = 56;
led_window_wid  = 18;

// Snap-fit lip geometry (runs around the parting line).
lip_thickness   = 1.2;
lip_height      = 1.6;
snap_clearance  = 0.15;  // print fit tolerance on snap features.

// -----------------------------------------------------------------------------
// Heatsink fins (top half, vertical, along the long X axis)
// -----------------------------------------------------------------------------
fin_count       = 9;
fin_thickness   = 1.4;
fin_height      = 9;     // fin protrusion above top shell.
fin_inset       = 4;     // X-end inset so fins don't run off the cap.

// -----------------------------------------------------------------------------
// Ventilation slots (top side walls)
// -----------------------------------------------------------------------------
vent_count      = 5;
vent_width      = 2;
vent_height     = 5;
vent_y_offset   = 0;     // 0 = centered vertically on the top half side wall.

// -----------------------------------------------------------------------------
// Cable management (bottom half)
// -----------------------------------------------------------------------------
cable_od                = 6;    // outer diameter of the bundled cable.
strain_relief_slot_w    = 1.2;  // clamping slot width (squeezes the jacket).
strain_relief_depth     = 6;
ziptie_slot_w           = 3;    // anchor slot for an alternative zip-tie route.
grommet_groove_w        = 1.8;  // groove for an O-ring style grommet.
channel_count           = 4;    // one channel per LED pair.
channel_w               = 2.4;
channel_h               = 2.4;
channel_spacing         = 5;

// -----------------------------------------------------------------------------
// Mounting features
// -----------------------------------------------------------------------------
screw_hole_d            = 4.2;   // M4 clearance.
screw_head_d            = 8.0;   // counterbore / keyhole head pocket.
tab_extension           = 10;    // how far the flat-mount tabs stick out (per side).
tab_thickness           = 4;

pole_diameter           = 30;    // default pipe/pole OD the clamp wraps.
clamp_thickness         = 6;
clamp_screw_d           = 4.2;
clamp_ear_w             = 14;

keyhole_slot_len        = 12;    // keyhole quick-mount slot (top of head + slot).
keyhole_head_d          = 9;
keyhole_neck_d          = 4.5;

tilt_bracket_enable     = true;
tilt_angles             = [-30, -15, 0, 15, 30]; // detent positions for tilt bracket.
tilt_pivot_d            = 5;     // pivot bolt diameter.

// -----------------------------------------------------------------------------
// Rendering quality
// -----------------------------------------------------------------------------
$fa = 2;
$fs = 0.4;
eps  = 0.01;            // anti-coincident-face epsilon.

// =============================================================================
// Derived values
// =============================================================================
top_h    = holder_height * split_ratio;
bot_h    = holder_height - top_h;
inner_l  = holder_length - 2 * wall;
inner_w  = holder_width  - 2 * wall;

// =============================================================================
// Helpers
// =============================================================================

// Rounded box (XY corners only) -- a quick sugary primitive.
module rbox(l, w, h, r=2) {
    hull() {
        for (x = [r - l/2, l/2 - r], y = [r - w/2, w/2 - r])
            translate([x, y, 0]) cylinder(h=h, r=r);
    }
}

// =============================================================================
// TOP HALF: shell + LED window + heat fins + side vents
// =============================================================================
module top_half() {
    difference() {
        union() {
            // Outer shell of the top half.
            translate([0, 0, bot_h])
                rbox(holder_length, holder_width, top_h, r=3);

            // Heatsink fin array along the long axis.
            heatsink_fins();

            // Snap-fit lip protrudes downward at the parting line.
            translate([0, 0, bot_h - lip_height])
                snap_lip(inset=lip_thickness + snap_clearance);
        }

        // Hollow interior (cavity that mates with bottom half).
        translate([0, 0, bot_h - eps])
            rbox(inner_l, inner_w, top_h + fin_height, r=2);

        // LED window cut through the top shell.
        translate([0, 0, holder_height - heat_floor])
            rbox(led_window_len, led_window_wid, heat_floor + fin_height + 1, r=1.5);

        // Side ventilation slots.
        side_vents();

        // Trim fins flush with shell ends (cosmetic + print stability).
        for (sx = [-1, 1])
            translate([sx * (holder_length/2 + 5), 0, bot_h + top_h + fin_height/2])
                cube([10, holder_width + 2, fin_height * 3], center=true);
    }
}

// Vertical fins running along X. Surface area scales with fin_count * fin_height.
module heatsink_fins() {
    fin_span     = holder_length - 2 * fin_inset;
    fin_pitch    = (holder_width - 2 * wall) / (fin_count + 1);
    fin_z0       = bot_h + top_h;
    for (i = [1 : fin_count]) {
        y = -holder_width/2 + wall + i * fin_pitch;
        translate([0, y, fin_z0 + fin_height/2])
            cube([fin_span, fin_thickness, fin_height], center=true);
    }
}

// Long horizontal slots on both Y-faces of the top half.
module side_vents() {
    vent_pitch = (holder_length - 2 * (wall + 4)) / vent_count;
    vent_z     = bot_h + top_h/2 + vent_y_offset;
    for (sy = [-1, 1])
        for (i = [0 : vent_count - 1]) {
            x = -holder_length/2 + wall + 4 + vent_pitch * (i + 0.5);
            translate([x, sy * holder_width/2, vent_z])
                cube([vent_pitch * 0.55, wall * 3, vent_height], center=true);
        }
}

// Lip that joins the two halves -- modeled as a thin shell-shaped ring.
module snap_lip(inset=0) {
    difference() {
        rbox(holder_length - 2 * inset, holder_width - 2 * inset, lip_height, r=2.5);
        translate([0, 0, -eps])
            rbox(holder_length - 2 * (inset + lip_thickness),
                 holder_width  - 2 * (inset + lip_thickness),
                 lip_height + 2 * eps, r=2);
    }
}

// =============================================================================
// BOTTOM HALF: shell, cable entry, cable channels, mounting features
// =============================================================================
module bottom_half() {
    difference() {
        union() {
            // Outer shell.
            rbox(holder_length, holder_width, bot_h, r=3);

            // Mating lip that pokes up into the top half (sized for snap_clearance).
            translate([0, 0, bot_h - eps])
                snap_lip(inset=lip_thickness * 2 + snap_clearance);

            // Flat-mount tabs (X-end ears with screw holes).
            mount_tabs();

            // Internal cable organizer ribs.
            cable_channels();
        }

        // Hollowed interior.
        translate([0, 0, wall])
            rbox(inner_l, inner_w, bot_h, r=2);

        // Cable entry (strain relief + grommet groove + zip-tie anchor).
        cable_entry();

        // Screw holes through the mount tabs.
        mount_tab_holes();

        // Keyhole slots in the floor for quick-attach to a screw head.
        keyhole_slots();

        // Pole-clamp interface bolt holes through the back face (only used when
        // attaching the optional pole_clamp piece).
        pole_clamp_holes();
    }
}

// Strain relief + grommet groove + zip-tie slot, all at the -X end.
module cable_entry() {
    // Main bore for the cable.
    translate([-holder_length/2 - eps, 0, bot_h/2])
        rotate([0, 90, 0])
            cylinder(h=wall + 2*eps, d=cable_od + 0.4);

    // Clamping slot just inside the wall: cable is squeezed when the top
    // half is snapped on, providing strain relief.
    translate([-holder_length/2 + wall + 0.5, 0, bot_h/2])
        rotate([0, 90, 0])
            cylinder(h=strain_relief_depth, d=cable_od - strain_relief_slot_w);

    // Grommet groove (ring around the bore on the outer face).
    translate([-holder_length/2 - eps, 0, bot_h/2])
        rotate([0, 90, 0])
            difference() {
                cylinder(h=grommet_groove_w, d=cable_od + 2.4);
                translate([0, 0, -eps])
                    cylinder(h=grommet_groove_w + 2*eps, d=cable_od + 0.4);
            }

    // Zip-tie anchor slot just inboard of the entry -- a slot through the floor.
    translate([-holder_length/2 + wall + strain_relief_depth + 3, 0, -eps])
        cube([ziptie_slot_w, cable_od + 6, wall + 2*eps], center=true);
}

// Four parallel ribs running along X to separate each channel's wire pair.
module cable_channels() {
    base_x = -holder_length/2 + wall + strain_relief_depth + 8;
    span_x = holder_length - (wall + strain_relief_depth + 8) - wall - 4;
    y0     = -(channel_count - 1) * channel_spacing / 2;
    for (i = [0 : channel_count]) {            // count+1 ribs = count channels.
        y = y0 - channel_spacing/2 + i * channel_spacing;
        translate([base_x + span_x/2, y, wall])
            cube([span_x, channel_w/2, channel_h], center=true);
    }
}

// Flat-mount ears extending past the housing on +X and -X.
module mount_tabs() {
    for (sx = [-1, 1])
        translate([sx * (holder_length/2 + tab_extension/2 - eps), 0, 0])
            rbox(tab_extension + 1, holder_width * 0.7, tab_thickness, r=3);
}

module mount_tab_holes() {
    for (sx = [-1, 1])
        translate([sx * (holder_length/2 + tab_extension * 0.55), 0, -eps])
            cylinder(h=tab_thickness + 1, d=screw_hole_d);
}

// Keyhole slots cut through the floor (one on each Y-side, centered in X)
// so the holder can drop onto two protruding screws and slide to lock.
module keyhole_slots() {
    for (sy = [-1, 1])
        translate([0, sy * (holder_width/2 - 5), -eps])
            keyhole(keyhole_head_d, keyhole_neck_d, keyhole_slot_len, wall + 2*eps);
}

module keyhole(head_d, neck_d, slot_len, depth) {
    union() {
        cylinder(h=depth, d=head_d);
        translate([slot_len/2, 0, depth/2])
            cube([slot_len, neck_d, depth], center=true);
        translate([slot_len, 0, 0]) cylinder(h=depth, d=neck_d);
    }
}

// Two bolt holes in the underside, spaced for the pole_clamp ear pattern.
module pole_clamp_holes() {
    for (sx = [-1, 1])
        translate([sx * (clamp_ear_w/2 + clamp_screw_d), 0, -eps])
            cylinder(h=wall + 2*eps, d=clamp_screw_d);
}

// =============================================================================
// POLE / PIPE CLAMP (separate part)
// =============================================================================
// Hinged two-jaw clamp that bolts to the bottom half's underside.
module pole_clamp() {
    jaw_w  = pole_diameter + clamp_thickness * 2;
    jaw_h  = clamp_thickness * 3;
    difference() {
        union() {
            // Two opposing jaws.
            for (sy = [-1, 1])
                translate([0, sy * (pole_diameter/2 + clamp_thickness/2), 0])
                    cube([jaw_w, clamp_thickness, jaw_h], center=true);
            // Ears on each end with bolt holes.
            for (sx = [-1, 1])
                translate([sx * (jaw_w/2 + clamp_ear_w/2 - eps), 0, 0])
                    cube([clamp_ear_w, pole_diameter + clamp_thickness * 2, jaw_h], center=true);
        }
        // Pipe bore.
        cylinder(h=jaw_h + 2, d=pole_diameter, center=true);
        // Pinch bolt holes through the ears.
        for (sx = [-1, 1])
            translate([sx * (jaw_w/2 + clamp_ear_w/2), 0, 0])
                rotate([90, 0, 0])
                    cylinder(h=pole_diameter + clamp_thickness * 4 + 2,
                             d=clamp_screw_d, center=true);
    }
}

// =============================================================================
// TILT BRACKET (optional, separate part)
// =============================================================================
// Pivots the holder around Y. Detents at the angles listed in `tilt_angles`.
module tilt_bracket() {
    if (!tilt_bracket_enable) {
        // No-op placeholder so part="tilt" never produces an empty STL accidentally.
        cube(eps);
    } else {
        plate_l = holder_length * 0.6;
        plate_w = holder_width;
        difference() {
            union() {
                // Base plate (mounts to surface / pole clamp).
                cube([plate_l, plate_w, tab_thickness], center=true);
                // Two side walls with pivot holes.
                for (sy = [-1, 1])
                    translate([0, sy * (plate_w/2 - wall/2), plate_l/4])
                        cube([plate_l, wall, plate_l/2], center=true);
            }
            // Pivot bore.
            translate([0, 0, plate_l/3])
                rotate([90, 0, 0])
                    cylinder(h=plate_w + 2, d=tilt_pivot_d, center=true);
            // Detent holes for each tilt angle.
            for (a = tilt_angles)
                translate([0, 0, plate_l/3])
                    rotate([0, a, 0])
                        translate([0, 0, plate_l/4])
                            rotate([90, 0, 0])
                                cylinder(h=plate_w + 2, d=clamp_screw_d, center=true);
            // Base-plate mounting screws.
            for (sx = [-1, 1])
                translate([sx * plate_l/3, 0, -tab_thickness/2 - eps])
                    cylinder(h=tab_thickness + 2*eps, d=screw_hole_d);
        }
    }
}

// =============================================================================
// Layout helpers
// =============================================================================

module assembly() {
    bottom_half();
    // Top half is the same model -- shown lifted apart for clarity in preview.
    color([0.7, 0.85, 1.0, 0.9]) top_half();
}

// Print plate: lay every part flat on Z=0 for slicing.
module all_plate() {
    translate([-holder_length/2 - 8, holder_width/2 + 12, 0])
        bottom_half();
    translate([ holder_length/2 + 8, holder_width/2 + 12, top_h + fin_height])
        rotate([180, 0, 0]) top_half();
    translate([-holder_length/2 - 8, -holder_width/2 - 30, 0])
        pole_clamp();
    if (tilt_bracket_enable)
        translate([ holder_length/2 + 8, -holder_width/2 - 30, tab_thickness/2])
            tilt_bracket();
}

// =============================================================================
// Render dispatch
// =============================================================================
if      (part == "top")       top_half();
else if (part == "bottom")    bottom_half();
else if (part == "clamp")     pole_clamp();
else if (part == "tilt")      tilt_bracket();
else if (part == "all_plate") all_plate();
else                          assembly();
