TString color_code [8]={"#343434","#1A3947","#566575","#797983","#EFBD9D","#FCA26E","#F5751D","#F5321D"};
TString color_code_2 [8]={"#CC768D","#19768D","#DDA573","#009193","#6E9193","#941100","#A08144","#517E66"};
double noise_hit_distance = 0.234; // note : 3 sigma of the l1 residual width

// note : the actual ladder position (unit : mm), the value is no longer to be 26.1, 
// note : since 26.1 is the gap between ladders, without the consideration of ladder thickness
double actual_xpos [3] = {0,29.552,59.104}; 

double slope_cut = 0.015; // note : the parameter for the DUT
double slope_correction = 0.002725; // note : to correct the slope distribution. 

int hit_allowed_in_adjacent_layers = 1;
int cluster_size_requirement = 0; // note : the cluster size of single cluster
int cluster_size_requirement_combine = 2;
double cluster_adc_value_requirement = 22.5;


double INTT_strip_width = 0.078;
double lower_section_initial = -9.945;
double upper_section_initial = 0.039;

int boundary_cut = 5;


void change_noise_hit_distance(double new_noise_hit_distance)
{
    noise_hit_distance = new_noise_hit_distance;
}

void change_slope_cut(double new_slope_cut)
{
    slope_cut = new_slope_cut;
}

void change_slope_correction(double new_slope_correction)
{
    slope_correction = new_slope_correction;
}

void change_cluster_size_requirement(double new_cluster_size_requirement)
{
    cluster_size_requirement = new_cluster_size_requirement;
}

void change_cluster_size_requirement_combine(double new_cluster_size_requirement_combine)
{
    cluster_size_requirement_combine = new_cluster_size_requirement_combine;
}

void change_cluster_adc_value_requirement(double new_cluster_adc_value_requirement)
{
    cluster_adc_value_requirement = new_cluster_adc_value_requirement;
}

void change_hit_allowed_in_adjacent_layers(double new_hit_allowed_in_adjacent_layers)
{
    hit_allowed_in_adjacent_layers = new_hit_allowed_in_adjacent_layers;
}

void change_INTT_strip_width(double new_INTT_strip_width)
{
    INTT_strip_width = new_INTT_strip_width;
}

void change_lower_section_initial(double new_lower_section_initial)
{
    lower_section_initial = new_lower_section_initial;
}

void change_upper_section_initial(double new_upper_section_initial)
{
    upper_section_initial = new_upper_section_initial;
}

void change_boundary_cut(double new_boundary_cut)
{
    boundary_cut = new_boundary_cut;
}