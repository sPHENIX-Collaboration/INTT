#pragma once

#include <iostream>
#include <iomanip>
#include <cstring>

#include <Math/Vector3D.h>
#include <Math/Cartesian2D.h>
#include <TEllipse.h>
#include <TLine.h>
#include <TMath.h>

#ifndef DRAW_INTT_LINE_NO_X // useful?
#include <TCanvas.h>
#include <TVirtualPad.h>
#endif //  DRAW_INTT_LINE_NO_X

namespace DrawInttLine
{
  int line_color = kGray;
  double line_color_alpha = 0.5;
  int line_width = 3;
  int tick_width = 2;
  const double kLayer_radii[4] = {7.1888, 7.800, 9.680, 10.330};
  const double kLadder_length = 0.0078 * 256;

  ROOT::Math::XYZVector vector_center( 0, 0, 0 ); // function is not implemented yet
  void HowTo()
  {

    std::vector < std::string > lines;
    lines.push_back( " How to use DrawInttLine function" );
    lines.push_back( "" );
    lines.push_back( " 1. You need to include this header:" );
    lines.push_back( "    #include \"DrawInttLine.hh\"" );
    lines.push_back( " 2. Execute the functon:" );
    lines.push_back( "    draw_intt(0); /* x-y plane*/" );
    lines.push_back( "     or" );
    lines.push_back( "    draw_intt(1); /* z-r plane*/" );
    lines.push_back( " 3. (optional) The classical function, which draws only outlines:" );
    lines.push_back( "    draw_intt_classical(0); /* x-y plane*/" );
    lines.push_back( "     or" );
    lines.push_back( "    draw_intt_classical(1); /* z-r plane*/" );
    lines.push_back( " 4. (optional) You can modify some parameters:" );
    lines.push_back( "    DrawInttLine::line_color : for line color (default: kRed)" );
    lines.push_back( "    DrawInttLine::line_color_alpha : for alpha channel (transparency) of line (default: 0.5)" );
    lines.push_back( "    DrawInttLine::line_width : for line width (default: 3)" );
    lines.push_back( "    DrawInttLine::tick_width : for tick width (default: 2)" );
    lines.push_back( " 5. (optional) Call DrawInttLine::HowTo() to see this help." );

    int max_length = 0;
    for( auto& line : lines )
      if( max_length < line.size() )
	max_length = line.size() ;

    std::cout << " +" << std::string( max_length, '-' ) << "+" << std::endl;
    for( auto& line : lines )
      std::cout << " |" << std::setfill( ' ' ) << std::setw( max_length ) << std::left << line << "|" << std::endl;
    std::cout << " +" << std::string( max_length, '-' ) << "+" << std::endl;
  }
  
}

void draw_intt_classical(int mode)
{

  if (mode == 0)
    {
      for (int i = 0; i < 4; i++)
        {
          auto circle = new TEllipse(0.0, 0.0, DrawInttLine::kLayer_radii[i]);
          circle->SetLineColorAlpha( DrawInttLine::line_color, DrawInttLine::line_color_alpha );
          circle->SetLineWidth( DrawInttLine::line_width );
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
              TLine *line = new TLine(-22.8, (2 * j - 1) * DrawInttLine::kLayer_radii[i], 22.8, (2 * j - 1) * DrawInttLine::kLayer_radii[i]);
	      line->SetLineColorAlpha( DrawInttLine::line_color, DrawInttLine::line_color_alpha );
              line->SetLineWidth( DrawInttLine::line_width );
              line->Draw("same");
            }
        }
    }
}

