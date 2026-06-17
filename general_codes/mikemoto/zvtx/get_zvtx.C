// dca_meanx, dca_meany set by hand

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <cmath>

#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TColor.h>

int n_dotracking = 0;

using namespace std;
TCanvas *c;

TH2F *h_dphi_nocut;
TH2F *h_dphi_cut;
TH2F *h_dtheta_phicut;

#include "least_square2.cc"
#include "track_pT.hh"

struct cluster
{
    int evt;
    uint64_t bco;

    double x, y, z, r;
    double adc;
    double size;
    int layer;
    TVector3 pos;
    int ladder;
    int sensor;        // senser = 0 or 2 : chiptype A // senser = 1 or 3 : chiptype B
    double chip_width; // chip_width=1.6[cm] : chiptype A // chip_width=2.0[cm] : chiptype B

    double x_vtx;
    double y_vtx;
    double zv;
    double r_vtx;

    double theta_clus;
    double phi_clus;

    double dca_mean[3];

    void set(int Evt, uint64_t Bco, double X, double Y, double Z, double Adc, double Size, int Layer, int Ladder, int Sensor)
    {
        evt = Evt;
        bco = Bco;
        x = X;
        y = Y;
        z = Z;
        adc = Adc;
        size = Size;
        layer = Layer;
        pos = TVector3(x, y, z);
        r = y / fabs(y) * sqrt(x * x + y * y);
        // truth vertex
        // x_vtx = X_vtx;
        // y_vtx = Y_vtx;
        // zv = Zv;
        // r_vtx = (y_vtx / fabs(y_vtx)) * sqrt(x_vtx * x_vtx + y_vtx * y_vtx);
        ladder = Ladder;
        sensor = Sensor;
        if (sensor == 0 || sensor == 2)
            chip_width = 16 * 1e-1;

        else if (sensor == 1 || sensor == 3)
            chip_width = 20 * 1e-1;
    }

    /*void print()
    {
        cout << "(" << setw(8) << x << ", " << setw(8) << y << ", " << setw(8) << z << ")\t"
             << size << "\t"
             << layer
             << endl;
    }*/

    double getphi_clus() // from truth vertex
    {
        phi_clus = atan2((y - y_vtx), (x - x_vtx));

        return phi_clus;
    }

    double gettheta_clus() // from truth vertex
    {
        // from the truth vertex
        theta_clus = atan2((fabs(r) - fabs(r_vtx)), (z - zv));

        return theta_clus;
    }
};

struct clustEvent
{
    int run_nu = 0;
    int ievt = 0;
    // int bco_full = 0;
    bool mag_on = false;
    vector<cluster> vclus;
    vector<truth *> vtruth;
    vector<track *> vtrack;

    double dcaz_max = 9999;
    double dcaz_min = -9999;
    double dca2d_max = 9999;
    double dca2d_min = -9999;

    // double dca_mean[3] = {0, 0, 0};
    double dca_mean[3] = {0, 0, 0};

    void clear()
    {
        for (auto itr = vtrack.begin(); itr != vtrack.end(); ++itr)
        {
            delete *itr;
        }
        vtrack.clear();
    }

    void makeonetrack()
    {
        for (unsigned int i = 0; i < vtrack.size(); i++)
        {
            for (unsigned int j = i + 1; j < vtrack.size(); j++)
            {
                bool share_point = ((vtrack[i]->p1 == vtrack[j]->p1) || (vtrack[i]->p2 == vtrack[j]->p2));
                share_point = share_point || ((vtrack[i]->p1 == vtrack[j]->p2) || (vtrack[i]->p2 == vtrack[j]->p1));
                if (share_point)
                {
                    if (fabs(vtrack[i]->dca_2d) > fabs(vtrack[j]->dca_2d))
                    {
                        vtrack[i]->is_deleted = true;
                        continue;
                    }
                    else
                    {
                        vtrack[j]->is_deleted = true;
                    }
                }
            }
        }
    }

    void dca_check(bool Debug)
    {
        for (unsigned int i = 0; i < vtrack.size(); i++)
        {
            if (vtrack[i]->dca_2d > dca2d_max)
                vtrack[i]->dca2d_range_out = true;
            if (vtrack[i]->dca_2d < dca2d_min)
                vtrack[i]->dca2d_range_out = true;

            if (vtrack[i]->dca[2] > dcaz_max)
                vtrack[i]->dcaz_range_out = true;
            if (vtrack[i]->dca[2] < dcaz_min)
                vtrack[i]->dcaz_range_out = true;

            if (vtrack[i]->dcaz_range_out || vtrack[i]->dca2d_range_out)
                vtrack[i]->dca_range_out = true;

            // // if (Debug)
            // {
            //     if (vtrack[i]->dca_mean[2] > vtrack[i]->p1.z() && vtrack[i]->dca_mean[2] < vtrack[i]->p2.z())
            //         vtrack[i]->dca_range_out = true;
            //     if (vtrack[i]->dca_mean[2] > vtrack[i]->p2.z() && vtrack[i]->dca_mean[2] < vtrack[i]->p1.z())
            //         vtrack[i]->dca_range_out = true;
            // }
        }
    }

    void truth_check()
    {
        for (unsigned int i = 0; i < vtruth.size(); i++)
        {
            double p = sqrt(vtruth[i]->m_truthpt * vtruth[i]->m_truthpt + vtruth[i]->m_truthpz * vtruth[i]->m_truthpz);

            if (vtruth[i]->m_status == 1 && fabs(vtruth[i]->m_trutheta) < 2 && p > (50 * 1e-3))
            {
                if (fabs(vtruth[i]->m_truthz_out) < 23.)
                {
                    vtruth[i]->is_intt = true;
                }
            }
        }
    }

    void cluster_check()
    {
        for (unsigned int itrt = 0; itrt < vtruth.size(); itrt++)
        {
            for (unsigned int icls = 0; icls < vclus.size(); icls++)
            {
                if (vclus[icls].layer < 2)
                    continue;

                double d_phi = vclus[icls].getphi_clus() - vtruth[itrt]->m_truthphi;
                d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

                if (fabs(d_phi) < (0.0014 * 3))
                {
                    vtruth[itrt]->have_cluster = true;
                    break;
                }
            }
        }
    }

    void track_check()
    {
        for (unsigned int i = 0; i < vtruth.size(); i++)
        {
            for (unsigned int itrk = 0; itrk < vtrack.size(); itrk++)
            {
                double d_phi = vtrack[itrk]->getphi() - vtruth[i]->m_truthphi;
                d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

                if (fabs(d_phi) < (3.3e-4 * 3))
                {
                    vtruth[i]->have_track = true;
                    break;
                }
            }
        }
    }

    void charge_check()
    {
        for (unsigned int i = 0; i < vtruth.size(); i++)
        {
            if ((abs(vtruth[i]->m_truthpid) != 22 && abs(vtruth[i]->m_truthpid) != 2112 && abs(vtruth[i]->m_truthpid) != 130 && abs(vtruth[i]->m_truthpid) != 310 && abs(vtruth[i]->m_truthpid) != 111))
                vtruth[i]->is_charged = true;
        }
    }

