Title: template
Author: Genki NUKAZUKA
Date: 2022/10/31
Email: nukadukagenki@gmail.com
CSS: https://filedn.com/lQst1YpnCbnQj7UxKv7MXJF/markdown.css
latex input:    mmd-article-header  
latex input:    mmd-article-begin-doc  
latex footer:   mmd-memoir-footer 
xhtml header:	<script async src="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.0/MathJax.js?config=TeX-AMS_CHTML"></script>


# README for data selection of the ELPH data
Order of the processes is:
1. trim_clone_hit.cc( "data/ELPH/", run_num, search_range, is_MC )
2. make_cluster.cc( run_num, is_MC )
3. make_bco_grouping( run_num, is_MC )
