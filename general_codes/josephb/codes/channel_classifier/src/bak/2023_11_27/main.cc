#include "ChannelClassifierHelper.h"

#include <cstdio>

int run_num = 20869;

int main()
{
	std::string dir_format = "/sphenix/user/jbertaux/Data/channel_classifier/run_%08d/";

	char buff[256];
	snprintf(buff, sizeof(buff), dir_format.c_str(), run_num);
	std::string dir = buff;

	std::string fit_plot_name = dir + "plots/fit.png";
	std::string pre_fit_plot_name = dir + "plots/iteration_%02d_before_fit.png";
	std::string post_fit_plot_name = dir + "plots/iteration_%02d_after_fit.png";

	ChannelClassifierHelper c;
	c.SetNumSig(4);
	c.SetNumAcceptionTerms(2);
	c.SetNumRejectionTerms(2);

	c.SetVerbose(true);
	c.SetFitPlotName(fit_plot_name);
	c.SetPreFitPlotName(pre_fit_plot_name);
	c.SetPostFitPlotName(post_fit_plot_name);
	c.SetFitMode("QN");

	std::string hit_file_name = dir + "hits.root";
	std::string hit_tree_name = "channel_tree";
	if(c.FitFile(hit_file_name, hit_tree_name))
	{
		std::cout << "Fit failed" << std::endl;
		std::cout << "Exiting" << std::endl;

		return 1;
	}

	std::string out_file_name = dir + "status.root";
	std::string out_tree_name = "status_tree";
	if(c.Classify(out_file_name, out_tree_name))
	{
		std::cout << "Fit failed" << std::endl;
		std::cout << "Exiting" << std::endl;

		return 1;
	}

	std::cout << "Done" << std::endl;
	std::cout << "Output can be found in file:" << std::endl;
	std::cout << "\t" << out_file_name << std::endl;

	return 0;
}