    void draw_intt(int mode)
    {

        const double kLayer_radii[4] = {7.1888, 7.800, 9.680, 10.330};
        if (mode == 0)
        {
            for (int i = 0; i < 4; i++)
            {
                auto circle = new TEllipse(0.0, 0.0, kLayer_radii[i]);
                circle->SetLineColorAlpha(kGray, 0.5);
                circle->SetLineWidth(2);
                circle->SetFillStyle(0);
                circle->Draw("same");
            }
        }
        else if (mode == 1)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    TLine *line = new TLine(-23., (2 * j - 1) * kLayer_radii[i], 23., (2 * j - 1) * kLayer_radii[i]);
                    line->SetLineColorAlpha(kGray, 0.5);
                    line->SetLineWidth(2);
                    line->Draw("same");
                }
            }
        }
    }

    void draw_frame(int mode = 0)
    {
        TH1 *flame;
        string mag;
        if (mag_on)
            mag = "B-on";
        else
            mag = "B-off";

        string title;

        if (mode == 0)
        {
            if (run_nu == 1)
                title = Form("x-y plane {MC(%s) event %d};x[cm];y[cm]", mag.c_str(), ievt);
            else
                title = Form("x-y plane {run%d(%s) event %d};x[cm];y[cm]", run_nu, mag.c_str(), ievt);
            flame = gPad->DrawFrame(-15, -15, 15, 15, title.c_str());
        }
        else if (mode == 1)
        {
            if (run_nu == 1)
                title = Form("z-r plane {MC(%s) event %d};z[cm];r[cm]", mag.c_str(), ievt);
            else
                title = Form("z-r plane {run%d(%s) event %d};z[cm];r[cm]", run_nu, mag.c_str(), ievt);
            flame = gPad->DrawFrame(-25, -15, 25, 15, title.c_str());
        }
    }

    void draw_tracklets(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
    {

        bool condition = !reverse;
        for (unsigned int i = 0; i < vtrack.size(); i++)
        {
            if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
            {
                continue;
            }

            if (vtrack[i]->is_deleted == condition && is_deleted == true)
            {
                continue;
            }

            TGraph *g_temp = new TGraph();
            g_temp->SetMarkerColor(color);
            g_temp->SetMarkerStyle(20);
            g_temp->SetMarkerSize(0.8);

            if (mode == 0)
            {
                g_temp->SetPoint(0, vtrack[i]->p1.x(), vtrack[i]->p1.y());
                g_temp->SetPoint(1, vtrack[i]->p2.x(), vtrack[i]->p2.y());
            }
            else if (mode == 1)
            {
                g_temp->SetPoint(0, vtrack[i]->p1.z(), vtrack[i]->getpointr(1));
                g_temp->SetPoint(1, vtrack[i]->p2.z(), vtrack[i]->getpointr(2));
            }
            g_temp->Draw("PLsame");
        }
    }

    void draw_trackline(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
    {
        bool condition = !reverse;
        vector<int> colorset = {kOrange - 3, kOrange + 3, kPink - 3, kPink + 3, kViolet - 3, kViolet + 3, kAzure - 3, kAzure + 3, kTeal - 3, kTeal + 3, kSpring - 3, kSpring + 3};
        int color_ = 0;

        for (unsigned int i = 0; i < vtrack.size(); i++)
        {

            if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
            {
                continue;
            }

            if (vtrack[i]->is_deleted == condition && is_deleted == true)
            {
                continue;
            }
            color_ = color_ % colorset.size();
            TGraph *g_temp = new TGraph();
            g_temp->SetMarkerColor(colorset[color_]);
            // g_temp->SetLineColor(color);
            g_temp->SetMarkerStyle(20);
            g_temp->SetMarkerSize(0.8);

            if (mode == 0)
            {
                vtrack[i]->track_xy->SetLineColorAlpha(colorset[color_], 0.5);
                // vtrack[i]->track_xy->SetLineColorAlpha(color, 0.5);
                if (vtrack[i]->dca_mean[0] < vtrack[i]->p1.x())
                {
                    vtrack[i]->track_xy->SetRange(vtrack[i]->dca_mean[0], 15);
                }
                else
                {
                    vtrack[i]->track_xy->SetRange(-15, vtrack[i]->dca_mean[0]);
                }

                vtrack[i]->track_xy->Draw("Lsame");
                g_temp->SetPoint(0, vtrack[i]->dca_mean[0], vtrack[i]->dca_mean[1]);
                g_temp->SetPoint(1, vtrack[i]->p1.x(), vtrack[i]->p1.y());
                g_temp->SetPoint(2, vtrack[i]->p2.x(), vtrack[i]->p2.y());
            }
            else if (mode == 1)
            {

                if (isinf(vtrack[i]->track_rz->GetParameter(1)))
                {
                    TLine *line = nullptr;
                    if (vtrack[i]->p1.y() > 0)
                        line = new TLine(vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3), vtrack[i]->dca_mean[2], 15);
                    else
                        line = new TLine(vtrack[i]->dca_mean[2], -15, vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3));

                    line->SetLineColorAlpha(colorset[color_], 0.5);
                    line->SetLineWidth(2);
                    line->Draw("same");
                }
                else
                {

                    vtrack[i]->track_rz->SetLineColorAlpha(colorset[color_], 0.5);

                    int y = 0;
                    if (vtrack[i]->p1.y() > 0)
                        y = 15;
                    else
                        y = -15;

                    double r1 = vtrack[i]->track_rz_inv->Eval(vtrack[i]->getpointr(3));
                    double r2 = vtrack[i]->track_rz_inv->Eval(y);

                    if (r1 <= r2)
                        vtrack[i]->track_rz->SetRange(r1, r2);
                    else
                        vtrack[i]->track_rz->SetRange(r2, r1);

                    vtrack[i]->track_rz->Draw("Lsame");
                }
                g_temp->SetPoint(0, vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3));
                g_temp->SetPoint(1, vtrack[i]->p1.z(), vtrack[i]->getpointr(1));
                g_temp->SetPoint(2, vtrack[i]->p2.z(), vtrack[i]->getpointr(2));
            }
            g_temp->Draw("Psame");
            color_++;
        }
    }

    double rad_deg(double Rad)
    {
        Rad = Rad / M_PI * 180;
        if (Rad < 0)
            Rad += 360;

        return Rad;
    }

    void draw_curve2(int mode, double px, double py, double pz, double rad, double cx, double cy, int color)
    {
        double sign_x = px / fabs(px);
        double sign_y = py / fabs(py);

        cout << "sign_x : " << sign_x << "  " << sign_y << endl;
        double x1 = sqrt((rad * rad) - ((sign_y * 15 - cy) * (sign_y * 15 - cy))) + cx;  // y = 15
        double x2 = -sqrt((rad * rad) - ((sign_y * 15 - cy) * (sign_y * 15 - cy))) + cx; // y = 15
        double y1 = sqrt((rad * rad) - ((sign_x * 15 - cx) * (sign_x * 15 - cx))) + cy;  // x = 15
        double y2 = -sqrt((rad * rad) - ((sign_x * 15 - cx) * (sign_x * 15 - cx))) + cy; // x = 15
        cout << "frame : " << sign_y * 15 << "  " << sign_x * 15 << endl;

        cout << x1 << "  " << x2 << "  " << y1 << "  " << y2 << "  " << endl;

        // cross
        double cross_x1 = px * (x1 - dca_mean[0]) + py * (sign_y * 15 - dca_mean[1]);
        double cross_x2 = px * (x2 - dca_mean[0]) + py * (sign_y * 15 - dca_mean[1]);
        double cross_y1 = px * (sign_x * 15 - dca_mean[0]) + py * (y1 - dca_mean[1]);
        double cross_y2 = px * (sign_x * 15 - dca_mean[0]) + py * (y2 - dca_mean[1]);

        cout << "cross : " << cross_x1 << "  " << cross_x2 << "  " << cross_y1 << "  " << cross_y2 << "  " << endl;
        double temp_x, temp_y;

        // if(cross_x1 > 0)
        // temp_x = x1;
        // else if(cross_x2 > 0)
        // temp_x = x2;

        // if(cross_y1 > 0)
        // temp_y = y1;
        // else if(cross_y2 > 0)
        // temp_y = y2;

        if (-15 <= x1 && x1 <= 15)
            temp_x = x1;
        if (-15 <= x2 && x2 <= 15)
            temp_x = x2;
        if (-15 <= y1 && y1 <= 15)
            temp_y = y1;
        if (-15 <= y2 && y2 <= 15)
            temp_y = y2;

        double temp_phix = atan2(sign_y * 15 - cy, temp_x - cx);
        double temp_phiy = atan2(temp_y - cy, sign_x * 15 - cx);

        temp_phix = rad_deg(temp_phix);
        temp_phiy = rad_deg(temp_phiy);

        double phi_min, phi_max;
        double phic = atan2(0 - cy, 0 - cx);
        phic = rad_deg(phic);
        double temp_phi;

        bool nan_x = ((rad) * (rad) - (sign_x * 15 - cx) * (sign_x * 15 - cx) < 0);
        bool nan_y = ((rad) * (rad) - (sign_y * 15 - cy) * (sign_y * 15 - cy) < 0);

        if (isnan(temp_phix) && isnan(temp_phiy))
        {
            temp_phi = phic;
        }
        else if (isnan(temp_phix))
        {
            temp_phi = temp_phiy;
        }
        else if (isnan(temp_phiy))
        {
            temp_phi = temp_phix;
        }
        else
        {
            if (fabs(phic - temp_phix) > fabs(phic - temp_phiy))
                temp_phi = temp_phiy;
            else
                temp_phi = temp_phix;
        }

        if (phic < temp_phi)
        {
            phi_min = phic;
            phi_max = temp_phi;
        }
        else
        {
            phi_min = temp_phi;
            phi_max = phic;
        }

        if (fabs(phi_max - phi_min) > 180)
        {
            phi_max = -(360 - phi_max);
            double temp = phi_min;
            phi_min = phi_max;
            phi_max = temp;
        }

        if (mode == 0)
        {
            TEllipse *circle = new TEllipse(cx, cy, rad, 0, phi_min, phi_max);
            circle->SetLineColorAlpha(color, 0.5);
            circle->SetFillStyle(0);
            circle->SetLineWidth(2);
            circle->SetLineStyle(1);
            circle->SetNoEdges(1);
            circle->Draw("same");
            // TEllipse *circle2 = new TEllipse(cx, cy, rad, 0, 0, 360);
            // circle2->SetLineColorAlpha(kGray, 0.2);
            // circle2->SetFillStyle(0);
            // circle2->SetLineWidth(2);
            // circle2->SetLineStyle(1);
            // circle2->SetNoEdges(1);
            // circle2->Draw("same");
        }
        else if (mode == 1)
        {
            TEllipse *circle_truth = new TEllipse(cx, cy, rad, 0, phi_min, phi_max);
            circle_truth->SetLineColorAlpha(color, 0.5);
            circle_truth->SetFillStyle(0);
            circle_truth->SetLineWidth(2);
            circle_truth->SetLineStyle(9);
            circle_truth->SetNoEdges(1);
            circle_truth->Draw("same");
        }
    }

    void draw_trackcurve(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
    {
        vector<int> colorset = {kOrange - 3, kOrange + 3, kPink - 3, kPink + 3, kViolet - 3, kViolet + 3, kAzure - 3, kAzure + 3, kTeal - 3, kTeal + 3, kSpring - 3, kSpring + 3};
        int color_ = 0;
        bool condition = !reverse;

        for (unsigned int i = 0; i < vtrack.size(); i++)
        {
            color_ = color_ % colorset.size();

            if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
            {
                continue;
            }

            if (vtrack[i]->is_deleted == condition && is_deleted == true)
            {
                continue;
            }

            TGraph *g_temp = new TGraph();
            g_temp->SetMarkerColor(colorset[color_]);
            g_temp->SetLineColor(colorset[color_]);
            g_temp->SetMarkerStyle(20);
            g_temp->SetMarkerSize(0.8);

            if (mode == 0)
            {
                draw_curve2(0, vtrack[i]->p_reco[0], vtrack[i]->p_reco[1], vtrack[i]->p_reco[2], vtrack[i]->rad, vtrack[i]->cx, vtrack[i]->cy, colorset[color_]);

                cout << "p1 : " << vtrack[i]->p1.x() << "  " << vtrack[i]->p1.y() << endl;

                g_temp->SetPoint(0, vtrack[i]->dca_mean[0], vtrack[i]->dca_mean[1]);
                g_temp->SetPoint(1, vtrack[i]->p1.x(), vtrack[i]->p1.y());
                g_temp->SetPoint(2, vtrack[i]->p2.x(), vtrack[i]->p2.y());
            }

            g_temp->Draw("Psame");
            color_++;
        }
    }
    void draw_truthcurve(int mode = 0, bool does_overlay = false, int color = kBlack, bool only_far_cluster = false, bool only_far_track = false)
    {
        for (unsigned int i = 0; i < vtruth.size(); i++)
        {
            if (vtruth[i]->is_intt == false)
                continue;

            if (vtruth[i]->is_charged == false)
                continue;

            if (vtruth[i]->have_track == true && only_far_track)
                continue;

            if (vtruth[i]->have_cluster == true && only_far_cluster)
                continue;

            TGraph *g_temp = new TGraph();
            g_temp->SetMarkerColor(color);
            g_temp->SetLineColor(color);
            g_temp->SetMarkerStyle(20);
            g_temp->SetMarkerSize(0.8);

            if (mode == 0)
            {
                draw_curve2(1, vtruth[i]->m_truthpx, vtruth[i]->m_truthpy, vtruth[i]->m_truthpz, vtruth[i]->m_rad, vtruth[i]->center[0], vtruth[i]->center[1], color);

                g_temp->SetPoint(0, vtruth[i]->m_xvtx, vtruth[i]->m_yvtx);
            }

            g_temp->Draw("Psame");
        }
    }

    void draw_truthline(int mode = 0, bool does_overlay = false, int color = kBlack, bool only_far_cluster = false, bool only_far_track = false)
    {
        for (unsigned int i = 0; i < vtruth.size(); i++)
        {
            if (vtruth[i]->is_intt == false)
                continue;

            if (vtruth[i]->is_charged == false)
                continue;

            if (vtruth[i]->have_track == true && only_far_track)
                continue;

            if (vtruth[i]->have_cluster == true && only_far_cluster)
                continue;

            TGraph *g_temp = new TGraph();
            g_temp->SetMarkerColor(color);
            g_temp->SetLineColor(color);
            g_temp->SetMarkerStyle(20);
            g_temp->SetMarkerSize(0.8);

            if (mode == 0)
            {
                vtruth[i]->truth_xy->SetLineColorAlpha(color, 0.5);
                vtruth[i]->truth_xy->SetLineStyle(9);
                vtruth[i]->truth_xy->SetLineWidth(2);

                if (vtruth[i]->m_truthpx > 0)
                {
                    vtruth[i]->truth_xy->SetRange(vtruth[i]->m_xvtx, 15);
                }
                else
                {
                    vtruth[i]->truth_xy->SetRange(-15, vtruth[i]->m_xvtx);
                }
                vtruth[i]->truth_xy->Draw("Lsame");

                g_temp->SetPoint(0, vtruth[i]->m_xvtx, vtruth[i]->m_yvtx);
            }
            else if (mode == 1)
            {
                vtruth[i]->truth_rz->SetLineColorAlpha(color, 0.5);
                vtruth[i]->truth_rz->SetLineStyle(9);
                vtruth[i]->truth_rz->SetLineWidth(2);

                int y = 0;
                if (vtruth[i]->m_truthpy > 0)
                    y = 15;
                else
                    y = -15;
                double r_min = vtruth[i]->truth_rz->GetX(vtruth[i]->getpointr(3));
                double r_max = vtruth[i]->truth_rz->GetX(y);
                if (r_min < r_max)
                    vtruth[i]->truth_rz->SetRange(r_min, r_max);
                else
                    vtruth[i]->truth_rz->SetRange(r_max, r_min);

                vtruth[i]->truth_rz->Draw("Lsame");
                g_temp->SetPoint(0, vtruth[i]->m_zvtx, vtruth[i]->getpointr(3));
            }
            g_temp->Draw("Psame");
        }
    }

    void draw_clusters(int mode = 0, bool does_overlay = false, int color = kBlack)
    {
        for (unsigned int i = 0; i < vclus.size(); i++)
        {
            TGraph *g_temp = new TGraph();
            g_temp->SetMarkerColorAlpha(color, 0.5);
            // g_temp->SetLineColor(color);
            g_temp->SetMarkerStyle(20);
            g_temp->SetMarkerSize(0.8);

            if (mode == 0)
            {
                g_temp->SetPoint(0, vclus[i].x, vclus[i].y);
            }
            else if (mode == 1)
            {
                g_temp->SetPoint(0, vclus[i].z, vclus[i].r);
            }
            g_temp->Draw("psame");
        }
    }
};

