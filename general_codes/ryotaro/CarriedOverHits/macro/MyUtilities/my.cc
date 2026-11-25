int GetNCollisionFromRunNum(int run);
int GetOpenTimeFromRunNum(int run);

std::string GetDate(){
  //  return "5/13/2025"; // use this once the dete is determined
  TDatime dt;
  int year    = dt.GetYear();
  int month    = dt.GetMonth();
  int day    = dt.GetDay();

  // format: mm/dd/yyyy
  std::stringstream ss;
  ss << month << "/" << day << "/" << year;

  return ss.str();
}

void PutCaption(int run, double first_margin = -0.1, double pos_x = 0.2, double pos_y = 0.9){
/*from Genki*/
  TLatex* tex = new TLatex();
  double line_height = 0.055; // no need to change
  // double first_margin = -0.1; // the margin from the top of the canvas to the top line (date). You can modify
  // double pos_x = 0.2; // you can modify
  // double pos_y = 1.0  + first_margin; // - line_height;

  /* Date, you may want to change 0.65 to pos_x */
  //tex->DrawLatexNDC( pos_x, pos_y,
  //           string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  pos_y -= line_height;
//   tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
  // tex->DrawLatexNDC( pos_x, 0.955, "#it{#bf{sPHENIX}} Internal" );
  tex->DrawLatexNDC( pos_x, 0.955, "#it{#bf{sPHENIX}} Preliminary" );
  //tex->DrawLatexNDC( pos_x, pos_y, string( string("#it{#bf{sPHENIX}} Preliminary ") + "#it{" + GetDate() + "}").c_str() );
  //tex->DrawLatexNDC( pos_x, pos_y, string("#it{#bf{sPHENIX}} Preliminary #it{05/16/2025}").c_str() );

  tex->DrawLatexNDC( 0.77, 0.955, string("#it{08/21/2025}").c_str() );
  // p+p 200 GeV
  // pos_y -= line_height ;// + 0.03;
  tex->DrawLatexNDC( pos_x, pos_y, ("#it{Au+Au} #sqrt{s}_{NN} = 200 GeV") );

  int n_collision = GetNCollisionFromRunNum(run);
  int open_time = GetOpenTimeFromRunNum(run);

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, Form("n_collision: %d, open_time: %d", n_collision, open_time) );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "Before offline correction for carry-over hits" );
}

void PutCaption_AfterCorrection(int run, double first_margin = -0.1, double pos_x = 0.2, double pos_y = 0.9){
/*from Genki*/
  TLatex* tex = new TLatex();
  double line_height = 0.055; // no need to change
  // double first_margin = -0.1; // the margin from the top of the canvas to the top line (date). You can modify
  // double pos_x = 0.2; // you can modify
  // double pos_y = 1.0  + first_margin; // - line_height;

  /* Date, you may want to change 0.65 to pos_x */
  //tex->DrawLatexNDC( pos_x, pos_y,
  //           string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  // pos_y -= line_height;
//   tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
  // tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
  // tex->DrawLatexNDC( pos_x, 0.955, "#it{#bf{sPHENIX}} Preliminary" );
  //tex->DrawLatexNDC( pos_x, pos_y, string( string("#it{#bf{sPHENIX}} Preliminary ") + "#it{" + GetDate() + "}").c_str() );
  //tex->DrawLatexNDC( pos_x, pos_y, string("#it{#bf{sPHENIX}} Preliminary #it{05/16/2025}").c_str() );

  // tex->DrawLatexNDC( 0.77, 0.955, string("#it{08/21/2025}").c_str() );
  // p+p 200 GeV
  pos_y -= line_height ;// + 0.03;
  tex->DrawLatexNDC( pos_x, pos_y, ("#it{Au+Au} #sqrt{s}_{NN} = 200 GeV") );

  int n_collision = GetNCollisionFromRunNum(run);
  int open_time = GetOpenTimeFromRunNum(run);

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, Form("n_collision: %d, open_time: %d", n_collision, open_time) );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "After offline correction for carry-over hits" );

}

void PutCaption_BeforeCorrection(int run, double first_margin = -0.1, double pos_x = 0.2, double pos_y = 0.9){
/*from Genki*/
  TLatex* tex = new TLatex();
  double line_height = 0.055; // no need to change
  // double first_margin = -0.1; // the margin from the top of the canvas to the top line (date). You can modify
  // double pos_x = 0.2; // you can modify
  // double pos_y = 1.0  + first_margin; // - line_height;

  /* Date, you may want to change 0.65 to pos_x */
  //tex->DrawLatexNDC( pos_x, pos_y,
  //           string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  // pos_y -= line_height;
//   tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
  // tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
  // tex->DrawLatexNDC( pos_x, 0.955, "#it{#bf{sPHENIX}} Preliminary" );
  //tex->DrawLatexNDC( pos_x, pos_y, string( string("#it{#bf{sPHENIX}} Preliminary ") + "#it{" + GetDate() + "}").c_str() );
  //tex->DrawLatexNDC( pos_x, pos_y, string("#it{#bf{sPHENIX}} Preliminary #it{05/16/2025}").c_str() );

  // tex->DrawLatexNDC( 0.77, 0.955, string("#it{08/21/2025}").c_str() );
  // p+p 200 GeV
  pos_y -= line_height ;// + 0.03;
  tex->DrawLatexNDC( pos_x, pos_y, ("#it{Au+Au} #sqrt{s}_{NN} = 200 GeV") );

  int n_collision = GetNCollisionFromRunNum(run);
  int open_time = GetOpenTimeFromRunNum(run);

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, Form("n_collision: %d, open_time: %d", n_collision, open_time) );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "Before offline correction for carry-over hits" );

}


