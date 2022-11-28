double double_gaus (Double_t *x, Double_t *par)
{
    //note : par0 : total height
    //note : par1 : fraction between these 2 gaus function
    //note : par2 : mean -> share to both distribtion
    //note : par3 : sigma of gaus1
    //note : par4 : sigma of gaus2
    double gaussian_eq_1  =  TMath::Exp( -1*( pow(x[0]-par[2],2)/(2*pow(par[3],2)) ) );
    double gaussian_eq_2  =  TMath::Exp( -1*( pow(x[0]-par[2],2)/(2*pow(par[4],2)) ) );
    return par[0] *  (  (par[1] * gaussian_eq_1) + ((1-par[1]) * gaussian_eq_2)     );

}
bool double_gaus_getsigmarange (TF1 * f, double the_portion, double& lower, double& upper )
{
    double xmin = f->GetXmin();
    double xmax = f->GetXmax();

    double mean = f->GetParameter(2);
    double whole_area = f->Integral(xmin,xmax);

    double division = 1e5;
    double step = (xmax - xmin)/division;

    for ( int i = 0; i < int(division); i++ )
    {
        double xmin_current = mean - i * step;
        double xmax_current = mean + i * step;

        double area = f -> Integral(xmin_current,xmax_current);

        if (area / whole_area > the_portion)
        {
            lower = xmin_current;
            upper = xmax_current;
            return true;
        }
    }

    return false;
}