template <class D>
void erase(vector<D> &vec)
{
    vec.erase(vec.begin(), vec.end());
}

template <class D>
void reverse(vector<D> &vec)
{
    reverse(vec.begin(), vec.end());
}

int getSector(double x, double y)
{
    double angle = atan2(y, x);
    if (angle < 0)
        angle += 2 * M_PI;
    return static_cast<int>(angle / (M_PI / 8)) % 16;
}

bool dotracking(clustEvent &vCluster)
{
    vector<vector<cluster *>> sec_out(16);

    for (unsigned int i = 0; i < vCluster.vclus.size(); i++)
    // for (unsigned int i = 0; i < 1; i++)
    {
        cluster &c2 = vCluster.vclus[i];
        if (c2.layer <= 1) // outer
            continue;
        int sec = getSector(c2.pos.x(), c2.pos.y());
        sec_out[sec].push_back(&c2);
    }

    TVector3 beamspot = {vCluster.dca_mean[0], vCluster.dca_mean[1], 0};

    for (unsigned int i = 0; i < vCluster.vclus.size(); i++)
    {
        cluster c1 = vCluster.vclus[i];
        if (2 <= c1.layer) // inner
            continue;
        int s1 = getSector(c1.pos.x(), c1.pos.y());
        TVector3 p1 = c1.pos - beamspot;
        TVector3 p1_left(p1.x(), p1.y(), p1.z() - c1.chip_width / 2);  // in_left
        TVector3 p1_right(p1.x(), p1.y(), p1.z() + c1.chip_width / 2); // in_right
        double p1_phi = atan2(p1.y(), p1.x());
        double p1_r = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
        double p1_theta = atan2(p1_r, p1.z());

        for (int ds = -1; ds <= 1; ++ds)
        {
            int s2 = (s1 + ds + 16) % 16;

            for (int sec = 0; sec < sec_out[s2].size(); ++sec)
            {
                cluster c2 = *sec_out[s2][sec]; // outer
                // TVector3 beamspot = {vCluster.dca_mean[0], vCluster.dca_mean[1], 0};
                TVector3 p2 = c2.pos - beamspot;
                double p2_phi = atan2(p2.y(), p2.x());
                double d_phi = p2_phi - p1_phi;

                // cout << "p1_phi : " << p1_phi << endl;
                // cout << "p2_phi : " << p2_phi << endl;
                // d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));
                if (d_phi > M_PI)
                    d_phi -= 2 * M_PI;
                else if (d_phi < -M_PI)
                    d_phi += 2 * M_PI;
                h_dphi_nocut->Fill(p1_phi, d_phi);

                // if (fabs(d_phi) > 0.01)
                // if (fabs(d_phi) > 0.04)
                if (fabs(d_phi) > 0.02) // d_phi cut
                    continue;
                h_dphi_cut->Fill(p1_phi, d_phi);

                // cluster posision + chip width
                TVector3 p2_left(p2.x(), p2.y(), p2.z() - c2.chip_width / 2);  // out_left
                TVector3 p2_right(p2.x(), p2.y(), p2.z() + c2.chip_width / 2); // out_right
                // cout << p2_left.z() << "   " << p2.z() << "   " << p2_right.z() << endl;

                double p2_r = sqrt(p2.x() * p2.x() + p2.y() * p2.y());
                double p2_theta = atan2(p2_r, p2.z());
                double d_theta = p2_theta - p1_theta;
                // if (n_dotracking == 1)

                h_dtheta_phicut->Fill(p1_theta, d_theta);

                // if (fabs(d_theta) > 0.4) // d_theta cut
                //     continue;

                TVector3 u = p2 - p1;
                TVector3 u_l = p2_right - p1_left;
                TVector3 u_r = p2_left - p1_right;
                double unorm = sqrt(u.x() * u.x() + u.y() * u.y());
                // unit vector in 2D
                double ux = u.x() / unorm;
                double uy = u.y() / unorm;
                double uz = u.z() / unorm; // same normalization factor(2D) is multiplied
                double uz_l = u_l.z() / unorm;
                double uz_r = u_r.z() / unorm;

                TVector3 p0 = beamspot - p1;

                double dca_p0 = p0.x() * uy - p0.y() * ux; // cross product of p0 x u
                double len_p0 = p0.x() * ux + p0.y() * uy; // dot product of p0 . u

                // beam center in X-Y plane
                double vx = len_p0 * ux + p1.x();
                double vy = len_p0 * uy + p1.y();
                double vz = len_p0 * uz + p1.z();
                double vz_l = len_p0 * uz_l + p1_left.z();
                double vz_r = len_p0 * uz_r + p1_right.z();

                track *tr = new track();
                tr->phi_in = p1_phi;
                tr->phi_out = p2_phi;
                tr->theta_in = c1.gettheta_clus();
                tr->theta_out = c2.gettheta_clus();
                tr->d_phi = d_phi;
                tr->d_theta = d_theta;
                tr->charge = dca_p0 / fabs(dca_p0);

                // track slope & intercept
                tr->ladder1 = (c1.layer == 0) ? (2 * c1.ladder + 1) : (2 * c1.ladder);
                tr->ladder2 = (c2.layer == 2) ? (24 + 2 * c2.ladder + 1) : (24 + 2 * c2.ladder);

                tr->dca[0] = vx;
                tr->dca[1] = vy;
                tr->dca[2] = vz;
                tr->dca[3] = vz_l;
                tr->dca[4] = vz_r;
                // cout << tr->dca[3] << "   " << tr->dca[2] << "   " << tr->dca[4] << endl;

                tr->p1 = c1.pos;
                tr->p2 = c2.pos;

                tr->dca_2d = dca_p0;
                vCluster.vtrack.push_back(tr);
            }
        }
    }

    return true;
}