void draw_intt(int mode)
{

  const int kLadder_num[4] = { 12, 12, 16, 16 };
  const int is_phi_offset[4] = { 1, 0, 1, 0 };

  TLine* line_intt = new TLine();
  line_intt->SetLineWidth( DrawInttLine::line_width );
  line_intt->SetLineColorAlpha( DrawInttLine::line_color, DrawInttLine::line_color_alpha );
  
  if (mode == 0) // x-y plane
    {
      for (int i = 0; i < 4; i++) // for each layer, 0: Inner layer of inner barrel
        {

          double phi_pitch = 2 * TMath::Pi() / kLadder_num[i]; // phi difference b/w ladders
          double phi_offset = is_phi_offset[i] * phi_pitch / 2; // phi offset, (12.5/2, 0, 16, 0 )
          double phi = -phi_offset; // phi position of current ladder
          for( int ladder=0; ladder<kLadder_num[i]; ladder++ ) // all ladders in the current layer
            {
              phi += phi_pitch;


	      //////////////////////////////////
	      //                              //
	      //         * <-- point2         //
	      //          \                   //
	      //	   \                  //
	      //         /  \                 //
	      //        /    * <-- point1     //
	      //  vtx x  ) phi                //
	      //      |                       //
	      //      |                       //
	      //  *---+---*  <- ladder        //
	      //  |       |                   //
	      //  point1  point2              //
	      //                              //
	      //////////////////////////////////
	      
              ROOT::Math::Cartesian2D<double> point1( - DrawInttLine::kLadder_length / 2, DrawInttLine::kLayer_radii[i] );
              ROOT::Math::Cartesian2D<double> point2( DrawInttLine::kLadder_length / 2, DrawInttLine::kLayer_radii[i] );

	      point1.Rotate( phi );
	      point2.Rotate( phi );

	      line_intt->DrawLine( point1.X(), point1.Y(), point2.X(), point2.Y() );	      
            } // end of loop over all ladders
        } // end of loop over all layers
    } // end of x-y plane mode
  else if (mode == 1) // z-r mode
    {
      for (int j = 0; j < 2; j++) // loop over 2 cases: +y or -y
        {
          for (int i = 0; i < 4; i++) // loop over all layers
            {

              double y =  (2 * j - 1) * DrawInttLine::kLayer_radii[i]; // vertical (r) position of current ladder
              double current_x = 0.0;
              for( int chip=1; chip<14; chip++ ) // chip +/-1, +/-2, ...
                {

                  double tick_height = 0.2; // cm
                  double chip_length = 1.6; // cm
                  if( chip > 8 )
                    chip_length = 2.0; // cm

                  double xmin = current_x;
                  double xmax = current_x + chip_length;

                  for( auto side : {1, -1} )
                    {
                      line_intt->SetLineWidth( DrawInttLine::line_width );
                      line_intt->DrawLine( side * xmin, y, side * xmax, y ); // chip region with horizontal line
		      
                      line_intt->SetLineWidth( DrawInttLine::tick_width );
                      line_intt->DrawLine( side * xmin, y-tick_height, side * xmin, y+tick_height ); // beginning of chip with |
                      line_intt->DrawLine( side * xmax, y-tick_height, side * xmax, y+tick_height ); // end of chip with |
                    }

                  current_x += chip_length;
                } // end of loop over chips
              //break;
            } // end of loop over layers

        } // end of loop over +/-y

    } // end of z-r mode

}

#ifndef DRAW_INTT_LINE_NO_X

/*!
  @brief Sample function
  @details You can learn how to use draw_intt_line function
*/
int draw_intt_line()
{
  DrawInttLine::HowTo();
  TCanvas* c = new TCanvas( "name", "title", 1600, 800 ) ;
  c->Divide( 2, 1 );

  // You can change the line properties
  DrawInttLine::line_color = kBlue;
  DrawInttLine::line_width = 5;
  auto pad1 = c->cd(1)->DrawFrame( -15, -15, 15, 15 );
  draw_intt( 0 );

  DrawInttLine::line_color = kRed;
  DrawInttLine::line_width = 1;
  DrawInttLine::tick_width = 1;
  auto pad2 = c->cd(2)->DrawFrame( -25, -15, 25, 15 );
  draw_intt( 1 );

  return 0;
}

#endif //  DRAW_INTT_LINE_NO_X
