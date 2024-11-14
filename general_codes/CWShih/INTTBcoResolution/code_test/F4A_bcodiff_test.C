int F4A_bcodiff_test()
{
    vector<int> out_felix0_bcodiff(128,0);


    int Nele = out_felix0_bcodiff.size();    
    int sort_entry_index[Nele];
    TMath::Sort(Nele, &out_felix0_bcodiff[0], sort_entry_index); // note : from big to small

    cout<<"sort_entry_index[0]: "<<sort_entry_index[0]<<endl;

    return 0;
}