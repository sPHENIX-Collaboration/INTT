// TTree には InttEvent クラスが詰まっている。InttEvent クラスのメンバーには InttCluster がある。
// これらを使うには以下の include 文が必要。
// クラスの実装部が必要なので .h ではなく .cc を取り込む必要がある。
#include "InttCluster.cc"
#include "InttEvent.cc"

int tree_analysis()
{

  // ROOT ファイルの読み込み
  TFile* tf = new TFile( "results/run_54688.root", "READ" );

  // ROOT ファイルから TTree を取得
  TTree* tree = (TTree*)tf->Get( "tree" );

  // 読み込んだ結果を格納する InttEvent オブジェクトを作る
  InttEvent* event = new InttEvent();

  // 格納オブジェクトと TTree の対応付けをする
  tree->SetBranchAddress( "event", &event );

  // 結果をいれるヒストグラムを作る
  TH1D* hist_adc = new TH1D( "hist_adc", "title", 30, 0, 900 );
  TH1D* hist_theta = new TH1D( "hist_theta", "Polar angle distribution;#theta (degree);Entries", 180, 0, 180 );

  // 処理するイベント数。ざっくり言って 1000 クラスター/イベント。1000 イベントの処理に 20, 30 秒かかる。
  int process_event_num = 100;

  // イベントループ開始
  for( int i=0; i<tree->GetEntries(); i++ )
    {

      // 進行状況表示
      cout << flush << right << "\r" << setw(10) << i;

      // i 番目のイベントを読み込む
      tree->GetEntry( i );
      // event->SetVerbosity( 2 ); // to see cluster information
      // event->Print();

      // このイベントにある全クラスターを取得
      auto clusters = event->GetClusters();

      // クラスターループ開始
      for( auto& cluster : clusters )
	{
	  // 天頂角を取得し degree 単位に変換
	  double theta = 180.0 * (cluster->GetTheta()) / TMath::Pi(); // It's actually |theta| (degree).

	  // このクラスターのパラメータをヒストグラムに格納
	  hist_theta->Fill( theta );
	  hist_adc->Fill( cluster->GetAdc() );
	}

      // 指定したイベント数でループ終了。全イベントを処理するには負の値でも指定しておけば良い
      if( i == process_event_num )
	{
	  cout << endl;
	  break;
	}
      
    }

  // 天頂各分布描画
  TCanvas* c_theta = new TCanvas();
  hist_theta->Draw();

  // ADC 分布描画
  TCanvas* c_adc = new TCanvas();
  hist_adc->Draw();
  return 0;
}