double calc_Peak(TH1F *h)
{
    int binN_max = h->GetMaximumBin();                 // getting the highest bin number
    double maxbincontent = h->GetBinContent(binN_max); // getting content of the highest bin
    vector<double> maxbins;
    int count = 1;

    for (int i = binN_max; i <= h->GetNbinsX(); i++)
    {
        // if the contents of the next bin are same as the highest bin's, add to "maxbins"
        maxbins.push_back(i);
        if (h->GetBinContent(i) == h->GetBinContent(i + 1) && h->GetBinContent(i) == maxbincontent)
        {
            maxbins.push_back(i + 1);
            count++;
        }
        else
            break;
    }

    double minbinValue = h->GetXaxis()->GetBinLowEdge(*std::min_element(maxbins.begin(), maxbins.end()));
    double maxbinValue = h->GetXaxis()->GetBinUpEdge(*std::max_element(maxbins.begin(), maxbins.end()));

    double peak = -999.9;
    peak = (minbinValue + maxbinValue) / 2; // calculating peak value
    return peak;
}

double calc_Mean(vector<double> v)
{
    double sum = accumulate(v.begin(), v.end(), 0.0);
    double mean = -999.9;
    mean = sum / v.size();
    // cout << "mean : " << mean << endl;
    return mean;
}

double calu_SgmMean(TH1F *h, vector<vector<double>> v, int w, vector<bool> flag, bool use_filter = true, bool select_frag = true)
{
    double mean = h->GetMean();
    double sigma = h->GetStdDev();

    double min = mean - w * sigma;
    double max = mean + w * sigma;

    // vector<double> used_value;
    double sum = 0.;
    int count = 0;

    for (int itrk = 0; itrk < v.size(); itrk++)
    {
        if (use_filter && flag[itrk] != select_frag)
            continue;
        for (int idcaz = 0; idcaz < v[itrk].size(); idcaz++)
        {
            if (v[itrk][idcaz] >= min && v[itrk][idcaz] <= max)
            {
                // used_value.push_back(v[itrk]);
                sum += v[itrk][idcaz];
                count++;
            }
        }
    }

    // double sum = accumulate(used_value.begin(), used_value.end(), 0.0);

    double sgmmean = -999.9;
    // sgmmean = sum / used_value.size();
    sgmmean = sum / count;
    // cout << Form("%dsgm mean : ", w) << sgmmean << endl;
    if (isnan(sgmmean))
        sgmmean = -999.9;

    return sgmmean;
}

double calu_SgmMean_old(TH1F *h, vector<double> v, int w)
{
    double mean = h->GetMean();
    double sigma = h->GetStdDev();

    double min = mean - w * sigma;
    double max = mean + w * sigma;

    double sum = 0.;
    int count = 0;

    for (int itrk = 0; itrk < v.size(); itrk++)
    {
        if (v[itrk] >= min && v[itrk] <= max)
        {
            // used_value.push_back(v[itrk]);
            sum += v[itrk];
            count++;
        }
    }
    double sgmmean = -999.9;

    sgmmean = sum / count;
    if (isnan(sgmmean))
        sgmmean = -999.9;
    // cout << Form("%dsgm mean : ", w) << sgmmean << endl;

    return sgmmean;
}

vector<double> calc_ErrRange(double minRange, double maxRange, double center, bool useSqrt12 = false)
{
    double step = 0.05; // modified from 0.05 at 20250611
    vector<double> values;

    if (useSqrt12)
    {
        double rangeWidth = maxRange - minRange;
        double newWidth = rangeWidth / sqrt(12);

        minRange = center - newWidth / 2.0;
        maxRange = center + newWidth / 2.0;
        // cout << "--- Sqrt12 ---" << endl;
    }
    // else
    //     cout << "--- full ---" << endl;

    for (double value = minRange; value <= maxRange + step; value += step)
    {
        values.push_back(value);
        // hist->Fill(value);
        // cout << value << endl;
    }

    return values;
}

TH1F *fill_ErrRange(vector<double> values)
{
    TH1F *hist = new TH1F("hist", "hist", 8000, -200, 200);
    for (int nbin = 0; nbin < values.size(); nbin++)
    {
        hist->Fill(values[nbin]);
        // cout << value << endl;
    }
    return hist;
}

void get_zvtx(bool debug = false, int run_nu = 1, bool zvtx_reso = false)
{
    clock_t start_all = clock();

    int sigma = 3;
    int nloop = 100000;
    int BCO = 100;
    // bool mag_on = false;
    bool mag_on = true;
    // bool geant = false;
    bool geant = true;
    bool sr = false;
    int f_nu = 1;
    // bool zvtx_reso = false;
    // double ang = 0.02;

    // if (run_nu == 1)
    // {
    //     geant = true;
    //     // double ang = 0.02;
    // }

    // bool does_reverse = false;

    int method = 1;
    int xsize = 100;
    int binw = 2;

    // string zv_tit[12] = {
    //     "Peak",
    //     "Mean",
    //     "3sigmaMean",
    //     "1sigmaMean",
    //     "Peak_error",
    //     "Mean_error",
    //     "3sigmaMean_error",
    //     "1sigmaMean_error",
    //     "Peak_error_sq12",
    //     "Mean_error_sq12",
    //     "3sigmaMean_error_sq12",
    //     "1sigmaMean_error_sq12",
    // };
    // does_reverse = true;

    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/inttana_%d.root", run_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/RUN24_auau/inttana_%d_00%d.root", run_nu, f_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/RUN24_auau/inttana_%d_0%d.root", run_nu, f_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25_preparation/inttana_%d.root", run_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25/inttana_%d.root", run_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25/test3.root");
    string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25/mbd/inttana_%d_mbd_100000evt.root", run_nu); // run25 with MBD
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run24/mbd/inttana_%d_mbd_100000evt.root", run_nu); // run 24 with MBD
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25/inttana_%d_trigger.root", run_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25/inttana_%d_10000evt.root", run_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25/inttana_%d_mbd_1000evt.root", run_nu);
    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/run25/inttana_%d_10k.root", run_nu);
    // string fname = Form("/Users/ikemotomahiro/SDCC/datamacro/inttana_%d.root", run_nu);

    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/readdst/AnaTutorial_inttana_%d.root", run_nu);
    // if (mag_on)
    //     fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/readdst/AnaTutorial_inttana_%d_mag.root", run_nu);

    if (sr)
        // fname = Form("/Users/ikemotomahiro/SDCC/datamacro/streaming_readout/ana/inttana_%d_%d.root", run_nu, BCO);
        // fname = Form("sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/streaming_readout/ana/inttana_%d_%d.root", run_nu, BCO);
        // fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/streaming_readout/ana/inttana_%d_%d.root", run_nu, BCO);x
        fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/streaming_readout/ana/inttana_%d_50.root", run_nu);

    if (geant)
    {
        if (mag_on)
        {
            // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_1K.root";
            // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_zvtxwidth10_ptmin0.root";
            // fname = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/MC/F4AInttRead/macro/macros/detectors/sPHENIX/DATA/DATA_100K_PYTHIA_BON_zvtx_within_52cm/merged_100K_PYTHIA_BON_zvtx_within_52cm.root";
            fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/mbd/inttana_0000000019-0%d.root", run_nu); // auau sim

            // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_geant_10K_bon_lowpt5_minus.root";
        }

        else
            // fname = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/inttana_sim_nomg.root";
            // fname = "/Users/ikemotomahiro/SDCC/datamacro/inttana_sim_nomg.root";
            fname = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/AnaTutorial_inttana_10K_zvtx20.root";
        // fname = "/sphenix/u/shishikura/work/work/shishikura/detection_efficiency/simulation/makeClusterTree/result/inttana_zero_pp_vertex.root";
        // fname = "/Users/ikemotomahiro/SDCC/zvtx/result/zvtx_sim_nomg.root";
    }

    TFile *f = TFile::Open(fname.c_str());
    cout << "opened file : " << fname << endl;

    // string dir_out = "result_tracking";
    TString fname_out = fname.substr(fname.find_last_of("/"), fname.size());
    // fname_out.ReplaceAll("AnaTutorial_inttana", "tracking");
    fname_out.ReplaceAll("inttana", "zvtx"); // aaa;;
    fname_out.ReplaceAll("test3", "test3_zvtx");
    // fname_out.ReplaceAll(".root", Form("_ang%f.root", ang));

    // if (does_reverse)
    //     fname_out.ReplaceAll(".root", "_reverse.root");
    if (debug)
        fname_out.ReplaceAll(".root", "_debug.root");

    // fname_out.ReplaceAll(".root", "_test.root");

    if (zvtx_reso)
        // fname_out = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/zvtx/result_zvtx/run25" + fname_out; // run25
        fname_out = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/zvtx/result_zvtx/sim" + fname_out; // sim
    // fname_out = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/zvtx/result_zvtx" + fname_out;
    else
        // fname_out = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/zvtx/result_zvtx/run25/only" + fname_out;
        // fname_out = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/zvtx/result_zvtx/run24/only" + fname_out; // run24
        fname_out = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/zvtx/result_zvtx/sim/only" + fname_out; // sim
    // fname_out = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/zvtx/result_zvtx/only" + fname_out;

    cout << "out put file : " << fname_out << endl;

    TH1F *h_dcaz_one_ = new TH1F("h_dcaz_one_", "", 100, -200, 200);

    TFile *froot = new TFile(fname_out, "recreate");
    TH1F *h_nclus = new TH1F("h_nclus", "", 100, 0, 100);

    TTree *temp_tree_ = new TTree("temp_tree_", "temp_tree_");
    TTree *main_tree = new TTree("main_tree", "main_tree");
    // TTree *clus_tree = new TTree("clus_tree", "clus_tree"); // with tracking
    TTree *clus_tree_ = new TTree("clus_tree_", "clus_tree_"); // without tracking
    // TTree *truth_tree = new TTree("truth_tree", "truth_tree");
    // TTree *track_tree = new TTree("track_tree", "track_tree"); // with tracking
    // TTree *track_tree_ = new TTree("track_tree_", "track_tree_"); // without

    h_dphi_nocut = new TH2F("h_dphi_nocut", "d_phi : phi_in;phi_in;d_phi", 200, -3.2, 3.2, 200, -0.1, 0.1);
    h_dphi_cut = new TH2F("h_dphi_cut", "d_phi : phi_in;phi_in;d_phi", 200, -3.2, 3.2, 200, -0.1, 0.1);
    h_dtheta_phicut = new TH2F("h_dtheta_phicut", "#theta_{outer} - #theta_{inner} [rad] {from the beam spot};#theta_{inner} [rad];#theta_{outer} - #theta_{inner} [rad]", 200, 0, 3.2, 200, -1.0, 1.0 /*-3.2, 3.2*/);

    TH1F *h_dcaz_sigma_one = new TH1F("h_dcaz_sigma_one", "h_dcaz_sigma_one;dcaz sigma of one event;entries", 100, 0, 15);
    TH1D *h_xvtx = new TH1D("h_xvtx", "h_xvtx", 100, -0.01, 0.01);
    TH1D *h_yvtx = new TH1D("h_yvtx", "h_yvtx", 100, -0.01, 0.01);
    // TH1D *h_zvtx = new TH1D("h_zvtx", "h_zvtx", 100, -20, 20);
    // TH2D *h_zr[2];

    // TH1F *h_dcaz_ = new TH1F("h_dcaz_", "dcaz distribution", 200, -200, 200);
    // TH1F *h_dcaz_err = new TH1F("h_dcaz_err", "dcaz with all range distribution", 8000, -200, 200);
    TH1F *h_dcaz_err_sq12 = new TH1F("h_dcaz_err_sq12", "dcaz with error range distribution", 8000, -200, 200);
    TH1F *h_dcaz_err_sq12_west = new TH1F("h_dcaz_err_sq12_west", "dcaz with error range distribution (x_in > 0)", 8000, -200, 200);
    TH1F *h_dcaz_err_sq12_east = new TH1F("h_dcaz_err_sq12_east", "dcaz with error range distribution (x_in < 0)", 8000, -200, 200);
    TH1F *h_dcaz_err_sq12_top = new TH1F("h_dcaz_err_sq12_top", "dcaz with error range distribution (y_in < 0)", 8000, -200, 200);
    TH1F *h_dcaz_err_sq12_bottom = new TH1F("h_dcaz_err_sq12_bottom", "dcaz with error range distribution (y_in < 0)", 8000, -200, 200);
    TH1F *h_zvtx = new TH1F("h_zvtx", Form("z_vertex distribution (by INTT);z_{vtx}^{INTT} [cm];Counts / %d cm", binw), xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_mbd = new TH1F("h_zvtx_mbd", Form("z_vertex distribution (by MBD);z_{vtx}^{MBD} [cm];Counts / %d cm", binw), xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_test = new TH1F("h_zvtx_test", Form("z_vertex distribution (mean method, just test);z_{vtx}^{INTT} [cm];Counts / %d cm", binw), xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_test_1sgm = new TH1F("h_zvtx_test_1sgm", Form("z_vertex distribution (1sigm mean method, just test);z_{vtx}^{INTT} [cm];Counts / %d cm", binw), xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_diff = new TH1F("h_diff_truth_reco", Form("truth zvtx - reco zvtx;z_{vtx}^{INTT} [cm];Counts / %d cm", binw), xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_west = new TH1F("h_zvtx_west", "zvtx (west side)", xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_east = new TH1F("h_zvtx_east", "zvtx (east side)", xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_east_west = new TH1F("h_zvtx_east_west", "zvtx (east - west)", 200 * 2 / 0.5, -200, 200);
    TH1F *h_zvtx_top = new TH1F("h_zvtx_top", "zvtx (top side)", xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_bottom = new TH1F("h_zvtx_bottom", "zvtx (bottom side)", xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_top_bottom = new TH1F("h_zvtx_top_bottom", "zvtx (top - bottom)", 200 * 2 / 0.5, -200, 200);
    TH1F *h_zvtx_truth = new TH1F("h_zvtx_truth", "truth zvtx", xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_CW = new TH1F("h_zvtx_CW", "zvtx by CW", xsize * 2 / binw, -xsize, xsize);
    TH2F *h_truth_vs_reco = new TH2F("h_truth_vs_reco", "truth zvtx vs reco zvtx;truth zvtx [cm];reco zvtx [cm]", xsize * 2 / binw, -xsize, xsize, xsize * 2 / binw, -xsize, xsize);
    TH2F *h2_zvtx_intt_mbd = new TH2F("h2_zvtx_intt_mbd", "INTT zvtx vs MBD zvtx;INTT zvtx [cm];MBD zvtx [cm]", xsize * 2 / binw, -xsize, xsize, xsize * 2 / binw, -xsize, xsize);
    // for (int i = 0; i < 2; i++)
    //     h_zr[i] = new TH2D(Form("h_zr_%d", i), "h_zr;z;r", 100, -20, 20, 100, -10, 10);
    // for (int l = 0; l < method; l++)
    // {
    //     string histName = "hist_" + zv_tit[l];
    //     string histTitle = zv_tit[l];
    //     h_zvtx[l] = new TH1F(histName.c_str(), histTitle.c_str(), xsize * 2 / binw, -xsize, xsize);
    // }

    // for (int l = 0; l < method; l++)
    // {
    //     string histName = "hist_diff_" + zv_tit[l];
    //     string histTitle = "truth - " + zv_tit[l];
    //     h_zvtx_diff[l] = new TH1F(histName.c_str(), histTitle.c_str(), xsize * 2 / binw, -xsize, xsize);
    // }

    // for (int l = 0; l < method; l++)
    // {
    //     string histName = "hist_truth_vs_" + zv_tit[l];
    //     string histTitle = "truth vs " + zv_tit[l];
    //     h_truth_vs_reco[l] = new TH2F(histName.c_str(), histTitle.c_str(), xsize * 2 / binw, -xsize, xsize, xsize * 2 / binw, -xsize, xsize);
    // }

    c = new TCanvas(fname_out.ReplaceAll(".root", ".pdf"), "c", 1200, 1200);
    c->Divide(2, 2);
    c->cd(1);
    c->Print(((string)c->GetName() + "[").c_str());

    int evt_temp_;
    vector<double> x_in_;
    vector<double> y_in_;
    vector<double> z_in_;
    vector<double> x_out_;
    vector<double> y_out_;
    vector<double> z_out_;
    vector<double> d_phi_;
    vector<double> d_theta_;
    vector<double> phi_in_;
    vector<double> dca_x_;
    vector<double> dca_y_;
    vector<double> dcaz_;
    vector<double> dcaz_l;
    vector<double> dcaz_r;
    // vector<double> dcaz_err;
    // vector<double> dcaz_err_sq12;
    vector<vector<double>> dcaz_err_sq12;
    // vector<double> dcaz_err_sq12_west;
    // vector<double> dcaz_err_sq12_east;
    vector<bool> is_west; // true = west of INTT, false = east
    vector<bool> is_top;  // true = top pf INTT, false = bottom
    vector<double> dca_2d_;
    double zvtx_one_;
    double zvtx_sigma_one_;
    vector<int> ladnum_in;
    vector<int> ladnum_out;

    temp_tree_->Branch("evt_temp_", &evt_temp_, "evt_temp_/I");
    temp_tree_->Branch("x_in_", &x_in_); // cluster with tracking
    temp_tree_->Branch("y_in_", &y_in_);
    temp_tree_->Branch("z_in_", &z_in_);
    temp_tree_->Branch("x_out_", &x_out_);
    temp_tree_->Branch("y_out_", &y_out_);
    temp_tree_->Branch("z_out_", &z_out_);
    temp_tree_->Branch("d_phi_", &d_phi_);
    temp_tree_->Branch("d_theta_", &d_theta_);
    temp_tree_->Branch("phi_in_", &phi_in_);
    temp_tree_->Branch("dca_x_", &dca_x_);
    temp_tree_->Branch("dca_y_", &dca_y_);
    temp_tree_->Branch("dcaz_", &dcaz_);
    temp_tree_->Branch("dca_2d_", &dca_2d_);
    temp_tree_->Branch("zvtx_one_", &zvtx_one_, "zvtx_one_/D");
    temp_tree_->Branch("zvtx_sigma_one_", &zvtx_sigma_one_, "zvtx_sigma_one_/D");
    temp_tree_->Branch("ladnum_in", &ladnum_in);
    temp_tree_->Branch("ladnum_out", &ladnum_out);

    int nevent;
    int ntrack;
    int ncluster;
    double zvtx_CW;
    double truthzvtx;
    // double zv_peak;
    // double zv_mean;
    // double zv_mean_3sgm;
    // double zv_mean_1sgm;
    // double zv_peak_err;
    // double zv_mean_err;
    // double zv_mean_3sgm_err;
    // double zv_mean_1sgm_err;
    // double zv_peak_err_sq12;
    // double zv_mean_err_sq12;
    // double zv_mean_3sgm_err_sq12;
    double zv_intt;
    double zv_mbd;
    double zv_intt_west;
    double zv_intt_east;
    double zv_intt_top;
    double zv_intt_bottom;

    main_tree->Branch("nevent", &nevent, "nevent/I");
    main_tree->Branch("ntrack", &ntrack, "ntrack/I");
    main_tree->Branch("ncluster", &ncluster, "ncluster/I");
    main_tree->Branch("zvtx_CW", &zvtx_CW, "zvtx_CW/D");
    main_tree->Branch("truthzvtx", &truthzvtx, "truthzvtx/D");
    main_tree->Branch("zv_intt", &zv_intt, "zv_intt/D");
    main_tree->Branch("zv_mbd", &zv_mbd, "zv_mbd/D");
    if (zvtx_reso)
    {
        main_tree->Branch("zv_intt_west", &zv_intt_west, "zv_intt_west/D");
        main_tree->Branch("zv_intt_east", &zv_intt_east, "zv_intt_east/D");
        main_tree->Branch("zv_intt_top", &zv_intt_top, "zv_intt_top/D");
        main_tree->Branch("zv_intt_bottom", &zv_intt_bottom, "zv_intt_bottom/D");
    }

    int evt_all;
    int ntrack_all;
    vector<double> x_in;
    vector<double> y_in;
    vector<double> z_in;
    vector<double> r_in;
    vector<double> phi_in;
    vector<double> theta_in;
    vector<double> x_out;
    vector<double> y_out;
    vector<double> z_out;
    vector<double> r_out;
    vector<double> phi_out;
    vector<double> theta_out;

    // cluster tree with tracking
    // clus_tree->Branch("evt_clus", &evt_clus, "evt_clus/I");
    // clus_tree->Branch("x_in", &x_in);
    // clus_tree->Branch("y_in", &y_in);
    // clus_tree->Branch("z_in", &z_in);
    // clus_tree->Branch("r_in", &r_in);
    // clus_tree->Branch("phi_in", &phi_in);
    // clus_tree->Branch("theta_in", &theta_in);
    // clus_tree->Branch("x_out", &x_out);
    // clus_tree->Branch("y_out", &y_out);
    // clus_tree->Branch("z_out", &z_out);
    // clus_tree->Branch("r_out", &r_out);
    // clus_tree->Branch("phi_out", &phi_out);
    // clus_tree->Branch("theta_out", &theta_out);

    // cluster tree without tracking
    clus_tree_->Branch("evt_all", &evt_all, "evt_all/I");
    clus_tree_->Branch("ntrack_all", &ntrack_all, "ntrack_all/I");
    clus_tree_->Branch("x_in", &x_in);
    clus_tree_->Branch("y_in", &y_in);
    clus_tree_->Branch("z_in", &z_in);
    clus_tree_->Branch("r_in", &r_in);
    clus_tree_->Branch("phi_in", &phi_in);
    clus_tree_->Branch("theta_in", &theta_in);
    clus_tree_->Branch("x_out", &x_out);
    clus_tree_->Branch("y_out", &y_out);
    clus_tree_->Branch("z_out", &z_out);
    clus_tree_->Branch("r_out", &r_out);
    clus_tree_->Branch("phi_out", &phi_out);
    clus_tree_->Branch("theta_out", &theta_out);

    // int evt_track;
    // int ntrack = 0;
    // vector<double> slope_rz;
    // vector<double> slope_xy;
    // vector<double> phi_tracklets;
    // vector<double> theta_tracklets;
    // vector<double> phi_track;
    // vector<double> theta_track;
    // vector<double> z_tracklets_out;
    // vector<double> dca_2d;
    // vector<double> dca_z;
    // vector<int> is_deleted;
    // vector<int> dca_range_out;
    // vector<int> dca2d_range_out;
    // vector<int> dcaz_range_out;
    // vector<double> pT;
    // vector<double> px_truth;
    // vector<double> py_truth;
    // vector<double> pz_truth;
    // vector<double> px;
    // vector<double> py;
    // vector<double> pz;
    // vector<double> rad;
    // vector<double> pT_truth;
    // vector<double> charge;
    // double x_vertex;
    // double y_vertex;
    // double z_vertex;

    // track tree with tracking
    /*track_tree->Branch("evt_track", &evt_track, "evt_track/I");
    track_tree->Branch("ntrack", &ntrack, "ntrack/I");
    track_tree->Branch("phi_tracklets", &phi_tracklets);
    track_tree->Branch("slope_rz", &slope_rz);
    track_tree->Branch("slope_xy", &slope_xy);
    track_tree->Branch("theta_tracklets", &theta_tracklets);
    track_tree->Branch("phi_track", &phi_track);
    track_tree->Branch("theta_track", &theta_track);
    track_tree->Branch("z_tracklets_out", &z_tracklets_out);
    track_tree->Branch("dca_2d", &dca_2d);
    track_tree->Branch("dca_z", &dca_z);
    track_tree->Branch("is_deleted", &is_deleted);
    track_tree->Branch("dca_range_out", &dca_range_out);
    track_tree->Branch("dcaz_range_out", &dcaz_range_out);
    track_tree->Branch("dca2d_range_out", &dca2d_range_out);
    track_tree->Branch("pT", &pT);
    track_tree->Branch("px", &px);
    track_tree->Branch("py", &py);
    track_tree->Branch("pz", &pz);
    track_tree->Branch("px_truth", &px_truth);
    track_tree->Branch("py_truth", &py_truth);
    track_tree->Branch("pz_truth", &pz_truth);
    track_tree->Branch("pT_truth", &pT_truth);
    track_tree->Branch("charge", &charge);
    track_tree->Branch("rad", &rad);
    track_tree->Branch("x_vertex", &x_vertex, "x_vertex/D");
    track_tree->Branch("y_vertex", &y_vertex, "y_vertex/D");
    track_tree->Branch("z_vertex", &z_vertex, "z_vertex/D");*/

    // track tree without tracking
    // track_tree_->Branch("evt_track", &evt_track, "evt_track/I");
    // track_tree_->Branch("ntrack", &ntrack, "ntrack/I");
    // track_tree_->Branch("phi_tracklets", &phi_tracklets);
    // track_tree_->Branch("slope_rz", &slope_rz);
    // track_tree_->Branch("slope_xy", &slope_xy);
    // track_tree_->Branch("theta_tracklets", &theta_tracklets);
    // track_tree_->Branch("phi_track", &phi_track);
    // track_tree_->Branch("theta_track", &theta_track);
    // track_tree_->Branch("z_tracklets_out", &z_tracklets_out);
    // track_tree_->Branch("dca_2d", &dca_2d);
    // track_tree_->Branch("dca_z", &dca_z);
    // track_tree_->Branch("is_deleted", &is_deleted);
    // track_tree_->Branch("dca_range_out", &dca_range_out);
    // track_tree_->Branch("dcaz_range_out", &dcaz_range_out);
    // track_tree_->Branch("dca2d_range_out", &dca2d_range_out);
    // track_tree_->Branch("pT", &pT);
    // track_tree_->Branch("px", &px);
    // track_tree_->Branch("py", &py);
    // track_tree_->Branch("pz", &pz);
    // track_tree_->Branch("px_truth", &px_truth);
    // track_tree_->Branch("py_truth", &py_truth);
    // track_tree_->Branch("pz_truth", &pz_truth);
    // track_tree_->Branch("pT_truth", &pT_truth);
    // track_tree_->Branch("charge", &charge);
    // track_tree_->Branch("rad", &rad);
    // track_tree_->Branch("x_vertex", &x_vertex, "x_vertex/D");
    // track_tree_->Branch("y_vertex", &y_vertex, "y_vertex/D");
    // track_tree_->Branch("z_vertex", &z_vertex, "z_vertex/D");

    // from input file
    TNtuple *ntp_clus = (TNtuple *)f->Get("ntp_clus");
    TNtuple *ntp_evt = (TNtuple *)f->Get("ntp_evt");
    // TTree *hepmctree = (TTree *)f->Get("hepmctree");
    // bool no_mc = (hepmctree->GetEntries() == 0);

    Float_t nclus, nclus2, bco_full, evt, size, adc, x, y, z, lay, lad, sen, /*z_vtxsim,*/ zv, truthzv;
    // Float_t nclus, nclus2, bco_full, evt, size, adc, x, y, z, lay, lad, sen, zvtx;

    ntp_clus->SetBranchAddress("nclus", &nclus);
    ntp_clus->SetBranchAddress("nclus2", &nclus2);
    ntp_clus->SetBranchAddress("bco_full", &bco_full);
    ntp_clus->SetBranchAddress("evt", &evt);
    ntp_clus->SetBranchAddress("size", &size);
    ntp_clus->SetBranchAddress("adc", &adc);
    ntp_clus->SetBranchAddress("x", &x);
    ntp_clus->SetBranchAddress("y", &y);
    ntp_clus->SetBranchAddress("z", &z);
    ntp_clus->SetBranchAddress("lay", &lay);
    ntp_clus->SetBranchAddress("lad", &lad);
    ntp_clus->SetBranchAddress("sen", &sen);
    // if (geant)
    // ntp_clus->SetBranchAddress("z_vtxsim", &z_vtxsim);
    // ntp_clus->SetBranchAddress("zvtx", &zvtx);
    // ntp_clus->SetBranchAddress("x_vtx", &x_vtx);
    // ntp_clus->SetBranchAddress("y_vtx", &y_vtx);
    ntp_clus->SetBranchAddress("zv", &zv);
    ntp_clus->SetBranchAddress("truthzv", &truthzv);

    if (!(geant))
    {
        Float_t zvsim, zv;
        ntp_evt->SetBranchAddress("zv", &zv);
        ntp_evt->SetBranchAddress("zvsim", &zvsim);
    }

    int sum_event = 0;
    int sum_event_all = 0;
    int ntrack_ = 0;
    int ncut = 0;
    clock_t before_tracking = clock();

    cout << "# of entries (ntp_clus) : " << ntp_clus->GetEntries() << endl;

    for (int icls = 0; icls < ntp_clus->GetEntries(); icls++)
    {
        cout << Form("  -----  event %d {event gene.}  -----  ", sum_event) << endl;

        clustEvent event{};
        cluster clust{};

        ntp_clus->GetEntry(icls); // first cluster in one event
        ntp_evt->GetEntry(sum_event);
        clust.set(evt, 0, x, y, z, adc, size, lay, lad, sen);
        event.vclus.push_back(clust);
        sum_event_all++;
        ntrack_all = event.vtrack.size();

        // h_dcaz_->Reset();
        // h_dcaz_err->Reset();
        h_dcaz_err_sq12->Reset();
        h_dcaz_err_sq12_west->Reset();
        h_dcaz_err_sq12_east->Reset();
        h_dcaz_err_sq12_top->Reset();
        h_dcaz_err_sq12_bottom->Reset();

        for (int j = 1; j < nclus; j++) // second~ cluster in one event
        {
            ntp_clus->GetEntry(++icls);
            cluster clust2{};
            clust2.set(evt, 0, x, y, z, adc, size, lay, lad, sen);
            event.vclus.push_back(clust2);
        }

        if (!(geant))
        {
            event.dca_mean[0] = -0.019; // run24 pp xvertex
            event.dca_mean[1] = 0.198;  // run24 pp yvertex
            // event.dca_mean[0] = -0.02207; // xvertex from dN/dη run 54280
            // event.dca_mean[1] = 0.2230;   // yvertex from dN/dη run 54280
            // event.dca_mean[1] = 0.1125; // yvertex/2 from dN/dη run 54280
            // cout << "xvtx : " << event.dca_mean[0] << endl;
            // event.dca_mean[0] = 0.;
            // event.dca_mean[1] = 0.;
            // event.dca_mean[0] = -0.0857; // run66522 x
            // event.dca_mean[1] = 0.1349;  // run66522 y
        }
        //
        // if (event.vclus.size() <= 4 || event.vclus.size() >= 180)
        // if (event.vclus.size() <= 4 || event.vclus.size() >= 2000)
        if (event.vclus.size() <= 4)
        // if (event.vclus.size() <= 11)
        {
            ncut++;
            continue;
        }
        if (debug && event.vclus.size() >= 2000)
        {
            ncut++;
            continue;
        }

        cout << "ncluster : " << event.vclus.size() << endl;
        ncluster = event.vclus.size();

        clock_t start_tracking = clock();
        // cout << "before tracking time = " << (double)(start_tracking - before_tracking) / CLOCKS_PER_SEC << "sec.\n";

        dotracking(event);
        clock_t end_tracking = clock();
        // cout << "tracking time = " << (double)(end_tracking - start_tracking) / CLOCKS_PER_SEC << "sec.\n";

        ntrack_ = event.vtrack.size();
        ntrack = ntrack_;
        h_dcaz_one_->Reset();

        for (int itrk = 0; itrk < ntrack_; itrk++)
        {
            h_dcaz_one_->Fill(event.vtrack[itrk]->dca[2]);
        }
        zvtx_one_ = h_dcaz_one_->GetMean();
        zvtx_sigma_one_ = h_dcaz_one_->GetStdDev();

        clock_t start_dcaz = clock();
        evt_temp_ = sum_event;

        for (int itrk = 0; itrk < ntrack_; itrk++)
        {
            x_in_.push_back(event.vtrack[itrk]->p1.x());
            // cout << "x_in_ : " << x_in_[itrk] << endl;
            y_in_.push_back(event.vtrack[itrk]->p1.y());
            dca_x_.push_back(event.vtrack[itrk]->dca[0]);
            dca_y_.push_back(event.vtrack[itrk]->dca[1]);
            dcaz_.push_back(event.vtrack[itrk]->dca[2]);
            dcaz_l.push_back(event.vtrack[itrk]->dca[3]);
            dcaz_r.push_back(event.vtrack[itrk]->dca[4]);
            dca_2d_.push_back(event.vtrack[itrk]->dca_2d);
            d_phi_.push_back(event.vtrack[itrk]->d_phi);
            d_theta_.push_back(event.vtrack[itrk]->d_theta);
            phi_in_.push_back(event.vtrack[itrk]->phi_in);
            ladnum_in.push_back(event.vtrack[itrk]->ladder1);
            ladnum_out.push_back(event.vtrack[itrk]->ladder2);

            // ---putting dcaz (no err range)---
            // h_dcaz_->Fill(dcaz_[itrk]);

            // ---caluculating all err range---
            // vector<double> errRange = calc_ErrRange(dcaz_l[itrk], dcaz_r[itrk], dcaz_[itrk], false);
            // dcaz_err.insert(dcaz_err.end(), errRange.begin(), errRange.end());
            // TH1F *Hist_err = fill_ErrRange(errRange);
            // h_dcaz_err->Add(Hist_err);
            // delete Hist_err;
            // errRange.clear();

            // cout << "left : " << dcaz_l[j] << "  dcaz : " << dcaz[j] << "  right : " << dcaz_r[j] << endl;
            // ---caluculating err range/sqrt(12) ---
            vector<double> errRangeSq12 = calc_ErrRange(dcaz_l[itrk], dcaz_r[itrk], dcaz_[itrk], true);
            // dcaz_err_sq12.insert(dcaz_err_sq12.end(), errRangeSq12.begin(), errRangeSq12.end());

            dcaz_err_sq12.push_back(errRangeSq12);

            // TH1F *Hist_err_sq12 = fill_ErrRange(errRangeSq12);
            for (int nbin = 0; nbin < errRangeSq12.size(); nbin++)
            {
                // h_dcaz_err_sq12->Fill(Hist_err_sq12);
                h_dcaz_err_sq12->Fill(errRangeSq12[nbin]);
            }

            // cout << "^^b " << endl;

            if (zvtx_reso)
            {
                is_west.push_back(x_in_[itrk] > 0);
                is_top.push_back(y_in_[itrk] > 0);

                if (x_in_[itrk] > 0)
                {
                    for (int nbin = 0; nbin < errRangeSq12.size(); nbin++)
                        h_dcaz_err_sq12_west->Fill(errRangeSq12[nbin]);
                }
                else
                {
                    for (int nbin = 0; nbin < errRangeSq12.size(); nbin++)
                        h_dcaz_err_sq12_east->Fill(errRangeSq12[nbin]);
                }

                if (y_in_[itrk] > 0)
                {
                    for (int nbin = 0; nbin < errRangeSq12.size(); nbin++)
                        h_dcaz_err_sq12_top->Fill(errRangeSq12[nbin]);
                }
                else
                {
                    for (int nbin = 0; nbin < errRangeSq12.size(); nbin++)
                        h_dcaz_err_sq12_bottom->Fill(errRangeSq12[nbin]);
                }
            }

            // delete Hist_err_sq12;
            errRangeSq12.clear();
        }
        clock_t end_dcaz = clock();

        // cout << "dcaz time = " << (double)(end_dcaz - start_dcaz) / CLOCKS_PER_SEC << "sec.\n";
        //
        // for (int nbin = 0; nbin < dcaz_err_sq12.size(); nbin++)
        //     h_dcaz_err_sq12_west->Fill(dcaz_err_sq12[nbin]);

        // zv_peak = calc_Peak(h_dcaz_);
        // zv_mean = calc_Mean(dcaz_);
        // zv_mean_3sgm = calu_SgmMean(h_dcaz_, dcaz_, 3);
        // zv_mean_1sgm = calu_SgmMean_old(h_dcaz_, dcaz_, 1);

        // zv_peak_err = calc_Peak(h_dcaz_err);
        // zv_mean_err = calc_Mean(dcaz_err);
        // zv_mean_3sgm_err = calu_SgmMean(h_dcaz_err, dcaz_err, 3);
        // zv_mean_1sgm_err = calu_SgmMean(h_dcaz_err, dcaz_err, 1);

        // zv_peak_err_sq12 = calc_Peak(h_dcaz_err_sq12);
        // zv_mean_err_sq12 = calc_Mean(dcaz_err_sq12);
        // zv_mean_3sgm_err_sq12 = calu_SgmMean(h_dcaz_err_sq12, dcaz_err_sq12, 3);

        zv_intt = calu_SgmMean(h_dcaz_err_sq12, dcaz_err_sq12, 1, is_west, false);

        zv_mbd = zv; // MBD vertex
        if (abs(zv) < 200)
        {
            h2_zvtx_intt_mbd->Fill(zv_intt, zv_mbd);
            cout << "mbd arimasu!" << endl;
        }
        clock_t end_zvtx = clock();
        // cout << "zvtx time = " << (double)(end_zvtx - end_dcaz) / CLOCKS_PER_SEC << "sec.\n";
        if (zvtx_reso)
        {
            zv_intt_west = calu_SgmMean(h_dcaz_err_sq12_west, dcaz_err_sq12, 1, is_west, true, true);
            zv_intt_east = calu_SgmMean(h_dcaz_err_sq12_east, dcaz_err_sq12, 1, is_west, true, false);
            zv_intt_top = calu_SgmMean(h_dcaz_err_sq12_top, dcaz_err_sq12, 1, is_top, true, true);
            zv_intt_bottom = calu_SgmMean(h_dcaz_err_sq12_bottom, dcaz_err_sq12, 1, is_top, true, false);
        }
        clock_t end_zvtx_ew = clock();
        // cout << "zvtx east west time = " << (double)(end_zvtx_ew - end_zvtx) / CLOCKS_PER_SEC << "sec.\n";
        // cout << "size : " << dcaz_err_sq12.size() << endl;

        // double rzv[12] = {zv_peak, zv_mean, zv_mean_3sgm, zv_mean_1sgm, zv_peak_err, zv_mean_err, zv_mean_3sgm_err, zv_mean_1sgm_err, zv_peak_err_sq12, zv_mean_err_sq12, zv_mean_3sgm_err_sq12, zv_intt};

        if (geant)
            truthzvtx = truthzv;
        //     truthzvtx = z_vtxsim;
        // truthzvtx = zvtx;

        // if (isnan(zv_intt))
        // {
        //     zv_intt = -999.9;
        // }
        h_zvtx->Fill(zv_intt);
        // h_zvtx_test->Fill(zv_mean);
        // h_zvtx_test_1sgm->Fill(zv_mean_1sgm);

        // if (isnan(zv_intt_west))
        // {
        //     zv_intt_west = -999.9;
        // }
        if (zvtx_reso)
        {
            h_zvtx_west->Fill(zv_intt_west);
            h_zvtx_east->Fill(zv_intt_east);
            h_zvtx_east_west->Fill(zv_intt_east - zv_intt_west);

            h_zvtx_top->Fill(zv_intt_top);
            h_zvtx_bottom->Fill(zv_intt_bottom);
            h_zvtx_top_bottom->Fill(zv_intt_top - zv_intt_bottom);
        }

        if (geant)
        {
            h_zvtx_diff->Fill(truthzvtx - zv_intt);
            h_truth_vs_reco->Fill(truthzvtx, zv_intt);
            // cout << "zvtx_truth : " << z_vtxsim << endl;
            // cout << "zvtx_truth : " << zvtx << endl;
            cout << "zvtx_reco : " << zv_intt << endl;

            h_zvtx_truth->Fill(truthzvtx);
            // cout << "truth zvtx : " << truthzvtx << endl;
        }

        // // cluster tree without tracking
        for (unsigned int i = 0; i < event.vclus.size(); i++)
        {
            if (event.vclus[i].layer < 2)
            {
                x_in.push_back(event.vclus[i].x);
                // cout << "x_in : " << x_in[i] << endl;
                y_in.push_back(event.vclus[i].y);
                z_in.push_back(event.vclus[i].z);
                r_in.push_back(event.vclus[i].r);
                phi_in.push_back(event.vclus[i].getphi_clus());
                theta_in.push_back(event.vclus[i].gettheta_clus());
            }
            if (1 < event.vclus[i].layer)
            {
                x_out.push_back(event.vclus[i].x);
                y_out.push_back(event.vclus[i].y);
                z_out.push_back(event.vclus[i].z);
                r_out.push_back(event.vclus[i].r);
                phi_out.push_back(event.vclus[i].getphi_clus());
                theta_out.push_back(event.vclus[i].gettheta_clus());
            }
        }

        clock_t end_clustree = clock();

        temp_tree_->Fill();
        erase(x_in_);
        erase(y_in_);
        erase(z_in_);
        erase(x_out_);
        erase(y_out_);
        erase(z_out_);
        erase(dca_x_);
        erase(dca_y_);
        erase(dcaz_);
        erase(dcaz_l);
        erase(dcaz_r);
        erase(dca_2d_);
        erase(d_phi_);
        erase(d_theta_);
        erase(phi_in_);
        erase(is_west);
        erase(is_top);

        main_tree->Fill();
        erase(ladnum_in);
        erase(ladnum_out);
        // erase(dcaz_err);
        // erase(dcaz_err_sq12[icls]);
        dcaz_err_sq12.clear();

        // filling cluster tree without tracking
        clus_tree_->Fill();
        erase(x_in);
        erase(y_in);
        erase(z_in);
        erase(r_in);
        erase(phi_in);
        erase(theta_in);
        erase(x_out);
        erase(y_out);
        erase(z_out);
        erase(r_out);
        erase(phi_out);
        erase(theta_out);

        event.clear();
        sum_event++;
        // cout << sum_event << endl;
        clock_t end_loop = clock();
        // cout << "deleate tree time = " << (double)(end_loop - end_clustree) / CLOCKS_PER_SEC << "sec.\n";
        if (debug && sum_event >= nloop)
            break;
    }
    clock_t start_drawing = clock();

    nevent = sum_event;
    evt_all = sum_event_all;
    // cout << nevent << endl;
    cout << "used : " << sum_event << "  cut : " << ncut << endl;

    // cout << "div : " << h_zvtx_east_west->GetStdDev() << endl;

    n_dotracking++;
    TH1F *h_dcax = new TH1F("h_dcax", "h_dcax", 100, -0.4, 0.4);
    temp_tree_->Draw("dca_x_>>h_dcax");
    TH1F *h_dcay = new TH1F("h_dcay", "h_dcay", 100, -0.4, 0.4);
    temp_tree_->Draw("dca_y_>>h_dcay");
    TH1F *h_dcaz = new TH1F("h_dcaz", "h_dcaz;DCA_z[cm]", 60, -150, 150);
    temp_tree_->Draw("dcaz_>>h_dcaz");
    TH1F *h_dtheta = new TH1F("h_dtheta", "dtheta{inner - outer};dtheta;entries", 100, -3.2, 3.2);
    temp_tree_->Draw("d_theta_>>h_dtheta");
    TH1F *h_dphi = new TH1F("h_dphi", "#Delta #phi_{AB};#Delta #phi_{AB}", 100, -1, 1);
    temp_tree_->Draw("d_phi_>>h_dphi");

    TH1F *h_dca2d = new TH1F("h_dca2d", "h_dca", 100, -10, 10);
    temp_tree_->Draw("dca_2d_>>h_dca2d");

    vector<TH1F *> h_ = {h_dcax, h_dcay, h_dcaz, h_dphi, h_dtheta, h_dca2d};

    h_zvtx->Draw();

    if (zvtx_reso)
    {
        c->cd(2);
        h_zvtx_east_west->Draw();

        c->cd(3);
        h_zvtx_west->Draw();
        c->cd(4);
        h_zvtx_east->Draw();
    }
    c->Print(c->GetName());

    if (zvtx_reso)
    {
        c->cd(1);
        if (geant)
            h_zvtx_truth->Draw();
        else
            // h_zvtx_CW->Draw();
            h_zvtx_top_bottom->Draw();

        c->cd(2);
        h_zvtx->Draw();
        c->cd(3);
        h_zvtx_top->Draw();
        c->cd(4);
        h_zvtx_bottom->Draw();
        c->Print(c->GetName());
    }
    c->cd(1);
    h_dcaz_err_sq12->Draw();
    c->cd(2);
    // h_dcaz_err_sq12_west->Draw();
    h2_zvtx_intt_mbd->Draw();
    c->cd(3);
    h_zvtx_test_1sgm->Draw();
    // h_dcaz_err_sq12_east->Draw();
    c->cd(4);
    h_zvtx_test->Draw();
    c->Print(c->GetName());

    if (geant)
    {
        c->cd(1);
        h_zvtx_diff->Draw();
        c->cd(2);
        TF1 *f = new TF1("f", "pol1", -30, 30);
        TProfile *p = h_truth_vs_reco->ProfileX("p", 1, -1, "s");
        h_truth_vs_reco->Draw("colz");
        cout << "slope : " << f->GetParameter(1) << endl;
        c->Print(c->GetName());
    }

    c->Print(((string)c->GetName() + "]").c_str());

    clock_t end_drawing = clock();
    // cout << "drawing time = " << (double)(end_drawing - start_drawing) / CLOCKS_PER_SEC << "sec.\n";

    froot->Write();
    for (int ihst = 0; ihst < h_.size(); ihst++)
    {
        froot->WriteTObject(h_[ihst], h_[ihst]->GetName());
    }

    froot->Close();
    clock_t end_all = clock();
    // cout << "hist tree time = " << (double)(end_all - end_drawing) / CLOCKS_PER_SEC << "sec.\n";

    cout << "time = " << (double)(end_all - start_all) / CLOCKS_PER_SEC << "sec.\n";
    cout << "out put file : " << fname_out << endl;
}
